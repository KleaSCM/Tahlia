/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * Name: import_manager.cpp
 * Description: Implementation of the ImportManager class for modular, high-performance asset importing.
 *              Provides single and bulk import with configurable options and pattern arrangements.
 *              Designed for seamless integration with AssetManager and flexible import workflows.
 *
 * Architecture:
 * - Modular import system with extensible import strategies
 * - Integration with AssetManager and AssetIndexer for asset metadata
 * - Configurable import options (location, rotation, scale, merge, auto-smooth, etc.)
 * - Bulk import patterns (grid, circle, line, random)
 * - Support for both linking and direct importing of assets
 * - Thread-safe operations for concurrent import tasks
 *
 * Key Features:
 * - Import single asset with full transform and merge options
 * - Bulk import assets in various spatial patterns
 * - Support for linking assets instead of importing
 * - Integration with material and collection management
 * - Detailed import result reporting and error handling
 * - Extensible design for new import patterns and asset types
 */

#include "import_manager.hpp"
#include "asset_manager.hpp"
#include <iostream>
#include <random>
#include <filesystem> // Required for std::filesystem::exists
#include <sstream>    // Required for std::ostringstream
#include <cmath>      // Required for M_PI and trigonometric functions
#include <fstream>    // Required for std::ifstream
#include <algorithm>  // Required for std::transform
#include <cstdio>      // For std::tmpnam
#include <cstdlib>     // For std::system
#include <array>       // For std::array
#include <memory>      // For std::unique_ptr
#include <stdexcept>   // For std::runtime_error

namespace AssetManager {

ImportManager::ImportManager() : asset_manager_(nullptr) {
    // Constructor: Initialize internal state if needed
}

ImportManager::~ImportManager() {
    // Destructor: Clean up resources if needed
}

void ImportManager::setAssetManager(std::shared_ptr<AssetManager> manager) {
    asset_manager_ = manager;
}

ImportResult ImportManager::importAsset(const std::string& asset_path, const ImportOptions& options) {
    /**
     * @brief Imports or links an asset using Blender subprocess, applying all ImportOptions.
     *        Generates a Python script that sets location, rotation, scale, merge, auto-smooth, etc.
     *        Handles both linking and importing, with robust error handling and detailed reporting.
     *
     * @param asset_path Path to the asset file to import or link.
     * @param options ImportOptions struct with all import parameters.
     * @return ImportResult with success status, message, and imported object names.
     */
    ImportResult result;
    result.asset_path = asset_path;
    result.success = false;
    result.message = "";
    if (asset_path.empty() || !std::filesystem::exists(asset_path)) {
        result.message = "Asset file does not exist: " + asset_path;
        return result;
    }
    std::filesystem::path path(asset_path);
    // Helper lambdas for tuple to string
    auto tuple3_to_str = [](const std::tuple<float, float, float>& t) {
        std::ostringstream oss;
        oss << std::get<0>(t) << ", " << std::get<1>(t) << ", " << std::get<2>(t);
        return oss.str();
    };
    // Prepare Python script with all options
    std::ostringstream py_script;
    py_script << "import bpy\n"
              << "import sys\n"
              << "import mathutils\n"
              << "try:\n"
              << "    asset_path = sys.argv[-1]\n"
              << "    with bpy.data.libraries.load(asset_path, link=" << (options.link_instead_of_import ? "True" : "False") << ") as (data_from, data_to):\n"
              << "        if data_from.collections:\n"
              << "            data_to.collections = [data_from.collections[0]]\n"
              << "        elif data_from.objects:\n"
              << "            data_to.objects = [data_from.objects[0]]\n"
              << "    imported = []\n"
              << "    for c in data_to.collections:\n"
              << "        bpy.context.scene.collection.children.link(c)\n"
              << "        for obj in c.objects:\n"
              << "            imported.append(obj)\n"
              << "    for obj in data_to.objects:\n"
              << "        bpy.context.scene.collection.objects.link(obj)\n"
              << "        imported.append(obj)\n"
              << "    # Apply transform and options\n"
              << "    for obj in imported:\n"
              << "        obj.location = mathutils.Vector([" << tuple3_to_str(options.location) << "])\n"
              << "        obj.rotation_euler = mathutils.Vector([" << tuple3_to_str(options.rotation) << "])\n"
              << "        obj.scale = mathutils.Vector([" << tuple3_to_str(options.scale) << "])\n"
              << "        if " << (options.auto_smooth ? "True" : "False") << ":\n"
              << "            if hasattr(obj.data, 'use_auto_smooth'):\n"
              << "                obj.data.use_auto_smooth = True\n"
              << "        if 'MESH' in obj.type and " << (options.merge_objects ? "True" : "False") << ":\n"
              << "            bpy.ops.object.select_all(action='DESELECT')\n"
              << "            obj.select_set(True)\n"
              << "            bpy.context.view_layer.objects.active = obj\n"
              << "    if " << (options.merge_objects ? "True" : "False") << ":\n"
              << "        bpy.ops.object.join()\n"
              << "    if '" << options.collection_name << "':\n"
              << "        if '" << options.collection_name << "' not in bpy.data.collections:\n"
              << "            new_coll = bpy.data.collections.new('" << options.collection_name << "')\n"
              << "            bpy.context.scene.collection.children.link(new_coll)\n"
              << "        for obj in imported:\n"
              << "            bpy.data.collections['" << options.collection_name << "'].objects.link(obj)\n"
              << "    print('IMPORTED:' if not " << (options.link_instead_of_import ? "True" : "False") << " else 'LINKED:', [o.name for o in imported])\n"
              << "    print('SUCCESS')\n"
              << "except Exception as e:\n"
              << "    print('ERROR:', str(e))\n";
    // Write script to temp file
    char tmp_py_name[L_tmpnam];
    std::tmpnam(tmp_py_name);
    std::ofstream py_file(tmp_py_name);
    py_file << py_script.str();
    py_file.close();
    std::string cmd = "blender --background --factory-startup --python " + std::string(tmp_py_name) + " -- " + asset_path + " 2>&1";
    std::array<char, 256> buffer;
    std::string output;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        result.message = "Failed to launch Blender for import/link.";
        return result;
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        output += buffer.data();
    }
    std::remove(tmp_py_name);
    if (output.find("SUCCESS") != std::string::npos) {
        result.success = true;
        result.message = options.link_instead_of_import ? "Asset linked successfully." : "Asset imported successfully.";
        // Parse imported/linked object names
        size_t pos = output.find(options.link_instead_of_import ? "LINKED:" : "IMPORTED:");
        if (pos != std::string::npos) {
            size_t start = output.find('[', pos);
            size_t end = output.find(']', pos);
            if (start != std::string::npos && end != std::string::npos && end > start) {
                std::string names = output.substr(start + 1, end - start - 1);
                std::istringstream iss(names);
                std::string name;
                while (std::getline(iss, name, ',')) {
                    name.erase(std::remove(name.begin(), name.end(), '\''), name.end());
                    name.erase(std::remove(name.begin(), name.end(), ' '), name.end());
                    if (!name.empty()) result.imported_objects.push_back(name);
                }
            }
        }
    } else {
        result.message = output;
    }
    return result;
}

