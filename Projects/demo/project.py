#!/usr/bin/env python3
"""
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: project.py
Description: Demo project showcasing universal asset management with Blender.
             Imports a tree from the Assets library and demonstrates the asset manager integration.
"""

import bpy
import os
import sys
import subprocess
import json
from pathlib import Path

# Add the project root to Python path for asset manager access
project_root = Path(__file__).parent.parent.parent
sys.path.append(str(project_root))

def get_asset_manager_path():
    """Get the path to the C++ asset manager binary"""
    return project_root / "zig-out" / "bin" / "blender_asset_manager"

def run_asset_audit():
    """Run the C++ asset audit to get library information"""
    try:
        asset_manager_path = get_asset_manager_path()
        if not asset_manager_path.exists():
            print(f"❌ Asset manager not found at: {asset_manager_path}")
            return None
            
        result = subprocess.run([str(asset_manager_path), "--audit"], 
                              capture_output=True, text=True, cwd=project_root)
        
        if result.returncode == 0:
            print("✅ Asset audit completed successfully")
            return result.stdout
        else:
            print(f"❌ Asset audit failed: {result.stderr}")
            return None
            
    except Exception as e:
        print(f"❌ Error running asset audit: {e}")
        return None

def find_tree_assets():
    """Find tree assets in the library using the C++ asset manager"""
    try:
        asset_manager_path = get_asset_manager_path()
        if not asset_manager_path.exists():
            print("❌ Asset manager not found")
            return []
            
        # Run asset manager to get tree assets
        result = subprocess.run([str(asset_manager_path)], 
                              capture_output=True, text=True, cwd=project_root)
        
        # For demo purposes, we'll look for tree assets in the Assets directory
        assets_dir = project_root / "Assets"
        tree_assets = []
        
        for root, dirs, files in os.walk(assets_dir):
            for file in files:
                if "tree" in file.lower() and file.endswith(('.blend', '.obj', '.fbx')):
                    tree_assets.append(os.path.join(root, file))
        
        return tree_assets
        
    except Exception as e:
        print(f"❌ Error finding tree assets: {e}")
        return []

def import_tree_asset(asset_path):
    """Import a tree asset into Blender"""
    try:
        asset_path = Path(asset_path)
        
        if not asset_path.exists():
            print(f"❌ Asset not found: {asset_path}")
            return False
            
        print(f"🌳 Importing tree asset: {asset_path.name}")
        
        # Clear existing objects
        bpy.ops.object.select_all(action='SELECT')
        bpy.ops.object.delete(use_global=False)
        
        # Import based on file type
        if asset_path.suffix.lower() == '.blend':
            # Link from blend file
            bpy.ops.wm.link(directory=str(asset_path), filename=asset_path.stem)
            print(f"✅ Linked tree from: {asset_path.name}")
            
        elif asset_path.suffix.lower() == '.obj':
            # Import OBJ file
            bpy.ops.import_scene.obj(filepath=str(asset_path))
            print(f"✅ Imported OBJ tree: {asset_path.name}")
            
        elif asset_path.suffix.lower() == '.fbx':
            # Import FBX file
            bpy.ops.import_scene.fbx(filepath=str(asset_path))
            print(f"✅ Imported FBX tree: {asset_path.name}")
            
        else:
            print(f"❌ Unsupported file type: {asset_path.suffix}")
            return False
        
        # Set up basic scene
        setup_demo_scene()
        
        return True
        
    except Exception as e:
        print(f"❌ Error importing tree asset: {e}")
        return False

def setup_demo_scene():
    """Set up a basic demo scene with lighting and camera"""
    try:
        # Add a ground plane
        bpy.ops.mesh.primitive_plane_add(size=20, location=(0, 0, 0))
        ground = bpy.context.active_object
        ground.name = "Ground"
        
        # Add material to ground
        ground_mat = bpy.data.materials.new(name="Ground_Material")
        ground_mat.use_nodes = True
        nodes = ground_mat.node_tree.nodes
        nodes["Principled BSDF"].inputs["Base Color"].default_value = (0.2, 0.3, 0.1, 1.0)
        ground.data.materials.append(ground_mat)
        
        # Add lighting
        bpy.ops.object.light_add(type='SUN', location=(5, 5, 10))
        sun = bpy.context.active_object
        sun.data.energy = 5.0
        
        # Add camera
        bpy.ops.object.camera_add(location=(10, -10, 8))
        camera = bpy.context.active_object
        camera.rotation_euler = (0.9, 0, 0.8)
        
        # Set as active camera
        bpy.context.scene.camera = camera
        
        print("✅ Demo scene setup complete")
        
    except Exception as e:
        print(f"❌ Error setting up demo scene: {e}")

def create_demo_cube():
    """Create a simple cube to demonstrate the scene"""
    try:
        bpy.ops.mesh.primitive_cube_add(size=2, location=(0, 0, 1))
        cube = bpy.context.active_object
        cube.name = "Demo_Cube"
        
        # Add material
        cube_mat = bpy.data.materials.new(name="Cube_Material")
        cube_mat.use_nodes = True
        nodes = cube_mat.node_tree.nodes
        nodes["Principled BSDF"].inputs["Base Color"].default_value = (0.8, 0.2, 0.2, 1.0)
        cube.data.materials.append(cube_mat)
        
        print("✅ Demo cube created")
        
    except Exception as e:
        print(f"❌ Error creating demo cube: {e}")

def import_specific_assets():
    """Import Tree1.blend directly - this actually works!"""
    import bpy
    from pathlib import Path
    
    # Clear everything
    bpy.ops.object.select_all(action='SELECT')
    bpy.ops.object.delete(use_global=False)
    
    # Import the blend file (this works!)
    blend_path = Path(__file__).parent.parent.parent / "Assets/Models/Environment/tree/Tree1.blend"
    print(f"Importing: {blend_path}")
    
    # Import all objects from the blend file
    with bpy.data.libraries.load(str(blend_path), link=False) as (data_from, data_to):
        data_to.objects = data_from.objects
    
    # Add all imported objects to the scene
    for obj in data_to.objects:
        bpy.context.scene.collection.objects.link(obj)
    
    # Print what we got
    objects = [obj.name for obj in bpy.data.objects]
    print(f"Objects in scene: {objects}")
    
    # Set up scene
    setup_demo_scene()

def main():
    """Main demo function"""
    print("🎨 Universal Asset Manager - Blender Demo")
    print("Author: KleaSCM")
    print("Email: KleaSCM@gmail.com")
    print("=" * 50)
    
    # Run asset audit
    print("\n📊 Running asset audit...")
    audit_output = run_asset_audit()
    if audit_output:
        print("Asset library audit completed")
    
    # Import specific assets
    print("\n🌳 Importing demo tree assets...")
    import_specific_assets()
    
    print("\n🎉 Demo project complete!")
    print("This demonstrates the universal asset manager integration with Blender.")

if __name__ == "__main__":
    main() 