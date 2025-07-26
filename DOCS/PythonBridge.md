# Tahlia Python Bridge Documentation

**Author:** KleaSCM  
**Email:** KleaSCM@gmail.com  
**Version:** 1.0.0

## Overview

The Tahlia Python Bridge provides a complete integration between the C++ core asset management system and Blender's Python API. It enables seamless asset importing, material creation, and context management with full Blender integration.

## Architecture

The Python bridge consists of two main components:

1. **C++ PythonBridge Class** (`src/core/python_bridge.cpp`) - Handles C++/Python interop
2. **Python Bridge Module** (`src/python/tahlia_bridge.py`) - Provides actual Blender API integration

### Key Features

- **Real Blender API Integration** - Uses actual Blender Python API calls
- **Context Management** - Preserves and restores Blender context state
- **Asset Importing** - Supports multiple file formats (OBJ, FBX, DAE, Blend, textures)
- **Material Creation** - Creates materials with PBR support
- **Pattern Importing** - Grid, circle, line, and random import patterns
- **Error Handling** - Comprehensive error handling and reporting
- **Thread Safety** - Safe concurrent Python/C++ operations

## Installation

### Prerequisites

- Python 3.11+
- Blender 3.0+
- C++17 compatible compiler

### Build Configuration

The bridge requires Python development headers and libraries:

```bash
# Install Python development packages
sudo pacman -S python-pip python-dev

# Build with Python support
zig build build-test-python-bridge
```

## Usage

### Basic Setup

```python
import tahlia_core
import tahlia_bridge

# Initialize the bridge
tahlia = tahlia_core.TahliaCore()
```

### Context Management

#### Capturing Context

```python
# Capture current Blender context
context = tahlia_bridge.capture_context()
print(f"Active object: {context['active_object']}")
print(f"Selected objects: {context['selected_objects']}")
print(f"View layer: {context['view_layer']}")
print(f"Mode: {context['mode']}")
```

#### Restoring Context

```python
# Restore previously captured context
success = tahlia_bridge.restore_context(context)
if success:
    print("Context restored successfully")
```

#### Context Stack Operations

```python
# Push current context onto stack
tahlia_bridge.push_context()

# Perform operations that might change context
bpy.ops.mesh.primitive_cube_add()

# Pop and restore previous context
tahlia_bridge.pop_context()

# Get stack size
size = tahlia_bridge.get_context_stack_size()

# Clear entire stack
tahlia_bridge.clear_context_stack()
```

#### Context Preservation

```python
# Execute operation while preserving context
def my_operation():
    bpy.ops.mesh.primitive_sphere_add()
    bpy.ops.object.shade_smooth()

success = tahlia_bridge.preserve_context(my_operation)
```

### Asset Importing

#### Single Asset Import

```python
# Import a single asset
result = tahlia_bridge.import_asset_blender("path/to/model.obj", {
    'link': 'False',  # Import vs link
    'collection_name': 'MyCollection'
})

if result['success']:
    print(f"Imported: {result['message']}")
    print(f"Objects: {result['list_data']}")
else:
    print(f"Import failed: {result['message']}")
```

#### Grid Import Pattern

```python
# Import multiple assets in a grid
asset_paths = ["model1.obj", "model2.obj", "model3.obj", "model4.obj"]
results = tahlia_core.import_assets_grid(
    asset_paths, 
    {},           # options
    2,            # rows
    2,            # columns
    3.0           # spacing
)

for result in results:
    if result['success']:
        print(f"Grid import: {result['message']}")
```

#### Circle Import Pattern

```python
# Import assets in a circle
results = tahlia_core.import_assets_circle(
    asset_paths,
    {},           # options
    10.0          # radius
)
```

#### Line Import Pattern

```python
# Import assets in a line
results = tahlia_core.import_assets_line(
    asset_paths,
    {},           # options
    5.0           # spacing
)
```

### Material Creation

#### Basic Material

```python
# Create a basic material
result = tahlia_bridge.create_material_blender("MyMaterial", {
    'metallic': '0.5',
    'roughness': '0.3',
    'base_color': '1.0,0.5,0.2'  # RGB values
})

if result['success']:
    print(f"Created material: {result['data']['material_name']}")
```

#### PBR Material

```python
# Create a PBR material with advanced settings
result = tahlia_bridge.create_pbr_material_blender("MyPBRMaterial", {
    'metallic': '0.8',
    'roughness': '0.2',
    'specular': '0.5',
    'ior': '1.45',
    'transmission': '0.1',
    'emission_strength': '0.0',
    'subsurface': '0.0'
})
```

### Supported File Formats

```python
# Get supported formats
formats = tahlia_bridge.get_supported_formats_blender()

print("Model formats:", formats['models'])
print("Texture formats:", formats['textures'])
print("Audio formats:", formats['audio'])
print("Video formats:", formats['video'])
```

### Configuration

#### Debug Mode

```python
# Enable debug mode for detailed logging
tahlia_bridge.set_debug_mode(True)

# Disable debug mode
tahlia_bridge.set_debug_mode(False)
```

#### Context Preservation

```python
# Enable/disable context preservation
tahlia_bridge.set_context_preservation(True)
tahlia_bridge.set_context_preservation(False)
```

#### Stack Size

```python
# Set maximum context stack size
tahlia_bridge.set_max_context_stack_size(20)
```

### Error Handling

```python
# Get last error
error = tahlia_bridge.get_last_error()
if error:
    print(f"Last error: {error}")

# Clear last error
tahlia_bridge.clear_last_error()
```

## C++ Integration

### PythonBridge Class

The C++ `PythonBridge` class provides the interface between C++ and Python:

