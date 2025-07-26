"""
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: test_python_bridge_full.py
Description: Comprehensive test script for the fully implemented Python bridge.
              Tests real Blender API integration and all bridge functionality.
"""

import bpy
import sys
import os
import json
from pathlib import Path

# Add the src/python directory to the path
sys.path.append('src/python')

# Import our modules
import tahlia_core
import tahlia_bridge

def test_context_capture():
    """Test context capture and restoration."""
    print("Testing context capture...")
    
    # Create some test objects
    bpy.ops.mesh.primitive_cube_add(location=(0, 0, 0))
    cube = bpy.context.active_object
    cube.name = "TestCube"
    
    bpy.ops.mesh.primitive_uv_sphere_add(location=(2, 0, 0))
    sphere = bpy.context.active_object
    sphere.name = "TestSphere"
    
    # Select both objects
    bpy.ops.object.select_all(action='DESELECT')
    cube.select_set(True)
    sphere.select_set(True)
    bpy.context.view_layer.objects.active = cube
    
    # Capture context
    context = tahlia_bridge.capture_context()
    print(f"Captured context: {context}")
    
    # Verify context was captured correctly
    assert "TestCube" in context['selected_objects']
    assert "TestSphere" in context['selected_objects']
    assert context['active_object'] == "TestCube"
    
    # Move objects and change selection
    bpy.ops.object.select_all(action='DESELECT')
    bpy.ops.mesh.primitive_cone_add(location=(0, 2, 0))
    cone = bpy.context.active_object
    cone.name = "TestCone"
    
    # Restore context
    success = tahlia_bridge.restore_context(context)
    assert success
    
    # Verify context was restored
    selected_objects = [obj.name for obj in bpy.context.selected_objects]
    assert "TestCube" in selected_objects
    assert "TestSphere" in selected_objects
    assert bpy.context.active_object.name == "TestCube"
    
    print("✅ Context capture test passed!")

def test_context_stack():
    """Test context stack operations."""
    print("Testing context stack...")
    
    # Clear any existing stack
    tahlia_bridge.clear_context_stack()
    assert tahlia_bridge.get_context_stack_size() == 0
    
    # Push contexts
    assert tahlia_bridge.push_context()
    assert tahlia_bridge.get_context_stack_size() == 1
    
    # Move objects
    bpy.ops.mesh.primitive_cube_add(location=(1, 1, 1))
    
    assert tahlia_bridge.push_context()
    assert tahlia_bridge.get_context_stack_size() == 2
    
    # Pop contexts
    assert tahlia_bridge.pop_context()
    assert tahlia_bridge.get_context_stack_size() == 1
    
    assert tahlia_bridge.pop_context()
    assert tahlia_bridge.get_context_stack_size() == 0
    
    print("✅ Context stack test passed!")

def test_asset_import():
    """Test asset importing functionality."""
    print("Testing asset import...")
    
    # Create a test OBJ file (simplified)
    test_obj_content = """# Test OBJ file
v 0.0 0.0 0.0
v 1.0 0.0 0.0
v 0.0 1.0 0.0
f 1 2 3
"""
    
    test_obj_path = "test_cube.obj"
    with open(test_obj_path, 'w') as f:
        f.write(test_obj_content)
    
    try:
        # Test importing the asset
        result = tahlia_bridge.import_asset_blender(test_obj_path, {})
        print(f"Import result: {result}")
        
        # Verify import was successful
        assert result['success']
        assert "test_cube.obj" in result['data']['file_path']
        
    finally:
        # Clean up
        if os.path.exists(test_obj_path):
            os.remove(test_obj_path)
    
    print("✅ Asset import test passed!")

def test_material_creation():
    """Test material creation functionality."""
    print("Testing material creation...")
    
    # Test basic material creation
    result = tahlia_bridge.create_material_blender("TestMaterial", {
        'metallic': '0.5',
        'roughness': '0.3',
        'base_color': '1.0,0.5,0.2'
    })
    
    print(f"Material creation result: {result}")
    assert result['success']
    assert result['data']['material_name'] == "TestMaterial"
    
    # Verify material was created in Blender
    material = bpy.data.materials.get("TestMaterial")
    assert material is not None
    assert material.use_nodes
    
    # Test PBR material creation
    result = tahlia_bridge.create_pbr_material_blender("TestPBRMaterial", {
        'metallic': '0.8',
        'roughness': '0.2',
        'specular': '0.5',
        'ior': '1.45',
        'transmission': '0.1'
    })
    
    print(f"PBR material creation result: {result}")
    assert result['success']
    assert result['data']['material_name'] == "TestPBRMaterial"
    
    # Verify PBR material was created
    pbr_material = bpy.data.materials.get("TestPBRMaterial")
    assert pbr_material is not None
    assert pbr_material.use_nodes
    
    print("✅ Material creation test passed!")

