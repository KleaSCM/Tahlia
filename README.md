# Blender Universal Asset Management System

**Version**: 1.0.0  
**Last Updated**: July 26, 2025  
**Author**: KleaSCM  
**Email**: KleaSCM@gmail.com 

A universal asset management system for Blender projects that can handle ANY file type, ANY size, and ANY directory structure. Designed for individual artists, teams, and enterprise-level studios.

##  How It Works

```
┌─────────────────────────────────────────────────────────────┐
│                    UNIVERSAL ASSET LIBRARY                  │
│  ┌─────────────────────────────────────────────────────┐   │
│  │ Assets/                                            │   │
│  │ ├── Models/Buildings/Characters/Props/Environment/ │   │
│  │ ├── Textures/Materials/Characters/Props/           │   │
│  │ └── Scenes/                                        │   │
│  └─────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────┐
│                    UNIVERSAL UTILS                          │
│  ┌─────────────────────────────────────────────────────┐   │
│  │ utils.py                                           │   │
│  │ ├── import_model() - Import ANY 3D model          │   │
│  │ ├── import_model_with_options() - Advanced import  │   │
│  │ ├── create_asset_collection() - Group assets       │   │
│  │ ├── auto_assign_materials() - Smart materials      │   │
│  │ ├── import_assets_in_pattern() - Batch patterns    │   │
│  │ ├── validate_asset_before_import() - Validation    │   │
│  │ ├── get_import_history() - Import history          │   │
│  │ ├── quick_material_setup() - Material presets      │   │
│  │ ├── link_or_import_asset() - Link vs Import        │   │
│  │ └── ...and more!                                  │   │
│  └─────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
                               │
                               ▼
┌─────────────────────────────────────────────────────────────┐
│                    INDIVIDUAL PROJECTS                      │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐  │
│  │ example_city/   │  │ my_project/     │  │ other_proj/ │  │
│  │ ├── project.py  │  │ ├── project.py  │  │ ├── proj.py │  │
│  │ └── launch.py   │  │ └── launch.py   │  │ └── run.py  │  │
│  └─────────────────┘  └─────────────────┘  └─────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### 📁 Project Structure

```
blender-asset-manager/
├── utils.py              # Universal utilities
├── audit_assets.py       # Asset auditing
├── test_utils.py         # Testing utilities
├── audit_and_test.py     # Combined validation
├── setup.py              # Package setup
├── pyproject.toml        # Modern packaging
├── requirements.txt       # Dependencies
├── README.md             # This file
├── CHANGELOG.md          # Version history
├── LICENSE               # MIT License
├── VERSION               # Version number
├── .gitignore           # Git ignore rules
├── Assets/              # Asset library
│   ├── Models/          # 3D models
│   ├── Textures/        # Textures
│   └── Scenes/          # Scene files
└── projects/            # Individual projects
    └── example_city/    # Example project
```

## Quick Start

### Installation

```bash
# Clone the repository
git clone https://github.com/KleaSCM/blender-asset-manager.git
cd blender-asset-manager

# Install dependencies
pip install -r requirements.txt

# Install in development mode
pip install -e .
```

### Basic Usage

```python
# Import the universal utilities
from utils import *

# Get asset statistics
stats = get_asset_stats()
print(f"Total files: {stats['total_files']}")

# Search for assets
buildings = search_assets("building")
print(f"Found {len(buildings)} building assets")

# Get assets by category
character_models = get_assets_by_category("Characters")
print(f"Found {len(character_models)} character models")
```

### Running Projects

```bash
# Navigate to a project
cd projects/example_city

# Launch the project
python launch.py
```

## 📁 Project Structure

```
blender-asset-manager/
├── utils.py              # Universal utilities
├── audit_assets.py       # Asset auditing
├── test_utils.py         # Testing utilities
├── audit_and_test.py     # Combined validation
├── setup.py              # Package setup
├── pyproject.toml        # Modern packaging
├── requirements.txt       # Dependencies
├── README.md             # This file
├── CHANGELOG.md          # Version history
├── LICENSE               # MIT License
├── VERSION               # Version number
├── .gitignore           # Git ignore rules
├── Assets/              # Asset library
│   ├── Models/          # 3D models
│   ├── Textures/        # Textures
│   └── Scenes/          # Scene files
└── projects/            # Individual projects
    └── example_city/    # Example project
```

## Universal Features

### Universal Asset Access
- **One Import**: `from utils import *` gives you everything
- **Universal Paths**: Works with any directory structure
- **Smart Detection**: Auto-discovers assets anywhere in the library
- **Cross-Platform**: Works on Windows, macOS, and Linux

### Universal File Support
- **Models**: `.blend`, `.obj`, `.fbx`, `.dae`, `.3ds`, `.stl`, `.ply`
- **Textures**: `.png`, `.jpg`, `.jpeg`, `.tga`, `.tiff`, `.bmp`, `.exr`, `.hdr`
- **Audio**: `.mp3`, `.wav`, `.flac`, `.aac`, `.ogg`, `.wma`, `.m4a`
- **Video**: `.mp4`, `.avi`, `.mov`, `.wmv`, `.flv`, `.webm`, `.mkv`
- **Documents**: `.pdf`, `.doc`, `.docx`, `.txt`, `.md`, `.json`, `.xml`
- **Scripts**: `.py`, `.js`, `.ts`, `.sh`, `.bat`, `.ps1`
- **Archives**: `.zip`, `.rar`, `.7z`, `.tar`, `.gz`, `.bz2`

### Universal Asset Discovery
- **Search by Name**: Find any asset by filename
- **Search by Category**: Get all assets in a category
- **Search by Type**: Filter by models, textures, audio, video, etc.
- **Statistics**: Complete library analysis and reporting

### Advanced Importing & Management
- **Advanced Import Options**: Merge, auto-smooth, import materials, organize in collections
- **Asset Collections**: Group assets for organized scenes
- **Smart Material Assignment**: Auto-assign or create materials based on object names
- **Batch Import Patterns**: Import assets in grid, circle, line, or random layouts
- **Asset Validation**: Check assets before importing (file exists, size, missing textures)
- **Import History**: Track what’s been imported in the session
- **Quick Material Setup**: One-click creation of metal, wood, glass, etc.
- **Link vs Import**: Choose between linking (reference) or importing (copy)

## Usage Examples

### Advanced Importing

```python
from utils import *

# Import with options
import_model_with_options(
    "Models/Buildings/building_04/building_04.blend",
    location=(0,0,0),
    merge_objects=True,
    auto_smooth=True,
    import_materials=True,
    collection_name="MyBuildings"
)

# Validate before import
validation = validate_asset_before_import("Models/Buildings/building_04/building_04.obj")
if validation['valid']:
    import_model("Models/Buildings/building_04/building_04.obj")

# Create a collection
create_asset_collection("City_Buildings", [
    "Models/Buildings/building_04/building_04.blend",
    "Models/Buildings/cottage_obj/cottage_obj.obj"
])

# Import in a grid pattern
import_assets_in_pattern(
    search_assets("building", "models"),
    pattern="grid", spacing=10
)

# Smart material assignment
imported = import_model("Models/Buildings/building_04/building_04.blend")
auto_assign_materials(imported)

# Quick material setup
metal_mat = quick_material_setup("metal", "Steel_Material")

# Link or import
link_or_import_asset("Models/Buildings/building_04/building_04.blend", link=True)

# Import history
history = get_import_history()
clear_import_history()
```

---

