#!/usr/bin/env python3
"""
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: launch.py
Description: Universal launch script for the Universal Asset Manager demo project.
             - If run from terminal, launches Blender and runs this script.
             - If run inside Blender, runs the demo logic directly.
"""

import sys
import os
from pathlib import Path

# Add the demo project directory to Python path
demo_path = Path(__file__).parent
if str(demo_path) not in sys.path:
    sys.path.insert(0, str(demo_path))

def is_blender():
    try:
        import bpy
        return True
    except ImportError:
        return False

def run_blender():
    """Launch Blender and run this script inside it."""
    blender_exe = "blender"
    script_path = Path(__file__).resolve()
    print(f"🚀 Launching Blender with demo: {script_path}")
    os.execvp(blender_exe, [blender_exe, "--python", str(script_path)])

def setup_blender_environment():
    try:
        import bpy
        for area in bpy.context.screen.areas:
            if area.type == 'VIEW_3D':
                for space in area.spaces:
                    if space.type == 'VIEW_3D':
                        space.shading.type = 'MATERIAL'
                        space.shading.use_scene_lights = True
                        space.shading.use_scene_world = True
        print("✅ Blender environment configured")
    except Exception as e:
        print(f"❌ Error setting up Blender environment: {e}")

def run_demo():
    try:
        from project import main
        main()
        print("\n🎉 Demo launched successfully!")
        print("Check the Blender console for detailed output.")
    except ImportError as e:
        print(f"❌ Error importing demo: {e}")
        print("Make sure you're running this from the correct directory.")
    except Exception as e:
        print(f"❌ Error running demo: {e}")

if __name__ == "__main__":
    if is_blender():
        print("🚀 Launching Universal Asset Manager Demo (Blender Mode)")
        print("Author: KleaSCM")
        print("Email: KleaSCM@gmail.com")
        print("=" * 50)
        setup_blender_environment()
        run_demo()
    else:
        print("🚀 Launching Universal Asset Manager Demo (Terminal Mode)")
        run_blender() 