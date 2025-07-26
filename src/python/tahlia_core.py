"""
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: tahlia_core.py
Description: Main Python module for Tahlia asset management system.
              Provides elegant Python APIs for asset importing, material creation,
              and history management with full Blender integration.
"""

import bpy
import bmesh
import mathutils
import json
import os
import sys
from typing import Dict, List, Optional, Tuple, Any
from pathlib import Path
import logging

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class TahliaCore:
    """
    Main Python interface for Tahlia asset management system.
    Provides elegant APIs for asset importing, material creation, and history management.
    """
    
    def __init__(self):
        """Initialize the Tahlia core system."""
        self.asset_manager = None
        self.import_manager = None
        self.material_manager = None
        self.import_history = None
        self.context_stack = []
        self.debug_mode = False
        self.context_preservation = True
        self.max_context_stack_size = 10
        
        # Initialize Blender-specific settings
        self.supported_formats = {
            'models': ['.blend', '.obj', '.fbx', '.dae', '.3ds', '.stl', '.ply'],
            'textures': ['.png', '.jpg', '.jpeg', '.tga', '.tiff', '.bmp', '.exr', '.hdr'],
            'audio': ['.mp3', '.wav', '.flac', '.aac', '.ogg', '.wma', '.m4a'],
            'video': ['.mp4', '.avi', '.mov', '.wmv', '.flv', '.webm', '.mkv']
        }
        
        logger.info("TahliaCore initialized successfully")
    
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
                'viewport_settings': {
                    'shading': bpy.context.space_data.shading.type if hasattr(bpy.context.space_data, 'shading') else 'SOLID',
                    'overlay': 'WIREFRAME' if bpy.context.space_data.overlay.show_wireframes else 'SOLID'
                },
                'visible_collections': [col.name for col in bpy.context.scene.collection.children if col.hide_viewport == False],
                'is_rendering': bpy.context.scene.render.is_rendering,
                'custom_state': {}
            }
            return context
        except Exception as e:
            logger.error(f"Failed to capture context: {e}")
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
            if hasattr(bpy.context.space_data, 'shading') and context.get('viewport_settings'):
                settings = context['viewport_settings']
                if 'shading' in settings:
                    bpy.context.space_data.shading.type = settings['shading']
            
            return True
        except Exception as e:
            logger.error(f"Failed to restore context: {e}")
            return False
    
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
    
    def preserve_context(self, operation: callable) -> bool:
        """
        Execute an operation while preserving the current Blender context.
        
        Args:
            operation: The operation to execute
            
        Returns:
            True if operation successful, False otherwise
        """
        if not self.context_preservation:
            operation()
            return True
        
        saved_context = self.capture_context()
        try:
            operation()
            return self.restore_context(saved_context)
        except Exception as e:
            logger.error(f"Operation failed: {e}")
            self.restore_context(saved_context)
            return False
    
    def import_asset(self, asset_path: str, options: Optional[Dict[str, str]] = None) -> Dict[str, Any]:
        """
        Import a single asset into Blender.
        
        Args:
            asset_path: Path to the asset file
            options: Import options
            
        Returns:
            Dict containing the import result
        """
        if options is None:
            options = {}
        
        def import_operation():
            return self._perform_import(asset_path, options)
        
        return self.preserve_context(import_operation)
    
    def _perform_import(self, asset_path: str, options: Dict[str, str]) -> Dict[str, Any]:
        """
        Perform the actual import operation.
        
        Args:
            asset_path: Path to the asset file
            options: Import options
            
        Returns:
            Dict containing the import result
        """
        try:
            if not os.path.exists(asset_path):
                return {
                    'success': False,
                    'message': f"Asset file not found: {asset_path}",
                    'data': {},
                    'list_data': []
                }
            
            file_ext = Path(asset_path).suffix.lower()
            
            # Handle different file formats
            if file_ext == '.blend':
                return self._import_blend_file(asset_path, options)
            elif file_ext == '.obj':
                return self._import_obj_file(asset_path, options)
            elif file_ext == '.fbx':
                return self._import_fbx_file(asset_path, options)
            elif file_ext == '.dae':
                return self._import_dae_file(asset_path, options)
            elif file_ext in ['.png', '.jpg', '.jpeg', '.tga', '.tiff', '.bmp', '.exr', '.hdr']:
                return self._import_texture(asset_path, options)
            else:
                return {
                    'success': False,
                    'message': f"Unsupported file format: {file_ext}",
                    'data': {},
                    'list_data': []
                }
                
        except Exception as e:
            logger.error(f"Import failed: {e}")
            return {
                'success': False,
                'message': f"Import failed: {str(e)}",
                'data': {},
                'list_data': []
            }
    
    def _import_blend_file(self, asset_path: str, options: Dict[str, str]) -> Dict[str, Any]:
        """Import a Blender file."""
        try:
            # Link/append from the blend file
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
            
            return {
                'success': True,
                'message': f"Imported {len(imported_objects)} objects from {asset_path}",
                'data': {
                    'imported_objects': ','.join(imported_objects),
                    'file_path': asset_path,
                    'file_type': 'blend'
                },
                'list_data': imported_objects
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to import blend file: {e}",
                'data': {},
                'list_data': []
            }
    
    def _import_obj_file(self, asset_path: str, options: Dict[str, str]) -> Dict[str, Any]:
        """Import an OBJ file."""
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
            
            return {
                'success': True,
                'message': f"Imported OBJ file: {asset_path}",
                'data': {
                    'imported_objects': ','.join(imported_objects),
                    'file_path': asset_path,
                    'file_type': 'obj'
                },
                'list_data': imported_objects
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to import OBJ file: {e}",
                'data': {},
                'list_data': []
            }
    
    def _import_fbx_file(self, asset_path: str, options: Dict[str, str]) -> Dict[str, Any]:
        """Import an FBX file."""
        try:
            bpy.ops.import_scene.fbx(filepath=asset_path)
            
            imported_objects = [obj.name for obj in bpy.context.selected_objects]
            
            return {
                'success': True,
                'message': f"Imported FBX file: {asset_path}",
                'data': {
                    'imported_objects': ','.join(imported_objects),
                    'file_path': asset_path,
                    'file_type': 'fbx'
                },
                'list_data': imported_objects
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to import FBX file: {e}",
                'data': {},
                'list_data': []
            }
    
    def _import_dae_file(self, asset_path: str, options: Dict[str, str]) -> Dict[str, Any]:
        """Import a Collada (DAE) file."""
        try:
            bpy.ops.wm.collada_import(filepath=asset_path)
            
            imported_objects = [obj.name for obj in bpy.context.selected_objects]
            
            return {
                'success': True,
                'message': f"Imported Collada file: {asset_path}",
                'data': {
                    'imported_objects': ','.join(imported_objects),
                    'file_path': asset_path,
                    'file_type': 'dae'
                },
                'list_data': imported_objects
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to import Collada file: {e}",
                'data': {},
                'list_data': []
            }
    
    def _import_texture(self, asset_path: str, options: Dict[str, str]) -> Dict[str, Any]:
        """Import a texture file."""
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
            
            return {
                'success': True,
                'message': f"Imported texture: {asset_path}",
                'data': {
                    'material_name': material_name,
                    'file_path': asset_path,
                    'file_type': 'texture'
                },
                'list_data': [material_name]
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to import texture: {e}",
                'data': {},
                'list_data': []
            }
    
    def import_assets_grid(self, asset_paths: List[str], options: Optional[Dict[str, str]] = None, 
                          rows: int = 1, cols: int = 1, spacing: float = 5.0) -> List[Dict[str, Any]]:
        """
        Import multiple assets in a grid pattern.
        
        Args:
            asset_paths: List of asset file paths
            options: Import options
            rows: Number of rows
            cols: Number of columns
            spacing: Spacing between assets
            
        Returns:
            List of import results
        """
        if options is None:
            options = {}
        
        results = []
        
        def grid_operation():
            for i, asset_path in enumerate(asset_paths):
                if i >= rows * cols:
                    break
                
                row = i // cols
                col = i % cols
                
                # Calculate position
                x = col * spacing
                y = row * spacing
                
                # Import the asset
                result = self._perform_import(asset_path, options)
                
                # Position the imported objects
                if result['success']:
                    for obj_name in result['list_data']:
                        if obj_name in bpy.data.objects:
                            obj = bpy.data.objects[obj_name]
                            obj.location = (x, y, 0)
                
                results.append(result)
        
        self.preserve_context(grid_operation)
        return results
    
    def import_assets_circle(self, asset_paths: List[str], options: Optional[Dict[str, str]] = None, 
                            radius: float = 10.0) -> List[Dict[str, Any]]:
        """
        Import multiple assets in a circle pattern.
        
        Args:
            asset_paths: List of asset file paths
            options: Import options
            radius: Radius of the circle
            
        Returns:
            List of import results
        """
        if options is None:
            options = {}
        
        results = []
        
        def circle_operation():
            for i, asset_path in enumerate(asset_paths):
                # Calculate position on circle
                angle = (2 * 3.14159 * i) / len(asset_paths)
                x = radius * math.cos(angle)
                y = radius * math.sin(angle)
                
                # Import the asset
                result = self._perform_import(asset_path, options)
                
                # Position the imported objects
                if result['success']:
                    for obj_name in result['list_data']:
                        if obj_name in bpy.data.objects:
                            obj = bpy.data.objects[obj_name]
                            obj.location = (x, y, 0)
                
                results.append(result)
        
        self.preserve_context(circle_operation)
        return results
    
    def import_assets_line(self, asset_paths: List[str], options: Optional[Dict[str, str]] = None, 
                          spacing: float = 5.0) -> List[Dict[str, Any]]:
        """
        Import multiple assets in a line pattern.
        
        Args:
            asset_paths: List of asset file paths
            options: Import options
            spacing: Spacing between assets
            
        Returns:
            List of import results
        """
        if options is None:
            options = {}
        
        results = []
        
        def line_operation():
            for i, asset_path in enumerate(asset_paths):
                # Calculate position
                x = i * spacing
                
                # Import the asset
                result = self._perform_import(asset_path, options)
                
                # Position the imported objects
                if result['success']:
                    for obj_name in result['list_data']:
                        if obj_name in bpy.data.objects:
                            obj = bpy.data.objects[obj_name]
                            obj.location = (x, 0, 0)
                
                results.append(result)
        
        self.preserve_context(line_operation)
        return results
    
    def create_material(self, name: str, options: Optional[Dict[str, str]] = None) -> Dict[str, Any]:
        """
        Create a new material.
        
        Args:
            name: Material name
            options: Material creation options
            
        Returns:
            Dict containing the material creation result
        """
        if options is None:
            options = {}
        
        try:
            material = bpy.data.materials.new(name=name)
            material.use_nodes = True
            
            # Apply material options
            if 'metallic' in options:
                material.node_tree.nodes['Principled BSDF'].inputs['Metallic'].default_value = float(options['metallic'])
            if 'roughness' in options:
                material.node_tree.nodes['Principled BSDF'].inputs['Roughness'].default_value = float(options['roughness'])
            if 'base_color' in options:
                # Parse color (assuming format like "1.0,0.5,0.2")
                color_values = [float(x) for x in options['base_color'].split(',')]
                material.node_tree.nodes['Principled BSDF'].inputs['Base Color'].default_value = (*color_values, 1.0)
            
            return {
                'success': True,
                'message': f"Created material: {name}",
                'data': {
                    'material_name': name,
                    'material_type': 'principled'
                },
                'list_data': [name]
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to create material: {e}",
                'data': {},
                'list_data': []
            }
    
    def create_pbr_material(self, name: str, options: Optional[Dict[str, str]] = None) -> Dict[str, Any]:
        """
        Create a PBR material with advanced settings.
        
        Args:
            name: Material name
            options: PBR material options
            
        Returns:
            Dict containing the material creation result
        """
        if options is None:
            options = {}
        
        try:
            material = bpy.data.materials.new(name=name)
            material.use_nodes = True
            
            # Get the principled BSDF node
            principled = material.node_tree.nodes['Principled BSDF']
            
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
            
            return {
                'success': True,
                'message': f"Created PBR material: {name}",
                'data': {
                    'material_name': name,
                    'material_type': 'pbr'
                },
                'list_data': [name]
            }
        except Exception as e:
            return {
                'success': False,
                'message': f"Failed to create PBR material: {e}",
                'data': {},
                'list_data': []
            }
    
    def get_supported_formats(self) -> Dict[str, List[str]]:
        """
        Get the list of supported file formats.
        
        Returns:
            Dict containing supported formats by category
        """
        return self.supported_formats
    
    def set_debug_mode(self, enable: bool) -> None:
        """
        Enable or disable debug mode.
        
        Args:
            enable: True to enable debug mode, False to disable
        """
        self.debug_mode = enable
        if enable:
            logging.getLogger().setLevel(logging.DEBUG)
        else:
            logging.getLogger().setLevel(logging.INFO)
    
    def set_context_preservation(self, enable: bool) -> None:
        """
        Enable or disable context preservation.
        
        Args:
            enable: True to enable context preservation, False to disable
        """
        self.context_preservation = enable
    
    def set_max_context_stack_size(self, max_size: int) -> None:
        """
        Set the maximum context stack size.
        
        Args:
            max_size: Maximum number of contexts to store
        """
        self.max_context_stack_size = max_size

