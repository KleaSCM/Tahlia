#!/usr/bin/env python3
"""
Material utilities for Blender asset management
Author: KleaSCM
Email: KleaSCM@gmail.com
Description: Python utilities for material creation and management in Blender
"""

import bpy
import sys
import os
import json
from typing import Dict, Any, Optional, List

def create_material(options: Dict[str, Any]) -> Dict[str, Any]:
    """
    Create a material with the specified options
    
    Args:
        options: Dictionary containing material options
        
    Returns:
        Dictionary with result information
    """
    try:
        # Create material
        mat = bpy.data.materials.new(name=options.get('name', 'NewMaterial'))
        mat.use_nodes = options.get('use_nodes', True)
        
        if mat.use_nodes:
            nodes = mat.node_tree.nodes
            links = mat.node_tree.links
            
            # Clear default nodes
            nodes.clear()
            
            # Create Principled BSDF
            principled = nodes.new('ShaderNodeBsdfPrincipled')
            principled.location = (0, 0)
            
            # Set PBR parameters
            principled.inputs['Metallic'].default_value = options.get('metallic', 0.0)
            principled.inputs['Roughness'].default_value = options.get('roughness', 0.5)
            principled.inputs['Specular'].default_value = options.get('specular', 0.5)
            principled.inputs['Clearcoat'].default_value = options.get('clearcoat', 0.0)
            principled.inputs['Clearcoat Roughness'].default_value = options.get('clearcoat_roughness', 0.0)
            principled.inputs['IOR'].default_value = options.get('ior', 1.45)
            principled.inputs['Transmission'].default_value = options.get('transmission', 0.0)
            principled.inputs['Transmission Roughness'].default_value = options.get('transmission_roughness', 0.0)
            principled.inputs['Emission Strength'].default_value = options.get('emission_strength', 0.0)
            principled.inputs['Alpha'].default_value = options.get('alpha', 1.0)
            
            # Create Material Output
            output = nodes.new('ShaderNodeOutputMaterial')
            output.location = (300, 0)
            
            # Link nodes
            links.new(principled.outputs['BSDF'], output.inputs['Surface'])
            
            # Add texture nodes if textures are specified
            texture_nodes = {}
            
            # Albedo texture
            if options.get('albedo_texture'):
                tex = nodes.new('ShaderNodeTexImage')
                tex.location = (-300, 200)
                tex.image = bpy.data.images.load(options['albedo_texture'])
                links.new(tex.outputs['Color'], principled.inputs['Base Color'])
                texture_nodes['albedo'] = tex
            
            # Normal texture
            if options.get('normal_texture'):
                tex = nodes.new('ShaderNodeTexImage')
                tex.location = (-300, 0)
                tex.image = bpy.data.images.load(options['normal_texture'])
                normal_map = nodes.new('ShaderNodeNormalMap')
                normal_map.location = (-100, 0)
                links.new(tex.outputs['Color'], normal_map.inputs['Color'])
                links.new(normal_map.outputs['Normal'], principled.inputs['Normal'])
                texture_nodes['normal'] = tex
            
            # Roughness texture
            if options.get('roughness_texture'):
                tex = nodes.new('ShaderNodeTexImage')
                tex.location = (-300, -200)
                tex.image = bpy.data.images.load(options['roughness_texture'])
                links.new(tex.outputs['Color'], principled.inputs['Roughness'])
                texture_nodes['roughness'] = tex
            
            # Metallic texture
            if options.get('metallic_texture'):
                tex = nodes.new('ShaderNodeTexImage')
                tex.location = (-300, -400)
                tex.image = bpy.data.images.load(options['metallic_texture'])
                links.new(tex.outputs['Color'], principled.inputs['Metallic'])
                texture_nodes['metallic'] = tex
            
            # AO texture
            if options.get('ao_texture'):
                tex = nodes.new('ShaderNodeTexImage')
                tex.location = (-300, -600)
                tex.image = bpy.data.images.load(options['ao_texture'])
                mix_rgb = nodes.new('ShaderNodeMixRGB')
                mix_rgb.location = (-100, 200)
                mix_rgb.blend_type = 'MULTIPLY'
                links.new(tex.outputs['Color'], mix_rgb.inputs[2])
                if 'albedo' in texture_nodes:
                    links.new(texture_nodes['albedo'].outputs['Color'], mix_rgb.inputs[1])
                    links.new(mix_rgb.outputs['Color'], principled.inputs['Base Color'])
                texture_nodes['ao'] = tex
            
            # Emission texture
            if options.get('emission_texture'):
                tex = nodes.new('ShaderNodeTexImage')
                tex.location = (-300, -800)
                tex.image = bpy.data.images.load(options['emission_texture'])
                links.new(tex.outputs['Color'], principled.inputs['Emission Color'])
                texture_nodes['emission'] = tex
            
            # Displacement texture
            if options.get('displacement_texture'):
                tex = nodes.new('ShaderNodeTexImage')
                tex.location = (-300, -1000)
                tex.image = bpy.data.images.load(options['displacement_texture'])
                disp = nodes.new('ShaderNodeDisplacement')
                disp.location = (-100, -1000)
                links.new(tex.outputs['Color'], disp.inputs['Height'])
                links.new(disp.outputs['Displacement'], output.inputs['Displacement'])
                texture_nodes['displacement'] = tex
        
        # Set material settings
        mat.use_backface_culling = options.get('backface_culling', False)
        
        if options.get('blend_method', False):
            mat.blend_method = 'BLEND'
        
        return {
            'success': True,
            'material_name': mat.name,
            'message': 'Material created successfully'
        }
        
    except Exception as e:
        return {
            'success': False,
            'message': f'Failed to create material: {str(e)}'
        }

