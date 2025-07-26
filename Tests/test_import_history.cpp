/**
 * @file test_import_history.cpp
 * @author KleaSCM
 * @email KleaSCM@gmail.com
 * @brief Comprehensive unit tests for ImportHistory using simple test harness
 * 
 * Tests the ImportHistory's ability to track imported assets, manage undo operations,
 * clear history, and provide detailed analytics and reporting.
 */

#include "test_harness.hpp"
#include "../include/import_history.hpp"
#include <iostream>
#include <memory>
#include <chrono>

using namespace TestHarness;

int main() {
    TestRunner runner;
    
    runner.beginSuite("ImportHistory Tests");
    
    // Test 1: ImportHistory creation
    runner.runTest("ImportHistory Constructor", []() -> bool {
        AssetManager::ImportHistory history;
        return true; // If constructor doesn't throw, test passes
    });
    
    // Test 2: Add entry to history
    runner.runTest("Add Entry to History", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry;
        entry.id = "test_001";
        entry.asset_path = "test_asset.fbx";
        entry.import_type = "import";
        entry.timestamp = std::chrono::system_clock::now();
        entry.success = true;
        entry.message = "Import successful";
        entry.imported_objects = {"TestObject"};
        
        history.addEntry(entry);
        
        return history.getHistorySize() == 1;
    });
    
    // Test 3: Get history
    runner.runTest("Get History", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry;
        entry.id = "test_002";
        entry.asset_path = "test_asset.fbx";
        entry.import_type = "import";
        entry.timestamp = std::chrono::system_clock::now();
        entry.success = true;
        entry.message = "Import successful";
        entry.imported_objects = {"TestObject"};
        
        history.addEntry(entry);
        
        auto history_list = history.getHistory();
        return history_list.size() == 1 && history_list[0].id == "test_002";
    });
    
    // Test 4: Get history by asset
    runner.runTest("Get History by Asset", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry1;
        entry1.id = "test_003";
        entry1.asset_path = "asset1.fbx";
        entry1.import_type = "import";
        entry1.timestamp = std::chrono::system_clock::now();
        entry1.success = true;
        entry1.message = "Import successful";
        entry1.imported_objects = {"Object1"};
        
        AssetManager::ImportHistoryEntry entry2;
        entry2.id = "test_004";
        entry2.asset_path = "asset2.fbx";
        entry2.import_type = "link";
        entry2.timestamp = std::chrono::system_clock::now();
        entry2.success = true;
        entry2.message = "Link successful";
        entry2.imported_objects = {"Object2"};
        
        history.addEntry(entry1);
        history.addEntry(entry2);
        
        auto asset1_history = history.getHistoryByAsset("asset1.fbx");
        return asset1_history.size() == 1 && asset1_history[0].asset_path == "asset1.fbx";
    });
    
    // Test 5: Get history by type
    runner.runTest("Get History by Type", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry1;
        entry1.id = "test_005";
        entry1.asset_path = "asset1.fbx";
        entry1.import_type = "import";
        entry1.timestamp = std::chrono::system_clock::now();
        entry1.success = true;
        entry1.message = "Import successful";
        entry1.imported_objects = {"Object1"};
        
        AssetManager::ImportHistoryEntry entry2;
        entry2.id = "test_006";
        entry2.asset_path = "asset2.fbx";
        entry2.import_type = "link";
        entry2.timestamp = std::chrono::system_clock::now();
        entry2.success = true;
        entry2.message = "Link successful";
        entry2.imported_objects = {"Object2"};
        
        history.addEntry(entry1);
        history.addEntry(entry2);
        
        auto import_history = history.getHistoryByType("import");
        auto link_history = history.getHistoryByType("link");
        
        return import_history.size() == 1 && link_history.size() == 1;
    });
    
    // Test 6: Can undo
    runner.runTest("Can Undo", []() -> bool {
        AssetManager::ImportHistory history;
        
        // Initially should not be able to undo
        if (history.canUndo()) {
            return false;
        }
        
        AssetManager::ImportHistoryEntry entry;
        entry.id = "test_007";
        entry.asset_path = "test_asset.fbx";
        entry.import_type = "import";
        entry.timestamp = std::chrono::system_clock::now();
        entry.success = true;
        entry.message = "Import successful";
        entry.imported_objects = {"TestObject"};
        
        history.addEntry(entry);
        
        // Now should be able to undo
        return history.canUndo();
    });
    
    // Test 7: Undo last import
    runner.runTest("Undo Last Import", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry;
        entry.id = "test_008";
        entry.asset_path = "test_asset.fbx";
        entry.import_type = "import";
        entry.timestamp = std::chrono::system_clock::now();
        entry.success = true;
        entry.message = "Import successful";
        entry.imported_objects = {"TestObject"};
        
        history.addEntry(entry);
        
        auto undo_result = history.undoLastImport();
        
        // Should succeed (even if Blender objects don't exist in test environment)
        return undo_result.success && history.getHistorySize() == 0;
    });
    
    // Test 8: Undo specific import
    runner.runTest("Undo Specific Import", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry;
        entry.id = "test_009";
        entry.asset_path = "test_asset.fbx";
        entry.import_type = "import";
        entry.timestamp = std::chrono::system_clock::now();
        entry.success = true;
        entry.message = "Import successful";
        entry.imported_objects = {"TestObject"};
        
        history.addEntry(entry);
        
        auto undo_result = history.undoImport("test_009");
        
        return undo_result.success && history.getHistorySize() == 0;
    });
    
    // Test 9: Undo non-existent import
    runner.runTest("Undo Non-existent Import", []() -> bool {
        AssetManager::ImportHistory history;
        
        auto undo_result = history.undoImport("non_existent_id");
        
        return !undo_result.success && undo_result.message.find("not found") != std::string::npos;
    });
    
    // Test 10: Clear history
    runner.runTest("Clear History", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry;
        entry.id = "test_010";
        entry.asset_path = "test_asset.fbx";
        entry.import_type = "import";
        entry.timestamp = std::chrono::system_clock::now();
        entry.success = true;
        entry.message = "Import successful";
        entry.imported_objects = {"TestObject"};
        
        history.addEntry(entry);
        
        history.clearHistory();
        
        return history.getHistorySize() == 0 && history.isEmpty();
    });
    
    // Test 11: Clear history by asset
    runner.runTest("Clear History by Asset", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry1;
        entry1.id = "test_011";
        entry1.asset_path = "asset1.fbx";
        entry1.import_type = "import";
        entry1.timestamp = std::chrono::system_clock::now();
        entry1.success = true;
        entry1.message = "Import successful";
        entry1.imported_objects = {"Object1"};
        
        AssetManager::ImportHistoryEntry entry2;
        entry2.id = "test_012";
        entry2.asset_path = "asset2.fbx";
        entry2.import_type = "link";
        entry2.timestamp = std::chrono::system_clock::now();
        entry2.success = true;
        entry2.message = "Link successful";
        entry2.imported_objects = {"Object2"};
        
        history.addEntry(entry1);
        history.addEntry(entry2);
        
        history.clearHistoryByAsset("asset1.fbx");
        
        return history.getHistorySize() == 1 && history.getHistory()[0].asset_path == "asset2.fbx";
    });
    
    // Test 12: Clear history by type
    runner.runTest("Clear History by Type", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry1;
        entry1.id = "test_013";
        entry1.asset_path = "asset1.fbx";
        entry1.import_type = "import";
        entry1.timestamp = std::chrono::system_clock::now();
        entry1.success = true;
        entry1.message = "Import successful";
        entry1.imported_objects = {"Object1"};
        
        AssetManager::ImportHistoryEntry entry2;
        entry2.id = "test_014";
        entry2.asset_path = "asset2.fbx";
        entry2.import_type = "link";
        entry2.timestamp = std::chrono::system_clock::now();
        entry2.success = true;
        entry2.message = "Link successful";
        entry2.imported_objects = {"Object2"};
        
        history.addEntry(entry1);
        history.addEntry(entry2);
        
        history.clearHistoryByType("import");
        
        return history.getHistorySize() == 1 && history.getHistory()[0].import_type == "link";
    });
    
    // Test 13: Clear failed imports
    runner.runTest("Clear Failed Imports", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry1;
        entry1.id = "test_015";
        entry1.asset_path = "asset1.fbx";
        entry1.import_type = "import";
        entry1.timestamp = std::chrono::system_clock::now();
        entry1.success = true;
        entry1.message = "Import successful";
        entry1.imported_objects = {"Object1"};
        
        AssetManager::ImportHistoryEntry entry2;
        entry2.id = "test_016";
        entry2.asset_path = "asset2.fbx";
        entry2.import_type = "link";
        entry2.timestamp = std::chrono::system_clock::now();
        entry2.success = false;
        entry2.message = "Import failed";
        entry2.imported_objects = {};
        
        history.addEntry(entry1);
        history.addEntry(entry2);
        
        history.clearFailedImports();
        
        return history.getHistorySize() == 1 && history.getHistory()[0].success;
    });
    
    // Test 14: Clear successful imports
    runner.runTest("Clear Successful Imports", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry1;
        entry1.id = "test_017";
        entry1.asset_path = "asset1.fbx";
        entry1.import_type = "import";
        entry1.timestamp = std::chrono::system_clock::now();
        entry1.success = true;
        entry1.message = "Import successful";
        entry1.imported_objects = {"Object1"};
        
        AssetManager::ImportHistoryEntry entry2;
        entry2.id = "test_018";
        entry2.asset_path = "asset2.fbx";
        entry2.import_type = "link";
        entry2.timestamp = std::chrono::system_clock::now();
        entry2.success = false;
        entry2.message = "Import failed";
        entry2.imported_objects = {};
        
        history.addEntry(entry1);
        history.addEntry(entry2);
        
        history.clearSuccessfulImports();
        
        return history.getHistorySize() == 1 && !history.getHistory()[0].success;
    });
    
    // Test 15: Get stats
    runner.runTest("Get Stats", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry1;
        entry1.id = "test_019";
        entry1.asset_path = "asset1.fbx";
        entry1.import_type = "import";
        entry1.timestamp = std::chrono::system_clock::now();
        entry1.success = true;
        entry1.message = "Import successful";
        entry1.imported_objects = {"Object1"};
        
        AssetManager::ImportHistoryEntry entry2;
        entry2.id = "test_020";
        entry2.asset_path = "asset2.fbx";
        entry2.import_type = "link";
        entry2.timestamp = std::chrono::system_clock::now();
        entry2.success = false;
        entry2.message = "Link failed";
        entry2.imported_objects = {};
        
        history.addEntry(entry1);
        history.addEntry(entry2);
        
        auto stats = history.getStats();
        
        return stats.total_imports == 2 && 
               stats.successful_imports == 1 && 
               stats.failed_imports == 1 &&
               stats.imported_assets == 1 &&
               stats.linked_assets == 1;
    });
    
    // Test 16: Get most imported assets
    runner.runTest("Get Most Imported Assets", []() -> bool {
        AssetManager::ImportHistory history;
        
        // Add multiple imports of the same asset
        for (int i = 0; i < 3; ++i) {
            AssetManager::ImportHistoryEntry entry;
            entry.id = "test_" + std::to_string(21 + i);
            entry.asset_path = "frequent_asset.fbx";
            entry.import_type = "import";
            entry.timestamp = std::chrono::system_clock::now();
            entry.success = true;
            entry.message = "Import successful";
            entry.imported_objects = {"Object" + std::to_string(i)};
            
            history.addEntry(entry);
        }
        
        // Add one import of a different asset
        AssetManager::ImportHistoryEntry entry;
        entry.id = "test_024";
        entry.asset_path = "rare_asset.fbx";
        entry.import_type = "import";
        entry.timestamp = std::chrono::system_clock::now();
        entry.success = true;
        entry.message = "Import successful";
        entry.imported_objects = {"RareObject"};
        
        history.addEntry(entry);
        
        auto most_imported = history.getMostImportedAssets(2);
        
        return most_imported.size() >= 1 && most_imported[0] == "frequent_asset.fbx";
    });
    
    // Test 17: Get recently imported assets
    runner.runTest("Get Recently Imported Assets", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry1;
        entry1.id = "test_025";
        entry1.asset_path = "old_asset.fbx";
        entry1.import_type = "import";
        entry1.timestamp = std::chrono::system_clock::now() - std::chrono::hours(1);
        entry1.success = true;
        entry1.message = "Import successful";
        entry1.imported_objects = {"OldObject"};
        
        AssetManager::ImportHistoryEntry entry2;
        entry2.id = "test_026";
        entry2.asset_path = "new_asset.fbx";
        entry2.import_type = "import";
        entry2.timestamp = std::chrono::system_clock::now();
        entry2.success = true;
        entry2.message = "Import successful";
        entry2.imported_objects = {"NewObject"};
        
        history.addEntry(entry1);
        history.addEntry(entry2);
        
        auto recent_assets = history.getRecentlyImportedAssets(2);
        
        return recent_assets.size() >= 1 && recent_assets[0] == "new_asset.fbx";
    });
    
    // Test 18: Get import type distribution
    runner.runTest("Get Import Type Distribution", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry1;
        entry1.id = "test_027";
        entry1.asset_path = "asset1.fbx";
        entry1.import_type = "import";
        entry1.timestamp = std::chrono::system_clock::now();
        entry1.success = true;
        entry1.message = "Import successful";
        entry1.imported_objects = {"Object1"};
        
        AssetManager::ImportHistoryEntry entry2;
        entry2.id = "test_028";
        entry2.asset_path = "asset2.fbx";
        entry2.import_type = "link";
        entry2.timestamp = std::chrono::system_clock::now();
        entry2.success = true;
        entry2.message = "Link successful";
        entry2.imported_objects = {"Object2"};
        
        history.addEntry(entry1);
        history.addEntry(entry2);
        
        auto distribution = history.getImportTypeDistribution();
        
        return distribution["import"] == 1 && distribution["link"] == 1;
    });
    
    // Test 19: Get asset type distribution
    runner.runTest("Get Asset Type Distribution", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry1;
        entry1.id = "test_029";
        entry1.asset_path = "asset1.fbx";
        entry1.import_type = "import";
        entry1.timestamp = std::chrono::system_clock::now();
        entry1.success = true;
        entry1.message = "Import successful";
        entry1.imported_objects = {"Object1"};
        
        AssetManager::ImportHistoryEntry entry2;
        entry2.id = "test_030";
        entry2.asset_path = "asset2.obj";
        entry2.import_type = "import";
        entry2.timestamp = std::chrono::system_clock::now();
        entry2.success = true;
        entry2.message = "Import successful";
        entry2.imported_objects = {"Object2"};
        
        history.addEntry(entry1);
        history.addEntry(entry2);
        
        auto distribution = history.getAssetTypeDistribution();
        
        return distribution[".fbx"] == 1 && distribution[".obj"] == 1;
    });
    
    // Test 20: Entry exists
    runner.runTest("Entry Exists", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry;
        entry.id = "test_031";
        entry.asset_path = "test_asset.fbx";
        entry.import_type = "import";
        entry.timestamp = std::chrono::system_clock::now();
        entry.success = true;
        entry.message = "Import successful";
        entry.imported_objects = {"TestObject"};
        
        history.addEntry(entry);
        
        return history.entryExists("test_031") && !history.entryExists("non_existent");
    });
    
    // Test 21: Get entry
    runner.runTest("Get Entry", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry;
        entry.id = "test_032";
        entry.asset_path = "test_asset.fbx";
        entry.import_type = "import";
        entry.timestamp = std::chrono::system_clock::now();
        entry.success = true;
        entry.message = "Import successful";
        entry.imported_objects = {"TestObject"};
        
        history.addEntry(entry);
        
        auto retrieved_entry = history.getEntry("test_032");
        auto non_existent_entry = history.getEntry("non_existent");
        
        return retrieved_entry && 
               retrieved_entry->id == "test_032" &&
               !non_existent_entry;
    });
    
    // Test 22: Generate entry ID
    runner.runTest("Generate Entry ID", []() -> bool {
        AssetManager::ImportHistory history;
        
        std::string id1 = history.generateEntryId();
        std::string id2 = history.generateEntryId();
        
        return !id1.empty() && !id2.empty() && id1 != id2;
    });
    
    // Test 23: Set max history size
    runner.runTest("Set Max History Size", []() -> bool {
        AssetManager::ImportHistory history;
        
        history.setMaxHistorySize(2);
        
        // Add more entries than the max size
        for (int i = 0; i < 3; ++i) {
            AssetManager::ImportHistoryEntry entry;
            entry.id = "test_" + std::to_string(33 + i);
            entry.asset_path = "asset" + std::to_string(i) + ".fbx";
            entry.import_type = "import";
            entry.timestamp = std::chrono::system_clock::now();
            entry.success = true;
            entry.message = "Import successful";
            entry.imported_objects = {"Object" + std::to_string(i)};
            
            history.addEntry(entry);
        }
        
        return history.getHistorySize() <= 2;
    });
    
    // Test 24: Export history as JSON
    runner.runTest("Export History as JSON", []() -> bool {
        AssetManager::ImportHistory history;
        
        AssetManager::ImportHistoryEntry entry;
        entry.id = "test_035";
        entry.asset_path = "test_asset.fbx";
        entry.import_type = "import";
        entry.timestamp = std::chrono::system_clock::now();
        entry.success = true;
        entry.message = "Import successful";
        entry.imported_objects = {"TestObject"};
        
        history.addEntry(entry);
        
        std::string json = history.exportHistoryAsJSON();
        
        return !json.empty() && 
               json.find("test_035") != std::string::npos &&
               json.find("test_asset.fbx") != std::string::npos;
    });
    
    runner.printSummary();
    
    return runner.getFailedCount() == 0 ? 0 : 1;
} 