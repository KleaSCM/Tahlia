/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * Name: test_import_manager.cpp
 * Description: Unit tests for the ImportManager class using Catch2.
 *              Tests single and bulk import patterns, linking logic, and integration with AssetManager.
 *
 * Architecture:
 * - Modular test cases for each import pattern and feature
 * - Uses stubs/mocks for AssetManager integration
 * - Validates ImportResult structure and error handling
 *
 * Key Features:
 * - Test single asset import with options
 * - Test grid, circle, line, and random bulk imports
 * - Test linking logic and error handling
 * - Ensure integration points with AssetManager are functional
 */

#include <catch2/catch_all.hpp>
#include "../include/import_manager.hpp"
#include <memory>
#include <vector>
#include <string>

using namespace AssetManager;

class MockAssetManager : public std::enable_shared_from_this<MockAssetManager> {
public:
    // Add mock methods as needed
};

TEST_CASE("ImportManager: Single Asset Import", "[import]") {
    ImportManager manager;
    auto mock_manager = std::make_shared<MockAssetManager>();
    manager.setAssetManager(std::static_pointer_cast<AssetManager>(mock_manager));
    ImportOptions options;
    options.location = {1.0f, 2.0f, 3.0f};
    options.rotation = {0.0f, 0.0f, 0.0f};
    options.scale = {1.0f, 1.0f, 1.0f};
    options.import_materials = true;
    options.merge_objects = false;
    options.auto_smooth = true;
    options.collection_name = "TestCollection";
    options.link_instead_of_import = false;
    ImportResult result = manager.importAsset("dummy_asset.obj", options);
    REQUIRE(result.asset_path == "dummy_asset.obj");
    REQUIRE_FALSE(result.success);
    REQUIRE(result.message.find("not yet implemented") != std::string::npos);
}

TEST_CASE("ImportManager: Grid Bulk Import", "[import][grid]") {
    ImportManager manager;
    std::vector<std::string> assets = {"a.obj", "b.obj", "c.obj"};
    auto results = manager.importAssetsGrid(assets, {}, 2, 2, 5.0f);
    REQUIRE(results.size() == assets.size());
    for (size_t i = 0; i < assets.size(); ++i) {
        REQUIRE(results[i].asset_path == assets[i]);
    }
}

TEST_CASE("ImportManager: Circle Bulk Import", "[import][circle]") {
    ImportManager manager;
    std::vector<std::string> assets = {"a.obj", "b.obj", "c.obj"};
    auto results = manager.importAssetsCircle(assets, {}, 10.0f);
    REQUIRE(results.size() == assets.size());
}

TEST_CASE("ImportManager: Line Bulk Import", "[import][line]") {
    ImportManager manager;
    std::vector<std::string> assets = {"a.obj", "b.obj", "c.obj"};
    auto results = manager.importAssetsLine(assets, {}, 5.0f);
    REQUIRE(results.size() == assets.size());
}

TEST_CASE("ImportManager: Random Bulk Import", "[import][random]") {
    ImportManager manager;
    std::vector<std::string> assets = {"a.obj", "b.obj", "c.obj"};
    auto results = manager.importAssetsRandom(assets, {}, 2, 20.0f);
    REQUIRE(results.size() == 2);
}

TEST_CASE("ImportManager: Linking Logic", "[import][link]") {
    ImportManager manager;
    REQUIRE_FALSE(manager.canLinkAsset("dummy_asset.obj"));
} 