def assign_texture(material_name: str, texture_path: str, texture_type: str) -> Dict[str, Any]:
    """
    Assign a texture to a specific material and texture type
    
    Args:
        material_name: Name of the material
        texture_path: Path to the texture file
        texture_type: Type of texture (albedo, normal, roughness, etc.)
        
    Returns:
        Dictionary with result information
    """
    try:
        mat = bpy.data.materials.get(material_name)
        if not mat:
            return {
                'success': False,
                'message': 'Material not found'
            }
        
        if not mat.use_nodes:
            mat.use_nodes = True
        
        nodes = mat.node_tree.nodes
        links = mat.node_tree.links
        
        # Find Principled BSDF
        principled = None
        for node in nodes:
            if node.type == 'BSDF_PRINCIPLED':
                principled = node
                break
        
        if not principled:
            return {
                'success': False,
                'message': 'No Principled BSDF found'
            }
        
        # Create texture node
        tex = nodes.new('ShaderNodeTexImage')
        tex.image = bpy.data.images.load(texture_path)
        
        # Connect based on texture type
        if texture_type == 'albedo':
            links.new(tex.outputs['Color'], principled.inputs['Base Color'])
        elif texture_type == 'normal':
            normal_map = nodes.new('ShaderNodeNormalMap')
            links.new(tex.outputs['Color'], normal_map.inputs['Color'])
            links.new(normal_map.outputs['Normal'], principled.inputs['Normal'])
        elif texture_type == 'roughness':
            links.new(tex.outputs['Color'], principled.inputs['Roughness'])
        elif texture_type == 'metallic':
            links.new(tex.outputs['Color'], principled.inputs['Metallic'])
        elif texture_type == 'ao':
            mix_rgb = nodes.new('ShaderNodeMixRGB')
            mix_rgb.blend_type = 'MULTIPLY'
            links.new(tex.outputs['Color'], mix_rgb.inputs[2])
            # Find existing albedo connection
            for link in links:
                if link.to_node == principled and link.to_socket.name == 'Base Color':
                    links.new(link.from_socket, mix_rgb.inputs[1])
                    links.new(mix_rgb.outputs['Color'], principled.inputs['Base Color'])
                    break
        elif texture_type == 'emission':
            links.new(tex.outputs['Color'], principled.inputs['Emission Color'])
        elif texture_type == 'displacement':
            disp = nodes.new('ShaderNodeDisplacement')
            links.new(tex.outputs['Color'], disp.inputs['Height'])
            # Find Material Output
            for node in nodes:
                if node.type == 'OUTPUT_MATERIAL':
                    links.new(disp.outputs['Displacement'], node.inputs['Displacement'])
                    break
        
        return {
            'success': True,
            'message': 'Texture assigned successfully'
        }
        
    except Exception as e:
        return {
            'success': False,
            'message': f'Failed to assign texture: {str(e)}'
        }