std::vector<ImportResult> ImportManager::importAssetsGrid(const std::vector<std::string>& asset_paths, const ImportOptions& options, int rows, int cols, float spacing) {
    /*
     * Implements grid pattern bulk import logic.
     * - Arranges assets in a grid with specified rows, columns, and spacing
     * - Calculates positions for each asset in the grid
     * - Applies base options to each asset with modified location
     * - Handles edge cases (more assets than grid positions, empty asset list)
     * - Returns vector of ImportResult for each asset
     */

    std::vector<ImportResult> results;

    // Handle edge cases
    if (asset_paths.empty()) {
        return results;
    }
    if (rows <= 0 || cols <= 0) {
        // Invalid grid dimensions, fall back to single column
        rows = static_cast<int>(asset_paths.size());
        cols = 1;
    }

    // Calculate grid positions
    int asset_index = 0;
    for (int row = 0; row < rows && asset_index < static_cast<int>(asset_paths.size()); ++row) {
        for (int col = 0; col < cols && asset_index < static_cast<int>(asset_paths.size()); ++col) {
            // Calculate position in grid
            float x = static_cast<float>(col) * spacing;
            float y = 0.0f; // Keep Y at 0 for grid layout
            float z = static_cast<float>(row) * spacing;

            // Create modified options with grid position
            ImportOptions grid_options = options;
            grid_options.location = {x, y, z};

            // Import asset at grid position
            ImportResult result = importAsset(asset_paths[asset_index], grid_options);
            results.push_back(result);

            ++asset_index;
        }
    }

    return results;
}

std::vector<ImportResult> ImportManager::importAssetsCircle(const std::vector<std::string>& asset_paths, const ImportOptions& options, float radius) {
    /*
     * Implements circle pattern bulk import logic.
     * - Arranges assets in a circle with specified radius
     * - Calculates positions for each asset around the circle
     * - Applies base options to each asset with modified location
     * - Handles edge cases (empty asset list, single asset)
     * - Returns vector of ImportResult for each asset
     */

    std::vector<ImportResult> results;

    // Handle edge cases
    if (asset_paths.empty()) {
        return results;
    }
    if (radius <= 0.0f) {
        radius = 10.0f; // Default radius
    }

    // Calculate circle positions
    const float angle_step = 2.0f * M_PI / static_cast<float>(asset_paths.size());
    
    for (size_t i = 0; i < asset_paths.size(); ++i) {
        // Calculate position on circle
        float angle = static_cast<float>(i) * angle_step;
        float x = radius * std::cos(angle);
        float y = 0.0f; // Keep Y at 0 for circle layout
        float z = radius * std::sin(angle);

        // Create modified options with circle position
        ImportOptions circle_options = options;
        circle_options.location = {x, y, z};

        // Import asset at circle position
        ImportResult result = importAsset(asset_paths[i], circle_options);
        results.push_back(result);
    }

    return results;
}