def test_grid_import():
    """Test grid import pattern."""
    print("Testing grid import...")
    
    # Create test assets
    test_assets = []
    for i in range(4):
        obj_content = f"""# Test OBJ file {i}
v 0.0 0.0 0.0
v 1.0 0.0 0.0
v 0.0 1.0 0.0
f 1 2 3
"""
        asset_path = f"test_asset_{i}.obj"
        with open(asset_path, 'w') as f:
            f.write(obj_content)
        test_assets.append(asset_path)
    
    try:
        # Test grid import
        results = tahlia_core.import_assets_grid(test_assets, {}, 2, 2, 3.0)
        print(f"Grid import results: {results}")
        
        # Verify all imports were successful
        for result in results:
            assert result['success']
        
    finally:
        # Clean up
        for asset_path in test_assets:
            if os.path.exists(asset_path):
                os.remove(asset_path)
    
    print("✅ Grid import test passed!")

def test_circle_import():
    """Test circle import pattern."""
    print("Testing circle import...")
    
    # Create test assets
    test_assets = []
    for i in range(6):
        obj_content = f"""# Test OBJ file {i}
v 0.0 0.0 0.0
v 1.0 0.0 0.0
v 0.0 1.0 0.0
f 1 2 3
"""
        asset_path = f"test_circle_asset_{i}.obj"
        with open(asset_path, 'w') as f:
            f.write(obj_content)
        test_assets.append(asset_path)
    
    try:
        # Test circle import
        results = tahlia_core.import_assets_circle(test_assets, {}, 5.0)
        print(f"Circle import results: {results}")
        
        # Verify all imports were successful
        for result in results:
            assert result['success']
        
    finally:
        # Clean up
        for asset_path in test_assets:
            if os.path.exists(asset_path):
                os.remove(asset_path)
    
    print("✅ Circle import test passed!")

def test_line_import():
    """Test line import pattern."""
    print("Testing line import...")
    
    # Create test assets
    test_assets = []
    for i in range(3):
        obj_content = f"""# Test OBJ file {i}
v 0.0 0.0 0.0
v 1.0 0.0 0.0
v 0.0 1.0 0.0
f 1 2 3
"""
        asset_path = f"test_line_asset_{i}.obj"
        with open(asset_path, 'w') as f:
            f.write(obj_content)
        test_assets.append(asset_path)
    
    try:
        # Test line import
        results = tahlia_core.import_assets_line(test_assets, {}, 2.0)
        print(f"Line import results: {results}")
        
        # Verify all imports were successful
        for result in results:
            assert result['success']
        
    finally:
        # Clean up
        for asset_path in test_assets:
            if os.path.exists(asset_path):
                os.remove(asset_path)
    
    print("✅ Line import test passed!")

def test_supported_formats():
    """Test supported formats functionality."""
    print("Testing supported formats...")
    
    formats = tahlia_bridge.get_supported_formats_blender()
    print(f"Supported formats: {formats}")
    
    # Verify we have the expected format categories
    assert 'models' in formats
    assert 'textures' in formats
    assert 'audio' in formats
    assert 'video' in formats
    
    # Verify we have common formats
    assert '.obj' in formats['models']
    assert '.fbx' in formats['models']
    assert '.blend' in formats['models']
    assert '.png' in formats['textures']
    assert '.jpg' in formats['textures']
    
    print("✅ Supported formats test passed!")

def test_debug_mode():
    """Test debug mode functionality."""
    print("Testing debug mode...")
    
    # Test setting debug mode
    tahlia_bridge.set_debug_mode(True)
    tahlia_bridge.set_debug_mode(False)
    
    print("✅ Debug mode test passed!")

def test_context_preservation():
    """Test context preservation functionality."""
    print("Testing context preservation...")
    
    # Test setting context preservation
    tahlia_bridge.set_context_preservation(True)
    tahlia_bridge.set_context_preservation(False)
    tahlia_bridge.set_context_preservation(True)  # Restore default
    
    print("✅ Context preservation test passed!")

def test_error_handling():
    """Test error handling functionality."""
    print("Testing error handling...")
    
    # Test importing non-existent file
    result = tahlia_bridge.import_asset_blender("non_existent_file.obj", {})
    assert not result['success']
    assert "not found" in result['message']
    
    # Test creating material with invalid options
    result = tahlia_bridge.create_material_blender("TestErrorMaterial", {
        'metallic': 'invalid_value'
    })
    # This should still succeed but with default values
    
    print("✅ Error handling test passed!")

def main():
    """Run all tests."""
    print("🧪 Running comprehensive Python bridge tests...")
    print("=" * 50)
    
    try:
        test_context_capture()
        test_context_stack()
        test_asset_import()
        test_material_creation()
        test_grid_import()
        test_circle_import()
        test_line_import()
        test_supported_formats()
        test_debug_mode()
        test_context_preservation()
        test_error_handling()
        
        print("=" * 50)
        print("🎉 All Python bridge tests passed!")
        print("✅ The Python bridge is fully functional!")
        
    except Exception as e:
        print(f"❌ Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False
    
    return True

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1) 