```cpp
#include "python_bridge.hpp"

// Initialize bridge
PythonBridge::PythonBridge bridge;
bridge.initialize();

// Set managers
bridge.setAssetManager(asset_manager);
bridge.setImportManager(import_manager);
bridge.setMaterialManager(material_manager);
bridge.setImportHistory(import_history);

// Import asset
PythonResult result = bridge.importAssetPython("model.obj", options);

// Create material
PythonResult material_result = bridge.createMaterialPython("MyMaterial", material_options);
```

### Context Management

```cpp
// Capture context
BlenderContext context = bridge.captureContext();

// Restore context
bool success = bridge.restoreContext(context);

// Stack operations
bridge.pushContext();
size_t size = bridge.getContextStackSize();
bridge.popContext();
bridge.clearContextStack();
```

## File Format Support

### Models
- **Blend** (.blend) - Native Blender files
- **OBJ** (.obj) - Wavefront OBJ format
- **FBX** (.fbx) - Autodesk FBX format
- **DAE** (.dae) - Collada format
- **3DS** (.3ds) - 3D Studio format
- **STL** (.stl) - Stereolithography format
- **PLY** (.ply) - Stanford format

### Textures
- **PNG** (.png) - Portable Network Graphics
- **JPEG** (.jpg, .jpeg) - Joint Photographic Experts Group
- **TGA** (.tga) - Truevision Graphics Adapter
- **TIFF** (.tiff) - Tagged Image File Format
- **BMP** (.bmp) - Bitmap format
- **EXR** (.exr) - OpenEXR format
- **HDR** (.hdr) - High Dynamic Range format

### Audio
- **MP3** (.mp3) - MPEG Audio Layer III
- **WAV** (.wav) - Waveform Audio File Format
- **FLAC** (.flac) - Free Lossless Audio Codec
- **AAC** (.aac) - Advanced Audio Coding
- **OGG** (.ogg) - Ogg Vorbis format
- **WMA** (.wma) - Windows Media Audio
- **M4A** (.m4a) - MPEG-4 Audio

### Video
- **MP4** (.mp4) - MPEG-4 Part 14
- **AVI** (.avi) - Audio Video Interleave
- **MOV** (.mov) - QuickTime Movie
- **WMV** (.wmv) - Windows Media Video
- **FLV** (.flv) - Flash Video
- **WebM** (.webm) - WebM format
- **MKV** (.mkv) - Matroska Video

## Performance Considerations

### Context Stack Management
- Default maximum stack size: 10
- Each context capture includes object selection, viewport settings, and mode
- Large scenes may require increased stack size

### Import Performance
- OBJ files: Fast import with full material support
- FBX files: Moderate performance, good compatibility
- Blend files: Fastest import (native format)
- Textures: Automatic material creation with texture nodes

### Memory Usage
- Context objects are lightweight (string references)
- Import operations use Blender's native memory management
- Python bridge maintains minimal overhead

## Error Handling

### Common Errors

1. **File Not Found**
   ```
   Error: Asset file not found: /path/to/missing/file.obj
   ```

2. **Unsupported Format**
   ```
   Error: Unsupported file format: .xyz
   ```

3. **Import Failure**
   ```
   Error: Failed to import OBJ file: Invalid file format
   ```

4. **Python Module Error**
   ```
   Error: Failed to import tahlia_bridge module
   ```

### Error Recovery

```python
# Always check result success
result = tahlia_bridge.import_asset_blender("file.obj", {})
if not result['success']:
    print(f"Import failed: {result['message']}")
    # Handle error appropriately
else:
    print("Import successful")
```

## Testing

### Running Tests

```bash
# Run C++ tests
zig build run-test-python-bridge

# Run Python tests (in Blender)
blender --python Tests/test_python_bridge_full.py
```

### Test Coverage

- Context capture and restoration
- Context stack operations
- Asset importing (all formats)
- Material creation (basic and PBR)
- Import patterns (grid, circle, line)
- Error handling
- Configuration options

## Troubleshooting

### Common Issues

1. **Python Module Not Found**
   - Ensure `src/python` is in Python path
   - Check Python version compatibility

2. **Blender API Errors**
   - Verify Blender version compatibility
   - Check if running in Blender environment

3. **Import Failures**
   - Verify file exists and is accessible
   - Check file format support
   - Ensure sufficient disk space

4. **Context Restoration Issues**
   - Objects may have been deleted
   - View layer changes may affect restoration
   - Check stack size limits

### Debug Mode

Enable debug mode for detailed logging:

```python
tahlia_bridge.set_debug_mode(True)
```

## Future Enhancements

### Planned Features

1. **Animation Support** - Import and manage animated assets
2. **Batch Processing** - Process multiple assets simultaneously
3. **Custom Importers** - Plugin system for custom formats
4. **Advanced Materials** - Support for complex material setups
5. **Scene Management** - Advanced scene organization tools

### Performance Improvements

1. **Async Operations** - Non-blocking import operations
2. **Caching** - Asset metadata caching
3. **Compression** - Support for compressed formats
4. **Streaming** - Large file streaming support

## Contributing

### Development Setup

1. Clone the repository
2. Install dependencies
3. Set up development environment
4. Run tests to verify setup

### Code Style

- Follow existing C++ and Python style guidelines
- Add comprehensive tests for new features
- Update documentation for API changes
- Maintain backward compatibility

### Testing

- Add unit tests for new functionality
- Update integration tests
- Verify Blender compatibility
- Test error conditions

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Support

For support and questions:

- **Email:** KleaSCM@gmail.com
- **Issues:** GitHub Issues
- **Documentation:** This file and inline code comments

---

**Note:** This Python bridge provides a complete integration between the Tahlia C++ core and Blender's Python API. It enables powerful asset management capabilities while maintaining the performance and reliability of the C++ backend. 