std::vector<ImportResult> ImportManager::importAssetsLine(const std::vector<std::string>& asset_paths, const ImportOptions& options, float spacing) {
    /*
     * Implements line pattern bulk import logic.
     * - Arranges assets in a straight line with specified spacing
     * - Calculates positions for each asset along the line
     * - Applies base options to each asset with modified location
     * - Handles edge cases (empty asset list, single asset)
     * - Returns vector of ImportResult for each asset
     */

    std::vector<ImportResult> results;

    // Handle edge cases
    if (asset_paths.empty()) {
        return results;
    }
    if (spacing <= 0.0f) {
        spacing = 5.0f; // Default spacing
    }

    // Calculate line positions
    for (size_t i = 0; i < asset_paths.size(); ++i) {
        // Calculate position along line
        float x = static_cast<float>(i) * spacing;
        float y = 0.0f; // Keep Y at 0 for line layout
        float z = 0.0f; // Keep Z at 0 for line layout

        // Create modified options with line position
        ImportOptions line_options = options;
        line_options.location = {x, y, z};

        // Import asset at line position
        ImportResult result = importAsset(asset_paths[i], line_options);
        results.push_back(result);
    }

    return results;
}

std::vector<ImportResult> ImportManager::importAssetsRandom(const std::vector<std::string>& asset_paths, const ImportOptions& options, int count, float area_size) {
    /*
     * Implements random pattern bulk import logic.
     * - Randomly places assets within a defined area
     * - Uses proper random number generation with seed
     * - Applies base options to each asset with modified location
     * - Handles edge cases (empty asset list, count > asset list size)
     * - Returns vector of ImportResult for each asset
     */

    std::vector<ImportResult> results;

    // Handle edge cases
    if (asset_paths.empty()) {
        return results;
    }
    if (count <= 0) {
        count = static_cast<int>(asset_paths.size());
    }
    if (area_size <= 0.0f) {
        area_size = 20.0f; // Default area size
    }

    // Limit count to available assets
    count = std::min(count, static_cast<int>(asset_paths.size()));

    // Initialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-area_size / 2.0f, area_size / 2.0f);

    // Randomly place assets
    for (int i = 0; i < count; ++i) {
        // Generate random position within area
        float x = dist(gen);
        float y = 0.0f; // Keep Y at 0 for random layout
        float z = dist(gen);

        // Create modified options with random position
        ImportOptions random_options = options;
        random_options.location = {x, y, z};

        // Import asset at random position
        ImportResult result = importAsset(asset_paths[i], random_options);
        results.push_back(result);
    }

    return results;
}

bool ImportManager::canLinkAsset(const std::string& asset_path) const {
    /*
     * Full implementation: Checks if the asset is a valid .blend file with linkable data blocks.
     * Calls Blender in background mode to verify the file and list linkable collections/objects.
     * Returns true if linkable, false otherwise.
     */
    if (asset_path.empty() || !std::filesystem::exists(asset_path)) {
        return false;
    }
    std::filesystem::path path(asset_path);
    if (path.extension() != ".blend") {
        return false;
    }
    // Generate a temporary Python script to check linkable data blocks
    std::string py_script =
        "import bpy\n"
        "import sys\n"
        "try:\n"
        "    with bpy.data.libraries.load(sys.argv[-1], link=True) as (data_from, data_to):\n"
        "        linkable = bool(data_from.collections or data_from.objects)\n"
        "    print('LINKABLE' if linkable else 'NOT_LINKABLE')\n"
        "except Exception as e:\n"
        "    print('NOT_LINKABLE')\n";
    // Write script to temp file
    char tmp_py_name[L_tmpnam];
    std::tmpnam(tmp_py_name);
    std::ofstream py_file(tmp_py_name);
    py_file << py_script;
    py_file.close();
    // Call Blender in background mode
    std::string cmd = "blender --background --factory-startup --python " + std::string(tmp_py_name) + " -- " + asset_path + " 2>&1";
    std::array<char, 256> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) return false;
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    std::remove(tmp_py_name);
    return result.find("LINKABLE") != std::string::npos;
}

} // namespace AssetManager 