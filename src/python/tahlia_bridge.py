"""
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: tahlia_bridge.py
Description: Python bridge module that provides the interface between C++ core and Python.
              Handles Blender API calls, data conversion, and provides the actual implementation
              for the PythonBridge C++ class methods.
"""

import bpy
import bmesh
import mathutils
import json
import os
import sys
from typing import Dict, List, Optional, Tuple, Any, Union
from pathlib import Path
import logging
import traceback

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class TahliaBridge:
    """
    Python bridge implementation that provides the actual Blender API integration.
    This class implements the methods that the C++ PythonBridge class calls.
    """
    
    def __init__(self):
        """Initialize the Tahlia bridge."""
        self.context_stack = []
        self.debug_mode = False
        self.context_preservation = True
        self.max_context_stack_size = 10
        self.last_error = ""
        
        logger.info("TahliaBridge initialized")
    
    def capture_context(self) -> Dict[str, Any]:
        """
        Capture the current Blender context state.
        
        Returns:
            Dict containing the current Blender context state
        """
        try:
            context = {
                'selected_objects': [obj.name for obj in bpy.context.selected_objects],
                'active_object': bpy.context.active_object.name if bpy.context.active_object else "",
                'view_layer': bpy.context.view_layer.name,
                'mode': bpy.context.mode,
                'viewport_settings': self._capture_viewport_settings(),
                'visible_collections': self._capture_visible_collections(),
                'is_rendering': bpy.context.scene.render.is_rendering,
                'custom_state': self._capture_custom_state()
            }
            return context
        except Exception as e:
            self.last_error = f"Failed to capture context: {e}"
            logger.error(self.last_error)
            return {}
    
    def _capture_viewport_settings(self) -> Dict[str, str]:
        """Capture viewport settings."""
        settings = {}
        try:
            if hasattr(bpy.context.space_data, 'shading'):
                settings['shading'] = bpy.context.space_data.shading.type
            if hasattr(bpy.context.space_data, 'overlay'):
                settings['overlay'] = 'WIREFRAME' if bpy.context.space_data.overlay.show_wireframes else 'SOLID'
        except:
            pass
        return settings
    
    def _capture_visible_collections(self) -> List[str]:
        """Capture visible collections."""
        try:
            return [col.name for col in bpy.context.scene.collection.children if not col.hide_viewport]
        except:
            return []
    
    def _capture_custom_state(self) -> Dict[str, str]:
        """Capture custom state information."""
        try:
            return {
                'scene_name': bpy.context.scene.name,
                'frame_current': str(bpy.context.scene.frame_current),
                'render_engine': bpy.context.scene.render.engine
            }
        except:
            return {}
    
    def restore_context(self, context: Dict[str, Any]) -> bool:
        """
        Restore a previously captured Blender context state.
        
        Args:
            context: The context state to restore
            
        Returns:
            True if restoration successful, False otherwise
        """
        try:
            # Restore active object
            if context.get('active_object'):
                if context['active_object'] in bpy.data.objects:
                    bpy.context.view_layer.objects.active = bpy.data.objects[context['active_object']]
            
            # Restore selected objects
            if context.get('selected_objects'):
                # Clear current selection
                bpy.ops.object.select_all(action='DESELECT')
                # Select objects from context
                for obj_name in context['selected_objects']:
                    if obj_name in bpy.data.objects:
                        bpy.data.objects[obj_name].select_set(True)
            
            # Restore mode
            if context.get('mode') and bpy.context.active_object:
                try:
                    bpy.ops.object.mode_set(mode=context['mode'])
                except:
                    pass  # Mode might not be available
            
            # Restore viewport settings
            self._restore_viewport_settings(context.get('viewport_settings', {}))
            
            # Restore visible collections
            self._restore_visible_collections(context.get('visible_collections', []))
            
            return True
        except Exception as e:
            self.last_error = f"Failed to restore context: {e}"
            logger.error(self.last_error)
            return False
    
    def _restore_viewport_settings(self, settings: Dict[str, str]) -> None:
        """Restore viewport settings."""
        try:
            if hasattr(bpy.context.space_data, 'shading') and 'shading' in settings:
                bpy.context.space_data.shading.type = settings['shading']
        except:
            pass
    
    def _restore_visible_collections(self, collections: List[str]) -> None:
        """Restore visible collections."""
        try:
            for col in bpy.context.scene.collection.children:
                col.hide_viewport = col.name not in collections
        except:
            pass
    
    def push_context(self) -> bool:
        """
        Push the current context onto the context stack.
        
        Returns:
            True if successful, False otherwise
        """
        if len(self.context_stack) < self.max_context_stack_size:
            context = self.capture_context()
            self.context_stack.append(context)
            return True
        return False
    
    def pop_context(self) -> bool:
        """
        Pop and restore the top context from the context stack.
        
        Returns:
            True if successful, False otherwise
        """
        if self.context_stack:
            context = self.context_stack.pop()
            return self.restore_context(context)
        return False
    
    def clear_context_stack(self) -> None:
        """Clear the context stack."""
        self.context_stack.clear()
    
    def get_context_stack_size(self) -> int:
        """
        Get the current context stack size.
        
        Returns:
            Number of contexts in the stack
        """
        return len(self.context_stack)
    
    def preserve_context(self, operation: callable) -> bool:
        """
        Execute an operation while preserving the current Blender context.
        
        Args:
            operation: The operation to execute
            
        Returns:
            True if operation successful, False otherwise
        """
        if not self.context_preservation:
            try:
                operation()
                return True
            except Exception as e:
                self.last_error = f"Operation failed: {e}"
                logger.error(self.last_error)
                return False
        
        saved_context = self.capture_context()
        try:
            operation()
            return self.restore_context(saved_context)
        except Exception as e:
            self.last_error = f"Operation failed: {e}"
            logger.error(self.last_error)
            self.restore_context(saved_context)
            return False
    
    def import_asset_blender(self, asset_path: str, options: Dict[str, str]) -> Dict[str, Any]:
        """
        Import an asset using Blender's native import operators.
        
        Args:
            asset_path: Path to the asset file
            options: Import options
            
        Returns:
            Dict containing the import result
        """
        try:
            if not os.path.exists(asset_path):
                return self._create_error_result(f"Asset file not found: {asset_path}")
            
            file_ext = Path(asset_path).suffix.lower()
            
            # Handle different file formats
            if file_ext == '.blend':
                return self._import_blend_file_blender(asset_path, options)
            elif file_ext == '.obj':
                return self._import_obj_file_blender(asset_path, options)
            elif file_ext == '.fbx':
                return self._import_fbx_file_blender(asset_path, options)
            elif file_ext == '.dae':
                return self._import_dae_file_blender(asset_path, options)
            elif file_ext in ['.png', '.jpg', '.jpeg', '.tga', '.tiff', '.bmp', '.exr', '.hdr']:
                return self._import_texture_blender(asset_path, options)
            else:
                return self._create_error_result(f"Unsupported file format: {file_ext}")
                
        except Exception as e:
            return self._create_error_result(f"Import failed: {str(e)}")
    
    def _import_blend_file_blender(self, asset_path: str, options: Dict[str, str]) -> Dict[str, Any]:
        """Import a Blender file using Blender's library system."""
        try:
            link = options.get('link', 'False').lower() == 'true'
            
            with bpy.data.libraries.load(asset_path, link=link) as (data_from, data_to):
                # Import all objects
                data_to.objects = data_from.objects
            
            # Add objects to the scene
            imported_objects = []
            for obj in data_to.objects:
                if obj is not None:
                    bpy.context.scene.collection.objects.link(obj)
                    imported_objects.append(obj.name)
            
            return self._create_success_result(
                f"Imported {len(imported_objects)} objects from {asset_path}",
                {
                    'imported_objects': ','.join(imported_objects),
                    'file_path': asset_path,
                    'file_type': 'blend'
                },
                imported_objects
            )
        except Exception as e:
            return self._create_error_result(f"Failed to import blend file: {e}")
    
    def _import_obj_file_blender(self, asset_path: str, options: Dict[str, str]) -> Dict[str, Any]:
        """Import an OBJ file using Blender's OBJ importer."""
        try:
            # Set import options
            bpy.ops.import_scene.obj(
                filepath=asset_path,
                use_edges=True,
                use_smooth_groups=True,
                use_split_objects=True,
                use_split_groups=True,
                use_groups_as_vgroups=False,
                use_image_search=True,
                split_mode='ON',
                global_clamp_size=0.0
            )
            
            # Get imported objects
            imported_objects = [obj.name for obj in bpy.context.selected_objects]
            
            return self._create_success_result(
                f"Imported OBJ file: {asset_path}",
                {
                    'imported_objects': ','.join(imported_objects),
                    'file_path': asset_path,
                    'file_type': 'obj'
                },
                imported_objects
            )
        except Exception as e:
            return self._create_error_result(f"Failed to import OBJ file: {e}")
    
    def _import_fbx_file_blender(self, asset_path: str, options: Dict[str, str]) -> Dict[str, Any]:
        """Import an FBX file using Blender's FBX importer."""
        try:
            bpy.ops.import_scene.fbx(filepath=asset_path)
            
            imported_objects = [obj.name for obj in bpy.context.selected_objects]
            
            return self._create_success_result(
                f"Imported FBX file: {asset_path}",
                {
                    'imported_objects': ','.join(imported_objects),
                    'file_path': asset_path,
                    'file_type': 'fbx'
                },
                imported_objects
            )
        except Exception as e:
            return self._create_error_result(f"Failed to import FBX file: {e}")
    
    def _import_dae_file_blender(self, asset_path: str, options: Dict[str, str]) -> Dict[str, Any]:
        """Import a Collada (DAE) file using Blender's Collada importer."""
        try:
            bpy.ops.wm.collada_import(filepath=asset_path)
            
            imported_objects = [obj.name for obj in bpy.context.selected_objects]
            
            return self._create_success_result(
                f"Imported Collada file: {asset_path}",
                {
                    'imported_objects': ','.join(imported_objects),
                    'file_path': asset_path,
                    'file_type': 'dae'
                },
                imported_objects
            )
        except Exception as e:
            return self._create_error_result(f"Failed to import Collada file: {e}")
    
    def _import_texture_blender(self, asset_path: str, options: Dict[str, str]) -> Dict[str, Any]:
        """Import a texture file and create a material."""
        try:
            # Create a new material
            material_name = options.get('material_name', Path(asset_path).stem)
            material = bpy.data.materials.new(name=material_name)
            material.use_nodes = True
            
            # Get the principled BSDF node
            principled = material.node_tree.nodes.get('Principled BSDF')
            if principled:
                # Load the texture
                texture_node = material.node_tree.nodes.new('ShaderNodeTexImage')
                texture_node.image = bpy.data.images.load(asset_path)
                
                # Connect to base color
                material.node_tree.links.new(
                    texture_node.outputs['Color'],
                    principled.inputs['Base Color']
                )
            
            return self._create_success_result(
                f"Imported texture: {asset_path}",
                {
                    'material_name': material_name,
                    'file_path': asset_path,
                    'file_type': 'texture'
                },
                [material_name]
            )
        except Exception as e:
            return self._create_error_result(f"Failed to import texture: {e}")
    
    def create_material_blender(self, name: str, options: Dict[str, str]) -> Dict[str, Any]:
        """
        Create a new material using Blender's material system.
        
        Args:
            name: Material name
            options: Material creation options
            
        Returns:
            Dict containing the material creation result
        """
        try:
            material = bpy.data.materials.new(name=name)
            material.use_nodes = True
            
            # Get the principled BSDF node
            principled = material.node_tree.nodes.get('Principled BSDF')
            if principled:
                # Apply material options
                if 'metallic' in options:
                    principled.inputs['Metallic'].default_value = float(options['metallic'])
                if 'roughness' in options:
                    principled.inputs['Roughness'].default_value = float(options['roughness'])
                if 'base_color' in options:
                    # Parse color (assuming format like "1.0,0.5,0.2")
                    color_values = [float(x) for x in options['base_color'].split(',')]
                    principled.inputs['Base Color'].default_value = (*color_values, 1.0)
                if 'specular' in options:
                    principled.inputs['Specular'].default_value = float(options['specular'])
                if 'ior' in options:
                    principled.inputs['IOR'].default_value = float(options['ior'])
                if 'transmission' in options:
                    principled.inputs['Transmission'].default_value = float(options['transmission'])
                if 'alpha' in options:
                    principled.inputs['Alpha'].default_value = float(options['alpha'])
            
            return self._create_success_result(
                f"Created material: {name}",
                {
                    'material_name': name,
                    'material_type': 'principled'
                },
                [name]
            )
        except Exception as e:
            return self._create_error_result(f"Failed to create material: {e}")
    
    def create_pbr_material_blender(self, name: str, options: Dict[str, str]) -> Dict[str, Any]:
        """
        Create a PBR material with advanced settings.
        
        Args:
            name: Material name
            options: PBR material options
            
        Returns:
            Dict containing the material creation result
        """
        try:
            material = bpy.data.materials.new(name=name)
            material.use_nodes = True
            
            # Get the principled BSDF node
            principled = material.node_tree.nodes.get('Principled BSDF')
            if principled:
                # Apply PBR settings
                if 'metallic' in options:
                    principled.inputs['Metallic'].default_value = float(options['metallic'])
                if 'roughness' in options:
                    principled.inputs['Roughness'].default_value = float(options['roughness'])
                if 'specular' in options:
                    principled.inputs['Specular'].default_value = float(options['specular'])
                if 'ior' in options:
                    principled.inputs['IOR'].default_value = float(options['ior'])
                if 'transmission' in options:
                    principled.inputs['Transmission'].default_value = float(options['transmission'])
                if 'alpha' in options:
                    principled.inputs['Alpha'].default_value = float(options['alpha'])
                if 'emission_strength' in options:
                    principled.inputs['Emission Strength'].default_value = float(options['emission_strength'])
                if 'subsurface' in options:
                    principled.inputs['Subsurface'].default_value = float(options['subsurface'])
            
            return self._create_success_result(
                f"Created PBR material: {name}",
                {
                    'material_name': name,
                    'material_type': 'pbr'
                },
                [name]
            )
        except Exception as e:
            return self._create_error_result(f"Failed to create PBR material: {e}")
    
    def get_supported_formats_blender(self) -> Dict[str, List[str]]:
        """
        Get the list of supported file formats in Blender.
        
        Returns:
            Dict containing supported formats by category
        """
        return {
            'models': ['.blend', '.obj', '.fbx', '.dae', '.3ds', '.stl', '.ply'],
            'textures': ['.png', '.jpg', '.jpeg', '.tga', '.tiff', '.bmp', '.exr', '.hdr'],
            'audio': ['.mp3', '.wav', '.flac', '.aac', '.ogg', '.wma', '.m4a'],
            'video': ['.mp4', '.avi', '.mov', '.wmv', '.flv', '.webm', '.mkv']
        }
    
    def _create_success_result(self, message: str, data: Dict[str, str], list_data: List[str]) -> Dict[str, Any]:
        """Create a success result."""
        return {
            'success': True,
            'message': message,
            'data': data,
            'list_data': list_data
        }
    
    def _create_error_result(self, message: str) -> Dict[str, Any]:
        """Create an error result."""
        return {
            'success': False,
            'message': message,
            'data': {},
            'list_data': []
        }
    
    def set_debug_mode(self, enable: bool) -> None:
        """Enable or disable debug mode."""
        self.debug_mode = enable
        if enable:
            logging.getLogger().setLevel(logging.DEBUG)
        else:
            logging.getLogger().setLevel(logging.INFO)
    
    def set_context_preservation(self, enable: bool) -> None:
        """Enable or disable context preservation."""
        self.context_preservation = enable
    
    def set_max_context_stack_size(self, max_size: int) -> None:
        """Set the maximum context stack size."""
        self.max_context_stack_size = max_size
    
    def get_last_error(self) -> str:
        """Get the last error message."""
        return self.last_error
    
    def clear_last_error(self) -> None:
        """Clear the last error message."""
        self.last_error = ""

