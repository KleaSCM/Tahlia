# Universal Asset Manager - Blender Demo

**Author**: KleaSCM  
**Email**: KleaSCM@gmail.com  
**Description**: Demo project showcasing the Universal Asset Manager integration with Blender.

## 🎨 What This Demo Does

This demo demonstrates how the Universal Asset Manager C++ core integrates with Blender to:

1. **Scan the Assets library** using the C++ asset manager
2. **Find tree assets** in the universal asset library
3. **Import a tree** into Blender with proper scene setup
4. **Show the integration** between C++ backend and Blender frontend

## 🚀 How to Run

### Method 1: Blender Text Editor
1. Open Blender
2. Go to the **Scripting** workspace
3. Open the **Text Editor**
4. Open `Projects/demo/launch.py`
5. Click **Run Script**

### Method 2: Command Line
```bash
# From the project root
blender --python Projects/demo/launch.py
```

### Method 3: Blender Console
```python
# In Blender's Python console
exec(open("Projects/demo/launch.py").read())
```

## 📁 Project Structure

```
Projects/demo/
├── project.py      # Main demo logic
├── launch.py       # Blender launch script
└── README.md       # This file
```

## 🌳 What You'll See

1. **Asset Audit**: The demo runs the C++ asset manager to scan your Assets library
2. **Tree Search**: It searches for tree assets in your library
3. **Import**: It imports the first tree asset it finds (blend, obj, or fbx)
4. **Scene Setup**: It creates a ground plane, lighting, and camera
5. **Result**: A complete scene with your imported tree!

## 🔧 Requirements

- **Blender** (3.0+ recommended)
- **C++ Asset Manager** built and available at `zig-out/bin/blender_asset_manager`
- **Assets library** with tree assets (the demo will create a cube if no trees are found)

## 🎯 Demo Features

- **Universal Asset Discovery**: Finds assets regardless of file type
- **Smart Import**: Handles blend, obj, and fbx files automatically
- **Scene Management**: Sets up proper lighting and camera
- **Error Handling**: Graceful fallback if assets aren't found
- **Integration**: Shows how C++ and Python work together

## 💡 Customization

You can modify `project.py` to:
- Import different asset types
- Change the scene setup
- Add more complex asset management features
- Integrate with other parts of the asset manager

## 🎉 Success Indicators

When the demo runs successfully, you should see:
- ✅ Asset audit completed
- ✅ Tree assets found
- ✅ Tree imported successfully
- ✅ Demo scene setup complete
- ✅ A tree (or cube) in your Blender scene

---

**This demo proves that your Universal Asset Manager works perfectly with Blender!** 🌸✨ 