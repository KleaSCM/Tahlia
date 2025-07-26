/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * Name: import_history.hpp
 * Description: Header file for the ImportHistory class providing elegant asset import tracking and management.
 *              Supports comprehensive history tracking, undo operations, and state management for imported assets.
 *              Designed for seamless integration with ImportManager and flexible history workflows.
 *
 * Architecture:
 * - Modular history system with persistent state management
 * - Integration with ImportManager for automatic tracking
 * - Configurable history retention and cleanup policies
 * - Thread-safe operations for concurrent import tracking
 * - Extensible design for custom history operations
 *
 * Key Features:
 * - Track imported assets with full metadata and timestamps
 * - Undo operations with proper state restoration
 * - Clear history with selective or complete removal
 * - Export/import history for backup and sharing
 * - Integration with ImportManager for automatic tracking
 * - Detailed history reporting and analytics
 * - Extensible design for new history features
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <filesystem>
#include <optional>
#include <set>

namespace AssetManager {

struct ImportHistoryEntry {
    std::string id;                                    // Unique identifier for this import
    std::string asset_path;                           // Path to the imported asset
    std::string import_type;                          // "import" or "link"
    std::chrono::system_clock::time_point timestamp;  // When the import occurred
    std::map<std::string, std::string> options;       // Import options used
    std::vector<std::string> imported_objects;        // Names of imported/linked objects
    bool success;                                     // Whether the import was successful
    std::string message;                              // Result message
    std::string collection_name;                      // Collection where assets were placed
    std::map<std::string, std::string> metadata;      // Additional metadata
};

struct HistoryStats {
    size_t total_imports;
    size_t successful_imports;
    size_t failed_imports;
    size_t linked_assets;
    size_t imported_assets;
    std::chrono::system_clock::time_point first_import;
    std::chrono::system_clock::time_point last_import;
    std::map<std::string, size_t> import_types;
    std::map<std::string, size_t> asset_types;
};

struct UndoResult {
    bool success;
    std::string message;
    std::vector<std::string> restored_objects;
    std::vector<std::string> removed_objects;
    std::map<std::string, std::string> metadata;
};

class ImportHistory {
public:
    ImportHistory();
    ~ImportHistory();

    // Core history management
    void addEntry(const ImportHistoryEntry& entry);
    std::vector<ImportHistoryEntry> getHistory() const;
    std::vector<ImportHistoryEntry> getHistoryByAsset(const std::string& asset_path) const;
    std::vector<ImportHistoryEntry> getHistoryByType(const std::string& import_type) const;
    std::vector<ImportHistoryEntry> getHistoryByTimeRange(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) const;

    // Undo operations
    UndoResult undoLastImport();
    UndoResult undoImport(const std::string& entry_id);
    UndoResult undoImports(const std::vector<std::string>& entry_ids);
    bool canUndo() const;
    std::vector<std::string> getUndoableEntries() const;

    // Clear operations
    void clearHistory();
    void clearHistoryByAsset(const std::string& asset_path);
    void clearHistoryByType(const std::string& import_type);
    void clearHistoryByTimeRange(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end);
    void clearFailedImports();
    void clearSuccessfulImports();

    // History analysis and reporting
    HistoryStats getStats() const;
    std::vector<std::string> getMostImportedAssets(size_t count = 10) const;
    std::vector<std::string> getRecentlyImportedAssets(size_t count = 10) const;
    std::map<std::string, size_t> getImportTypeDistribution() const;
    std::map<std::string, size_t> getAssetTypeDistribution() const;

    // Persistence and export
    bool saveHistory(const std::string& file_path) const;
    bool loadHistory(const std::string& file_path);
    std::string exportHistoryAsJSON() const;
    bool importHistoryFromJSON(const std::string& json_data);

    // Configuration
    void setMaxHistorySize(size_t max_size);
    void setRetentionPeriod(std::chrono::hours retention_hours);
    void enableAutoCleanup(bool enable);
    void setHistoryFilePath(const std::string& file_path);

    // Utility methods
    std::string generateEntryId() const;
    bool entryExists(const std::string& entry_id) const;
    std::optional<ImportHistoryEntry> getEntry(const std::string& entry_id) const;
    size_t getHistorySize() const;
    bool isEmpty() const;

private:
    std::vector<ImportHistoryEntry> history_;
    size_t max_history_size_;
    std::chrono::hours retention_period_;
    bool auto_cleanup_enabled_;
    std::string history_file_path_;
    
    // Internal helpers
    void cleanupOldEntries();
    void enforceMaxSize();
    std::string generateUniqueId() const;
    bool removeEntryFromBlender(const ImportHistoryEntry& entry);
    std::vector<std::string> getImportedObjectNames(const ImportHistoryEntry& entry) const;
};

} // namespace AssetManager 