def load_texture_info(texture_path: str) -> Dict[str, Any]:
    """
    Load and analyze texture information
    
    Args:
        texture_path: Path to the texture file
        
    Returns:
        Dictionary with texture information
    """
    try:
        img = bpy.data.images.load(texture_path)
        return {
            'success': True,
            'width': img.size[0],
            'height': img.size[1],
            'channels': img.channels,
            'file_format': img.file_format,
            'is_hdr': img.is_hdr
        }
    except Exception as e:
        return {
            'success': False,
            'message': f'Failed to load texture: {str(e)}'
        }

def validate_material(material_name: str) -> Dict[str, Any]:
    """
    Validate material settings and texture assignments
    
    Args:
        material_name: Name of the material to validate
        
    Returns:
        Dictionary with validation results
    """
    try:
        mat = bpy.data.materials.get(material_name)
        if not mat:
            return {
                'success': False,
                'message': 'Material not found'
            }
        
        issues = []
        
        if not mat.use_nodes:
            issues.append('Material does not use nodes')
        else:
            nodes = mat.node_tree.nodes
            links = mat.node_tree.links
            
            # Check for Principled BSDF
            principled = None
            for node in nodes:
                if node.type == 'BSDF_PRINCIPLED':
                    principled = node
                    break
            
            if not principled:
                issues.append('No Principled BSDF found')
            
            # Check texture nodes
            texture_nodes = []
            for node in nodes:
                if node.type == 'TEX_IMAGE':
                    texture_nodes.append(node)
                    if not node.image:
                        issues.append(f'Texture node has no image: {node.name}')
                    elif not node.image.filepath:
                        issues.append(f'Texture image has no filepath: {node.name}')
            
            if not texture_nodes:
                issues.append('No texture nodes found')
        
        if issues:
            return {
                'success': True,
                'valid': False,
                'issues': issues
            }
        else:
            return {
                'success': True,
                'valid': True
            }
            
    except Exception as e:
        return {
            'success': False,
            'message': f'Failed to validate material: {str(e)}'
        }

def optimize_material(material_name: str) -> Dict[str, Any]:
    """
    Optimize material settings for performance and quality
    
    Args:
        material_name: Name of the material to optimize
        
    Returns:
        Dictionary with optimization results
    """
    try:
        mat = bpy.data.materials.get(material_name)
        if not mat:
            return {
                'success': False,
                'message': 'Material not found'
            }
        
        optimizations = []
        
        if mat.use_nodes:
            nodes = mat.node_tree.nodes
            links = mat.node_tree.links
            
            # Remove unused nodes
            used_nodes = set()
            for link in links:
                used_nodes.add(link.from_node)
                used_nodes.add(link.to_node)
            
            nodes_to_remove = []
            for node in nodes:
                if node not in used_nodes and node.type != 'OUTPUT_MATERIAL':
                    nodes_to_remove.append(node)
            
            for node in nodes_to_remove:
                nodes.remove(node)
                optimizations.append(f'Removed unused node: {node.name}')
            
            # Optimize texture nodes
            for node in nodes:
                if node.type == 'TEX_IMAGE' and node.image:
                    # Set texture interpolation to linear for better performance
                    node.interpolation = 'Linear'
                    
                    # Set texture extension to repeat for better tiling
                    node.extension = 'REPEAT'
                    
                    # Enable mipmaps for better performance
                    node.image.use_auto_refresh = False
                    
                    optimizations.append(f'Optimized texture node: {node.name}')
        
        return {
            'success': True,
            'optimizations': optimizations
        }
        
    except Exception as e:
        return {
            'success': False,
            'message': f'Failed to optimize material: {str(e)}'
        }

if __name__ == "__main__":
    # Handle command line arguments for C++ integration
    if len(sys.argv) > 1:
        command = sys.argv[1]
        
        if command == "create_material":
            # Read options from stdin or file
            options_str = sys.argv[2] if len(sys.argv) > 2 else "{}"
            options = json.loads(options_str)
            result = create_material(options)
            print(json.dumps(result))
            
        elif command == "assign_texture":
            material_name = sys.argv[2]
            texture_path = sys.argv[3]
            texture_type = sys.argv[4]
            result = assign_texture(material_name, texture_path, texture_type)
            print(json.dumps(result))
            
        elif command == "load_texture":
            texture_path = sys.argv[2]
            result = load_texture_info(texture_path)
            print(json.dumps(result))
            
        elif command == "validate_material":
            material_name = sys.argv[2]
            result = validate_material(material_name)
            print(json.dumps(result))
            
        elif command == "optimize_material":
            material_name = sys.argv[2]
            result = optimize_material(material_name)
            print(json.dumps(result)) 