# Create a global instance
tahlia = TahliaCore()

# Export the main functions for easy access
def import_asset(asset_path: str, options: Optional[Dict[str, str]] = None) -> Dict[str, Any]:
    """Import a single asset."""
    return tahlia.import_asset(asset_path, options)

def import_assets_grid(asset_paths: List[str], options: Optional[Dict[str, str]] = None, 
                      rows: int = 1, cols: int = 1, spacing: float = 5.0) -> List[Dict[str, Any]]:
    """Import multiple assets in a grid pattern."""
    return tahlia.import_assets_grid(asset_paths, options, rows, cols, spacing)

def import_assets_circle(asset_paths: List[str], options: Optional[Dict[str, str]] = None, 
                        radius: float = 10.0) -> List[Dict[str, Any]]:
    """Import multiple assets in a circle pattern."""
    return tahlia.import_assets_circle(asset_paths, options, radius)

def import_assets_line(asset_paths: List[str], options: Optional[Dict[str, str]] = None, 
                      spacing: float = 5.0) -> List[Dict[str, Any]]:
    """Import multiple assets in a line pattern."""
    return tahlia.import_assets_line(asset_paths, options, spacing)

def create_material(name: str, options: Optional[Dict[str, str]] = None) -> Dict[str, Any]:
    """Create a new material."""
    return tahlia.create_material(name, options)

def create_pbr_material(name: str, options: Optional[Dict[str, str]] = None) -> Dict[str, Any]:
    """Create a PBR material."""
    return tahlia.create_pbr_material(name, options)

def get_supported_formats() -> Dict[str, List[str]]:
    """Get supported file formats."""
    return tahlia.get_supported_formats()

# Version info
__version__ = "1.0.0"
__author__ = "KleaSCM"
__email__ = "KleaSCM@gmail.com" 