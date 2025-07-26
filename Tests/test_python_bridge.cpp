/**
 * @file test_python_bridge.cpp
 * @author KleaSCM
 * @email KleaSCM@gmail.com
 * @brief Comprehensive unit tests for PythonBridge using simple test harness
 * 
 * Tests the PythonBridge's ability to expose C++ core to Python, manage Blender context,
 * and provide elegant Python APIs with proper error handling.
 */

#include "test_harness.hpp"
#include "../include/python_bridge.hpp"
#include "../include/asset_manager.hpp"
#include "../include/import_manager.hpp"
#include "../include/material_manager.hpp"
#include "../include/import_history.hpp"
#include <iostream>
#include <memory>

using namespace TestHarness;

int main() {
    TestRunner runner;
    
    runner.beginSuite("PythonBridge Tests");
    
    // Test 1: PythonBridge creation
    runner.runTest("PythonBridge Constructor", []() -> bool {
        PythonBridge::PythonBridge bridge;
        return true; // If constructor doesn't throw, test passes
    });
    
    // Test 2: Initialize bridge
    runner.runTest("Initialize Bridge", []() -> bool {
        PythonBridge::PythonBridge bridge;
        PythonBridge::PythonModuleConfig config;
        config.module_name = "test_module";
        config.version = "1.0.0";
        config.description = "Test module";
        config.enable_debug_mode = true;
        config.enable_context_preservation = true;
        
        bool result = bridge.initialize(config);
        return result && bridge.isInitialized();
    });
    
    // Test 3: Initialize with default config
    runner.runTest("Initialize with Default Config", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bool result = bridge.initialize();
        return result && bridge.isInitialized();
    });
    
    // Test 4: Set managers
    runner.runTest("Set Managers", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        auto asset_manager = std::make_shared<AssetManager::AssetManager>();
        auto import_manager = std::make_shared<AssetManager::ImportManager>();
        auto material_manager = std::make_shared<AssetManager::MaterialManager>();
        auto import_history = std::make_shared<AssetManager::ImportHistory>();
        
        bridge.setAssetManager(asset_manager);
        bridge.setImportManager(import_manager);
        bridge.setMaterialManager(material_manager);
        bridge.setImportHistory(import_history);
        
        return true; // If no exceptions, test passes
    });
    
    // Test 5: Capture context
    runner.runTest("Capture Context", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        PythonBridge::BlenderContext context = bridge.captureContext();
        
        // Check that context has been populated (even with placeholder values)
        return !context.selected_objects.empty() || !context.active_object.empty();
    });
    
    // Test 6: Context stack operations
    runner.runTest("Context Stack Operations", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        // Initially empty
        size_t initial_size = bridge.getContextStackSize();
        if (initial_size != 0) {
            std::cerr << "Initial size: " << initial_size << " (expected 0)" << std::endl;
            return false;
        }
        
        // Push context
        bridge.pushContext();
        size_t after_first_push = bridge.getContextStackSize();
        if (after_first_push != 1) {
            std::cerr << "After first push: " << after_first_push << " (expected 1)" << std::endl;
            return false;
        }
        
        // Push another context
        bridge.pushContext();
        size_t after_second_push = bridge.getContextStackSize();
        if (after_second_push != 2) {
            std::cerr << "After second push: " << after_second_push << " (expected 2)" << std::endl;
            return false;
        }
        
        // Pop context
        bridge.popContext();
        size_t after_pop = bridge.getContextStackSize();
        if (after_pop != 1) {
            std::cerr << "After pop: " << after_pop << " (expected 1)" << std::endl;
            return false;
        }
        
        // Clear stack
        bridge.clearContextStack();
        size_t after_clear = bridge.getContextStackSize();
        if (after_clear != 0) {
            std::cerr << "After clear: " << after_clear << " (expected 0)" << std::endl;
            return false;
        }
        
        return true;
    });
    
    // Test 7: Preserve context
    runner.runTest("Preserve Context", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        bool operation_called = false;
        bool result = bridge.preserveContext([&operation_called]() {
            operation_called = true;
        });
        
        return result && operation_called;
    });
    
    // Test 8: Error handling
    runner.runTest("Error Handling", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        // Initially no error
        if (bridge.hasError()) {
            return false;
        }
        
        // Set error handler
        bool handler_called = false;
        bridge.setPythonExceptionHandler([&handler_called](const std::string& error) {
            handler_called = true;
        });
        
        // Trigger an error (this would normally happen during Python operations)
        // For now, we'll just test the basic functionality
        
        return !bridge.hasError(); // Should still be no error
    });
    
    // Test 9: Configuration
    runner.runTest("Configuration", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        // Test debug mode
        bridge.setDebugMode(true);
        auto config = bridge.getConfig();
        if (!config.enable_debug_mode) {
            return false;
        }
        
        // Test context preservation
        bridge.setContextPreservation(false);
        config = bridge.getConfig();
        if (config.enable_context_preservation) {
            return false;
        }
        
        // Test max context stack size
        bridge.setMaxContextStackSize(5);
        config = bridge.getConfig();
        if (config.max_context_stack_size != 5) {
            return false;
        }
        
        return true;
    });
    
    // Test 10: Cleanup
    runner.runTest("Cleanup", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        if (!bridge.isInitialized()) {
            return false;
        }
        
        bridge.cleanup();
        
        return !bridge.isInitialized();
    });
    
    // Test 11: Python-friendly import asset
    runner.runTest("Python-friendly Import Asset", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        auto import_manager = std::make_shared<AssetManager::ImportManager>();
        bridge.setImportManager(import_manager);
        
        // Test with empty options
        std::map<std::string, std::string> options;
        PythonBridge::PythonResult result = bridge.importAssetPython("test_asset.fbx", options);
        
        // Should return an error result since the asset doesn't exist
        return !result.success;
    });
    
    // Test 12: Python-friendly import assets grid
    runner.runTest("Python-friendly Import Assets Grid", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        auto import_manager = std::make_shared<AssetManager::ImportManager>();
        bridge.setImportManager(import_manager);
        
        std::vector<std::string> asset_paths = {"asset1.fbx", "asset2.fbx"};
        std::map<std::string, std::string> options;
        options["location_x"] = "0.0";
        options["location_y"] = "0.0";
        options["location_z"] = "0.0";
        
        std::vector<PythonBridge::PythonResult> results = bridge.importAssetsGridPython(asset_paths, options, 2, 2, 5.0f);
        
        // Should return a vector of error results
        return results.size() == 2;
    });
    
    // Test 13: Python-friendly import assets circle
    runner.runTest("Python-friendly Import Assets Circle", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        auto import_manager = std::make_shared<AssetManager::ImportManager>();
        bridge.setImportManager(import_manager);
        
        std::vector<std::string> asset_paths = {"asset1.fbx", "asset2.fbx", "asset3.fbx"};
        std::map<std::string, std::string> options;
        options["scale_x"] = "1.0";
        options["scale_y"] = "1.0";
        options["scale_z"] = "1.0";
        
        std::vector<PythonBridge::PythonResult> results = bridge.importAssetsCirclePython(asset_paths, options, 10.0f);
        
        // Should return a vector of error results
        return results.size() == 3;
    });
    
    // Test 14: Python-friendly import assets line
    runner.runTest("Python-friendly Import Assets Line", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        auto import_manager = std::make_shared<AssetManager::ImportManager>();
        bridge.setImportManager(import_manager);
        
        std::vector<std::string> asset_paths = {"asset1.fbx", "asset2.fbx"};
        std::map<std::string, std::string> options;
        options["rotation_x"] = "0.0";
        options["rotation_y"] = "0.0";
        options["rotation_z"] = "0.0";
        
        std::vector<PythonBridge::PythonResult> results = bridge.importAssetsLinePython(asset_paths, options, 5.0f);
        
        // Should return a vector of error results
        return results.size() == 2;
    });
    
    // Test 15: Python-friendly import assets random
    runner.runTest("Python-friendly Import Assets Random", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        auto import_manager = std::make_shared<AssetManager::ImportManager>();
        bridge.setImportManager(import_manager);
        
        std::vector<std::string> asset_paths = {"asset1.fbx", "asset2.fbx"};
        std::map<std::string, std::string> options;
        options["merge_objects"] = "true";
        
        std::vector<PythonBridge::PythonResult> results = bridge.importAssetsRandomPython(asset_paths, options, 2, 20.0f);
        
        // Should return a vector of error results
        return results.size() == 2;
    });
    
    // Test 16: Python-friendly create material
    runner.runTest("Python-friendly Create Material", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        auto material_manager = std::make_shared<AssetManager::MaterialManager>();
        bridge.setMaterialManager(material_manager);
        
        std::map<std::string, std::string> options;
        options["metallic"] = "0.5";
        options["roughness"] = "0.3";
        
        PythonBridge::PythonResult result = bridge.createMaterialPython("TestMaterial", options);
        
        // Should return an error result since we're not in Blender
        return !result.success;
    });
    
    // Test 17: Python-friendly create PBR material
    runner.runTest("Python-friendly Create PBR Material", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        auto material_manager = std::make_shared<AssetManager::MaterialManager>();
        bridge.setMaterialManager(material_manager);
        
        std::map<std::string, std::string> options;
        options["specular"] = "0.7";
        
        PythonBridge::PythonResult result = bridge.createPBRMaterialPython("TestPBRMaterial", options);
        
        // Should return an error result since we're not in Blender
        return !result.success;
    });
    
    // Test 18: Python-friendly create quick material
    runner.runTest("Python-friendly Create Quick Material", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        auto material_manager = std::make_shared<AssetManager::MaterialManager>();
        bridge.setMaterialManager(material_manager);
        
        PythonBridge::PythonResult result = bridge.createQuickMaterialPython("TestQuickMaterial", "metal");
        
        // Should return an error result since we're not in Blender
        return !result.success;
    });
    
    // Test 19: Python-friendly undo last import
    runner.runTest("Python-friendly Undo Last Import", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        auto import_history = std::make_shared<AssetManager::ImportHistory>();
        bridge.setImportHistory(import_history);
        
        PythonBridge::PythonResult result = bridge.undoLastImportPython();
        
        // Should return an error result since there's no history
        return !result.success;
    });
    
    // Test 20: Python-friendly undo specific import
    runner.runTest("Python-friendly Undo Specific Import", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        auto import_history = std::make_shared<AssetManager::ImportHistory>();
        bridge.setImportHistory(import_history);
        
        PythonBridge::PythonResult result = bridge.undoImportPython("non_existent_id");
        
        // Should return an error result since the entry doesn't exist
        return !result.success;
    });
    
    // Test 21: Python-friendly get history
    runner.runTest("Python-friendly Get History", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        auto import_history = std::make_shared<AssetManager::ImportHistory>();
        bridge.setImportHistory(import_history);
        
        std::vector<PythonBridge::PythonResult> results = bridge.getHistoryPython();
        
        // Should return an empty vector
        return results.empty();
    });
    
    // Test 22: Python-friendly get history stats
    runner.runTest("Python-friendly Get History Stats", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        auto import_history = std::make_shared<AssetManager::ImportHistory>();
        bridge.setImportHistory(import_history);
        
        PythonBridge::PythonResult result = bridge.getHistoryStatsPython();
        
        // Should return stats result
        return result.success && result.data.find("total_imports") != result.data.end();
    });
    
    // Test 23: Convert ImportOptions to map
    runner.runTest("Convert ImportOptions to Map", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        AssetManager::ImportOptions options;
        options.location = {1.0f, 2.0f, 3.0f};
        options.rotation = {0.5f, 1.0f, 1.5f};
        options.scale = {2.0f, 0.5f, 1.0f};
        options.import_materials = true;
        options.merge_objects = false;
        options.auto_smooth = true;
        options.collection_name = "TestCollection";
        options.link_instead_of_import = false;
        
        std::map<std::string, std::string> map = bridge.convertImportOptionsToMap(options);
        
        return map.find("location_x") != map.end() && 
               map.find("rotation_x") != map.end() && 
               map.find("scale_x") != map.end() &&
               map.find("import_materials") != map.end() &&
               map.find("merge_objects") != map.end() &&
               map.find("auto_smooth") != map.end() &&
               map.find("collection_name") != map.end() &&
               map.find("link_instead_of_import") != map.end();
    });
    
    // Test 24: Convert map to ImportOptions
    runner.runTest("Convert Map to ImportOptions", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        std::map<std::string, std::string> options;
        options["location_x"] = "1.0";
        options["location_y"] = "2.0";
        options["location_z"] = "3.0";
        options["rotation_x"] = "0.5";
        options["rotation_y"] = "1.0";
        options["rotation_z"] = "1.5";
        options["scale_x"] = "2.0";
        options["scale_y"] = "0.5";
        options["scale_z"] = "1.0";
        options["import_materials"] = "true";
        options["merge_objects"] = "false";
        options["auto_smooth"] = "true";
        options["collection_name"] = "TestCollection";
        options["link_instead_of_import"] = "false";
        
        AssetManager::ImportOptions import_options = bridge.convertMapToImportOptions(options);
        
        return std::get<0>(import_options.location) == 1.0f &&
               std::get<1>(import_options.location) == 2.0f &&
               std::get<2>(import_options.location) == 3.0f &&
               import_options.import_materials &&
               !import_options.merge_objects &&
               import_options.auto_smooth &&
               import_options.collection_name == "TestCollection" &&
               !import_options.link_instead_of_import;
    });
    
    // Test 25: Convert ImportResult to map
    runner.runTest("Convert ImportResult to Map", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        AssetManager::ImportResult result;
        result.asset_path = "test_asset.fbx";
        result.success = true;
        result.message = "Import successful";
        result.imported_objects = {"TestObject"};
        
        PythonBridge::PythonResult python_result = bridge.convertImportResultToMap(result);
        
        return python_result.success &&
               python_result.message == "Import successful" &&
               python_result.data.find("asset_path") != python_result.data.end() &&
               python_result.data["asset_path"] == "test_asset.fbx" &&
               python_result.list_data.size() == 1 &&
               python_result.list_data[0] == "TestObject";
    });
    
    // Test 26: Convert MaterialOptions to map
    runner.runTest("Convert MaterialOptions to Map", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        AssetManager::MaterialOptions options;
        options.name = "TestMaterial";
        options.use_nodes = true;
        options.metallic = 0.5f;
        options.roughness = 0.3f;
        options.specular = 0.7f;
        
        std::map<std::string, std::string> map = bridge.convertMaterialOptionsToMap(options);
        
        return map.find("name") != map.end() &&
               map.find("use_nodes") != map.end() &&
               map.find("metallic") != map.end() &&
               map.find("roughness") != map.end() &&
               map.find("specular") != map.end() &&
               map["name"] == "TestMaterial" &&
               map["use_nodes"] == "true" &&
               map["metallic"] == "0.500000";
    });
    
    // Test 27: Convert map to MaterialOptions
    runner.runTest("Convert Map to MaterialOptions", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        std::map<std::string, std::string> options;
        options["name"] = "TestMaterial";
        options["use_nodes"] = "true";
        options["metallic"] = "0.5";
        options["roughness"] = "0.3";
        options["specular"] = "0.7";
        
        AssetManager::MaterialOptions material_options = bridge.convertMapToMaterialOptions(options);
        
        return material_options.name == "TestMaterial" &&
               material_options.use_nodes &&
               material_options.metallic == 0.5f &&
               material_options.roughness == 0.3f &&
               material_options.specular == 0.7f;
    });
    
    // Test 28: Convert MaterialResult to map
    runner.runTest("Convert MaterialResult to Map", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        AssetManager::MaterialResult result;
        result.success = true;
        result.message = "Material created successfully";
        result.created_materials = {"TestMaterial"};
        result.assigned_textures = {"test_texture.png"};
        
        PythonBridge::PythonResult python_result = bridge.convertMaterialResultToMap(result);
        
        return python_result.success &&
               python_result.message == "Material created successfully" &&
               python_result.list_data.size() == 1 &&
               python_result.list_data[0] == "TestMaterial";
    });
    
    // Test 29: Convert HistoryEntry to map
    runner.runTest("Convert HistoryEntry to Map", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        AssetManager::ImportHistoryEntry entry;
        entry.id = "test_001";
        entry.asset_path = "test_asset.fbx";
        entry.import_type = "import";
        entry.success = true;
        entry.message = "Import successful";
        entry.imported_objects = {"TestObject"};
        
        PythonBridge::PythonResult python_result = bridge.convertHistoryEntryToMap(entry);
        
        return python_result.success &&
               python_result.message == "Import successful" &&
               python_result.data.find("id") != python_result.data.end() &&
               python_result.data.find("asset_path") != python_result.data.end() &&
               python_result.data.find("import_type") != python_result.data.end() &&
               python_result.data["id"] == "test_001" &&
               python_result.data["asset_path"] == "test_asset.fbx" &&
               python_result.data["import_type"] == "import" &&
               python_result.list_data.size() == 1 &&
               python_result.list_data[0] == "TestObject";
    });
    
    // Test 30: Convert HistoryStats to map
    runner.runTest("Convert HistoryStats to Map", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        AssetManager::HistoryStats stats = {};
        stats.total_imports = 10;
        stats.successful_imports = 8;
        stats.failed_imports = 2;
        stats.linked_assets = 3;
        stats.imported_assets = 7;
        
        PythonBridge::PythonResult python_result = bridge.convertHistoryStatsToMap(stats);
        
        return python_result.success &&
               python_result.message == "History statistics retrieved successfully" &&
               python_result.data.find("total_imports") != python_result.data.end() &&
               python_result.data.find("successful_imports") != python_result.data.end() &&
               python_result.data.find("failed_imports") != python_result.data.end() &&
               python_result.data.find("linked_assets") != python_result.data.end() &&
               python_result.data.find("imported_assets") != python_result.data.end() &&
               python_result.data["total_imports"] == "10" &&
               python_result.data["successful_imports"] == "8" &&
               python_result.data["failed_imports"] == "2" &&
               python_result.data["linked_assets"] == "3" &&
               python_result.data["imported_assets"] == "7";
    });
    
    // Test 31: Convert UndoResult to map
    runner.runTest("Convert UndoResult to Map", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        AssetManager::UndoResult result;
        result.success = true;
        result.message = "Undo successful";
        result.restored_objects = {"RestoredObject"};
        result.removed_objects = {"RemovedObject"};
        
        PythonBridge::PythonResult python_result = bridge.convertUndoResultToMap(result);
        
        return python_result.success &&
               python_result.message == "Undo successful" &&
               python_result.list_data.size() == 1 &&
               python_result.list_data[0] == "RestoredObject";
    });
    
    // Test 32: Create error result
    runner.runTest("Create Error Result", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        PythonBridge::PythonResult result = bridge.createErrorResult("Test error message");
        
        return !result.success &&
               result.message == "Test error message";
    });
    
    // Test 33: Create success result
    runner.runTest("Create Success Result", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        PythonBridge::PythonResult result = bridge.createSuccessResult("Test success message");
        
        return result.success &&
               result.message == "Test success message";
    });
    
    // Test 34: Create success result with default message
    runner.runTest("Create Success Result with Default Message", []() -> bool {
        PythonBridge::PythonBridge bridge;
        bridge.initialize();
        
        PythonBridge::PythonResult result = bridge.createSuccessResult();
        
        return result.success &&
               result.message == "Success";
    });
    
    runner.printSummary();
    
    return runner.getFailedCount() == 0 ? 0 : 1;
} 