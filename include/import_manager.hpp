/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * Name: import_manager.hpp
 * Description: Header file for the ImportManager class providing modular, high-performance asset importing capabilities.
 *              Supports single and bulk asset import with configurable options and pattern arrangements.
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

#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <any>
#include <memory>
#include <filesystem>

namespace AssetManager {

struct ImportOptions {
    std::tuple<float, float, float> location = {0.0f, 0.0f, 0.0f};
    std::tuple<float, float, float> rotation = {0.0f, 0.0f, 0.0f};
    std::tuple<float, float, float> scale = {1.0f, 1.0f, 1.0f};
    bool import_materials = true;
    bool merge_objects = false;
    bool auto_smooth = true;
    std::string collection_name;
    bool link_instead_of_import = false;
    // Extend with more options as needed
};

struct ImportResult {
    std::string asset_path;
    bool success;
    std::string message;
    std::vector<std::string> imported_objects;
    std::map<std::string, std::any> metadata;
};

class ImportManager {
public:
    ImportManager();
    ~ImportManager();

    // Import a single asset with options
    ImportResult importAsset(const std::string& asset_path, const ImportOptions& options = {});

    // Bulk import assets in a grid pattern
    std::vector<ImportResult> importAssetsGrid(const std::vector<std::string>& asset_paths, const ImportOptions& options = {}, int rows = 1, int cols = 1, float spacing = 5.0f);

    // Bulk import assets in a circle pattern
    std::vector<ImportResult> importAssetsCircle(const std::vector<std::string>& asset_paths, const ImportOptions& options = {}, float radius = 10.0f);

    // Bulk import assets in a line pattern
    std::vector<ImportResult> importAssetsLine(const std::vector<std::string>& asset_paths, const ImportOptions& options = {}, float spacing = 5.0f);

    // Bulk import assets in a random pattern
    std::vector<ImportResult> importAssetsRandom(const std::vector<std::string>& asset_paths, const ImportOptions& options = {}, int count = 10, float area_size = 20.0f);

    // Set integration points (e.g., AssetManager reference)
    void setAssetManager(std::shared_ptr<class AssetManager> manager);

    // Utility: Check if asset can be linked instead of imported
    bool canLinkAsset(const std::string& asset_path) const;

private:
    std::shared_ptr<class AssetManager> asset_manager_;
    // Internal helpers and state
};

} // namespace AssetManager 