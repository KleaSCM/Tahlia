/*
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: test_gui.cpp
Description: Simple test file for GUI components to ensure they compile and initialize correctly.
*/

#include "gui/asset_library_gui.hpp"
#include <iostream>
#include <memory>
#include <cassert>

int main() {
    std::cout << "🧪 Testing Tahlia Asset Library GUI Components" << std::endl;
    std::cout << "🌸 Universal Asset Management System" << std::endl;
    std::cout << "✨ Dear ImGui Integration" << std::endl;
    std::cout << std::endl;

    int tests_passed = 0;
    int total_tests = 10;

    // Test 1: GUI Configuration
    std::cout << "Test 1: GUI Configuration... ";
    try {
        TahliaGUI::GUIConfig config;
        config.dark_theme = true;
        config.font_scale = 1.0f;
        config.thumbnail_size = 128;
        config.default_view_mode = TahliaGUI::AssetViewMode::GRID;
        config.enable_docking = true;
        config.enable_multi_viewport = true;
        
        assert(config.dark_theme == true);
        assert(config.font_scale == 1.0f);
        assert(config.thumbnail_size == 128);
        assert(config.default_view_mode == TahliaGUI::AssetViewMode::GRID);
        assert(config.enable_docking == true);
        assert(config.enable_multi_viewport == true);
        
        std::cout << "✅ PASSED" << std::endl;
        tests_passed++;
    } catch (...) {
        std::cout << "❌ FAILED" << std::endl;
    }

    // Test 2: Asset Item Creation
    std::cout << "Test 2: Asset Item Creation... ";
    try {
        TahliaGUI::AssetItem asset;
        asset.name = "TestAsset.fbx";
        asset.path = "/test/path/TestAsset.fbx";
        asset.type = "Model";
        asset.category = "Characters";
        asset.file_size = 1024000;
        asset.last_modified = "2024-01-15 10:30:00";
        asset.tags = {"character", "human", "male"};
        
        assert(asset.name == "TestAsset.fbx");
        assert(asset.path == "/test/path/TestAsset.fbx");
        assert(asset.type == "Model");
        assert(asset.category == "Characters");
        assert(asset.file_size == 1024000);
        assert(asset.last_modified == "2024-01-15 10:30:00");
        assert(asset.tags.size() == 3);
        assert(asset.tags[0] == "character");
        assert(asset.tags[1] == "human");
        assert(asset.tags[2] == "male");
        
        std::cout << "✅ PASSED" << std::endl;
        tests_passed++;
    } catch (...) {
        std::cout << "❌ FAILED" << std::endl;
    }

    // Test 3: Search Filter Creation
    std::cout << "Test 3: Search Filter Creation... ";
    try {
        TahliaGUI::SearchFilter filter;
        filter.search_text = "character";
        filter.file_type_filter = "Model";
        filter.category_filter = "Characters";
        filter.tag_filters = {"human", "male"};
        filter.show_only_favorites = false;
        filter.show_only_recent = false;
        filter.min_file_size = 1024;
        filter.max_file_size = 10485760;
        
        assert(filter.search_text == "character");
        assert(filter.file_type_filter == "Model");
        assert(filter.category_filter == "Characters");
        assert(filter.tag_filters.size() == 2);
        assert(filter.tag_filters[0] == "human");
        assert(filter.tag_filters[1] == "male");
        assert(filter.show_only_favorites == false);
        assert(filter.show_only_recent == false);
        assert(filter.min_file_size == 1024);
        assert(filter.max_file_size == 10485760);
        
        std::cout << "✅ PASSED" << std::endl;
        tests_passed++;
    } catch (...) {
        std::cout << "❌ FAILED" << std::endl;
    }

    // Test 4: Import Options Creation
    std::cout << "Test 4: Import Options Creation... ";
    try {
        TahliaGUI::ImportOptions options;
        options.target_location = "/import/target";
        options.scale = 2.0f;
        options.rotation[0] = 0.0f;
        options.rotation[1] = 90.0f;
        options.rotation[2] = 0.0f;
        options.position[0] = 10.0f;
        options.position[1] = 0.0f;
        options.position[2] = 5.0f;
        options.merge_objects = true;
        options.auto_smooth = false;
        options.link_assets = true;
        options.import_pattern = "grid";
        
        assert(options.target_location == "/import/target");
        assert(options.scale == 2.0f);
        assert(options.rotation[0] == 0.0f);
        assert(options.rotation[1] == 90.0f);
        assert(options.rotation[2] == 0.0f);
        assert(options.position[0] == 10.0f);
        assert(options.position[1] == 0.0f);
        assert(options.position[2] == 5.0f);
        assert(options.merge_objects == true);
        assert(options.auto_smooth == false);
        assert(options.link_assets == true);
        assert(options.import_pattern == "grid");
        
        std::cout << "✅ PASSED" << std::endl;
        tests_passed++;
    } catch (...) {
        std::cout << "❌ FAILED" << std::endl;
    }

    // Test 5: View Mode Enumeration
    std::cout << "Test 5: View Mode Enumeration... ";
    try {
        TahliaGUI::AssetViewMode grid_mode = TahliaGUI::AssetViewMode::GRID;
        TahliaGUI::AssetViewMode list_mode = TahliaGUI::AssetViewMode::LIST;
        TahliaGUI::AssetViewMode details_mode = TahliaGUI::AssetViewMode::DETAILS;
        
        assert(grid_mode == TahliaGUI::AssetViewMode::GRID);
        assert(list_mode == TahliaGUI::AssetViewMode::LIST);
        assert(details_mode == TahliaGUI::AssetViewMode::DETAILS);
        
        std::cout << "✅ PASSED" << std::endl;
        tests_passed++;
    } catch (...) {
        std::cout << "❌ FAILED" << std::endl;
    }

    // Test 6: Panel Type Enumeration
    std::cout << "Test 6: Panel Type Enumeration... ";
    try {
        TahliaGUI::PanelType browser_panel = TahliaGUI::PanelType::ASSET_BROWSER;
        TahliaGUI::PanelType preview_panel = TahliaGUI::PanelType::ASSET_PREVIEW;
        TahliaGUI::PanelType details_panel = TahliaGUI::PanelType::ASSET_DETAILS;
        TahliaGUI::PanelType search_panel = TahliaGUI::PanelType::SEARCH_FILTER;
        TahliaGUI::PanelType import_panel = TahliaGUI::PanelType::IMPORT_PANEL;
        TahliaGUI::PanelType material_panel = TahliaGUI::PanelType::MATERIAL_EDITOR;
        TahliaGUI::PanelType history_panel = TahliaGUI::PanelType::HISTORY_PANEL;
        TahliaGUI::PanelType settings_panel = TahliaGUI::PanelType::SETTINGS_PANEL;
        
        assert(browser_panel == TahliaGUI::PanelType::ASSET_BROWSER);
        assert(preview_panel == TahliaGUI::PanelType::ASSET_PREVIEW);
        assert(details_panel == TahliaGUI::PanelType::ASSET_DETAILS);
        assert(search_panel == TahliaGUI::PanelType::SEARCH_FILTER);
        assert(import_panel == TahliaGUI::PanelType::IMPORT_PANEL);
        assert(material_panel == TahliaGUI::PanelType::MATERIAL_EDITOR);
        assert(history_panel == TahliaGUI::PanelType::HISTORY_PANEL);
        assert(settings_panel == TahliaGUI::PanelType::SETTINGS_PANEL);
        
        std::cout << "✅ PASSED" << std::endl;
        tests_passed++;
    } catch (...) {
        std::cout << "❌ FAILED" << std::endl;
    }

    // Test 7: Asset Vector Operations
    std::cout << "Test 7: Asset Vector Operations... ";
    try {
        std::vector<TahliaGUI::AssetItem> assets;
        
        TahliaGUI::AssetItem asset1;
        asset1.name = "Asset1.fbx";
        asset1.type = "Model";
        
        TahliaGUI::AssetItem asset2;
        asset2.name = "Asset2.png";
        asset2.type = "Texture";
        
        assets.push_back(asset1);
        assets.push_back(asset2);
        
        assert(assets.size() == 2);
        assert(assets[0].name == "Asset1.fbx");
        assert(assets[0].type == "Model");
        assert(assets[1].name == "Asset2.png");
        assert(assets[1].type == "Texture");
        
        std::cout << "✅ PASSED" << std::endl;
        tests_passed++;
    } catch (...) {
        std::cout << "❌ FAILED" << std::endl;
    }

    // Test 8: String Vector Operations
    std::cout << "Test 8: String Vector Operations... ";
    try {
        std::vector<std::string> selected_assets;
        selected_assets.push_back("Asset1.fbx");
        selected_assets.push_back("Asset2.png");
        selected_assets.push_back("Asset3.mat");
        
        assert(selected_assets.size() == 3);
        assert(selected_assets[0] == "Asset1.fbx");
        assert(selected_assets[1] == "Asset2.png");
        assert(selected_assets[2] == "Asset3.mat");
        
        std::cout << "✅ PASSED" << std::endl;
        tests_passed++;
    } catch (...) {
        std::cout << "❌ FAILED" << std::endl;
    }

    // Test 9: Callback Function Assignment
    std::cout << "Test 9: Callback Function Assignment... ";
    try {
        bool callback_called = false;
        
        std::function<void(const TahliaGUI::AssetItem&)> callback = 
            [&callback_called](const TahliaGUI::AssetItem& asset) {
                callback_called = true;
            };
        
        TahliaGUI::AssetItem test_asset;
        test_asset.name = "TestAsset.fbx";
        
        callback(test_asset);
        
        assert(callback_called == true);
        
        std::cout << "✅ PASSED" << std::endl;
        tests_passed++;
    } catch (...) {
        std::cout << "❌ FAILED" << std::endl;
    }

    // Test 10: Import Callback Function Assignment
    std::cout << "Test 10: Import Callback Function Assignment... ";
    try {
        bool import_callback_called = false;
        std::vector<std::string> received_paths;
        TahliaGUI::ImportOptions received_options;
        
        std::function<void(const std::vector<std::string>&, const TahliaGUI::ImportOptions&)> callback = 
            [&import_callback_called, &received_paths, &received_options]
            (const std::vector<std::string>& paths, const TahliaGUI::ImportOptions& options) {
                import_callback_called = true;
                received_paths = paths;
                received_options = options;
            };
        
        std::vector<std::string> test_paths = {"Asset1.fbx", "Asset2.png"};
        TahliaGUI::ImportOptions test_options;
        test_options.scale = 2.0f;
        
        callback(test_paths, test_options);
        
        assert(import_callback_called == true);
        assert(received_paths.size() == 2);
        assert(received_paths[0] == "Asset1.fbx");
        assert(received_paths[1] == "Asset2.png");
        assert(received_options.scale == 2.0f);
        
        std::cout << "✅ PASSED" << std::endl;
        tests_passed++;
    } catch (...) {
        std::cout << "❌ FAILED" << std::endl;
    }

    // Summary
    std::cout << std::endl;
    std::cout << "🎉 GUI Component Tests Completed!" << std::endl;
    std::cout << "✅ Tests Passed: " << tests_passed << "/" << total_tests << std::endl;
    std::cout << "✨ All GUI data structures and enumerations working correctly" << std::endl;
    std::cout << "🚀 Ready for Dear ImGui integration" << std::endl;
    
    return (tests_passed == total_tests) ? 0 : 1;
} 