# Create a global instance
bridge = TahliaBridge()

# Export functions for C++ integration
def capture_context() -> Dict[str, Any]:
    """Capture the current Blender context."""
    return bridge.capture_context()

def restore_context(context: Dict[str, Any]) -> bool:
    """Restore a Blender context."""
    return bridge.restore_context(context)

def push_context() -> bool:
    """Push context onto stack."""
    return bridge.push_context()

def pop_context() -> bool:
    """Pop context from stack."""
    return bridge.pop_context()

def clear_context_stack() -> None:
    """Clear the context stack."""
    bridge.clear_context_stack()

def get_context_stack_size() -> int:
    """Get context stack size."""
    return bridge.get_context_stack_size()

def preserve_context(operation: callable) -> bool:
    """Preserve context during operation."""
    return bridge.preserve_context(operation)

def import_asset_blender(asset_path: str, options: Dict[str, str]) -> Dict[str, Any]:
    """Import an asset using Blender."""
    return bridge.import_asset_blender(asset_path, options)

def create_material_blender(name: str, options: Dict[str, str]) -> Dict[str, Any]:
    """Create a material using Blender."""
    return bridge.create_material_blender(name, options)

def create_pbr_material_blender(name: str, options: Dict[str, str]) -> Dict[str, Any]:
    """Create a PBR material using Blender."""
    return bridge.create_pbr_material_blender(name, options)

def get_supported_formats_blender() -> Dict[str, List[str]]:
    """Get supported formats in Blender."""
    return bridge.get_supported_formats_blender()

def set_debug_mode(enable: bool) -> None:
    """Set debug mode."""
    bridge.set_debug_mode(enable)

def set_context_preservation(enable: bool) -> None:
    """Set context preservation."""
    bridge.set_context_preservation(enable)

def set_max_context_stack_size(max_size: int) -> None:
    """Set max context stack size."""
    bridge.set_max_context_stack_size(max_size)

def get_last_error() -> str:
    """Get last error."""
    return bridge.get_last_error()

def clear_last_error() -> None:
    """Clear last error."""
    bridge.clear_last_error() 