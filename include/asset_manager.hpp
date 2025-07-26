/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * Name: asset_manager.hpp
 * Description: Header file for the AssetManager class providing comprehensive asset management capabilities.
 *              Features high-performance asset discovery, validation, importing, and material management.
 *              Designed for managing diverse asset libraries with advanced import options and PBR material support.
 *
 * Architecture:
 * - Modular asset management system with specialized subsystems
 * - High-performance asset discovery and indexing with intelligent caching
 * - Advanced material management with PBR texture mapping support
 * - Flexible import system with ImportManager for single and bulk operations
 * - Comprehensive validation and error reporting mechanisms
 * - Extensible design for new asset types and import handlers
 *
 * Key Features:
 * - High-performance asset discovery and indexing with configurable scanning
 * - Advanced material management with PBR texture mapping and presets
 * - Flexible import system with location, rotation, scale, and collection options
 * - Bulk asset importing with pattern matching and automatic spacing
 * - Comprehensive asset validation with detailed issue reporting
 * - Advanced search capabilities with multiple filter criteria
 * - Import history tracking and management
 * - Collection management for organized asset grouping
 * - JSON-based communication for Python FFI integration
 * - Configurable cache management with automatic invalidation
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <any>
#include <chrono>
#include <filesystem>
#include <optional>
#include <functional>
#include "import_manager.hpp"
#include "material_manager.hpp"
#include "import_history.hpp"

namespace AssetManager {

// Forward declarations
class AssetIndexer;
class AssetValidator;
class AssetSearcher;
class MaterialManager;
class CollectionManager;

// Core data structures
struct AssetInfo {
    std::string path;
    std::string name;
    std::string type;
    std::string category;
    size_t file_size;
    std::chrono::system_clock::time_point last_modified;
    std::map<std::string, std::any> metadata;
    std::vector<std::string> dependencies;
    bool is_valid;
    std::vector<std::string> issues;
    std::vector<std::string> warnings;
};

struct SearchFilters {
    std::string search_term;
    std::string asset_type;
    std::string category;
    size_t min_file_size = 0;
    size_t max_file_size = SIZE_MAX;
    std::chrono::system_clock::time_point modified_after;
    std::chrono::system_clock::time_point modified_before;
};

// MaterialPreset is now defined in material_manager.hpp
// ImportHistoryEntry is now defined in import_history.hpp

// Main AssetManager class
class AssetManager {
public:
    AssetManager();
    ~AssetManager();
    
    // Initialization and configuration
    bool initialize(const std::string& assets_root_path = "");
    void set_assets_root(const std::string& path);
    std::string get_assets_root() const;
    
    // Asset discovery and indexing
    bool scan_assets(bool force_refresh = false);
    std::vector<AssetInfo> get_all_assets() const;
    std::vector<AssetInfo> get_assets_by_type(const std::string& type) const;
    std::vector<AssetInfo> get_assets_by_category(const std::string& category) const;
    std::optional<AssetInfo> get_asset_by_path(const std::string& path) const;
    
    // Asset validation
    bool validate_asset(const std::string& asset_path);
    AssetInfo get_asset_info(const std::string& asset_path) const;
    std::vector<std::string> get_asset_issues(const std::string& asset_path) const;
    
    // Asset search
    std::vector<AssetInfo> search_assets(const SearchFilters& filters) const;
    std::vector<AssetInfo> search_by_name(const std::string& search_term) const;
    std::vector<AssetInfo> search_by_pattern(const std::string& pattern) const;
    
    // Material management
    // Material creation methods are now handled by MaterialManager
    
    // Collection management
    std::string create_collection(const std::string& name, const std::vector<std::string>& asset_paths = {});
    std::string add_to_collection(const std::string& collection_name, const std::vector<std::string>& asset_paths);
    
    // Asset statistics and information
    std::string get_asset_stats() const;
    std::string get_asset_library_info() const;
    size_t get_total_asset_count() const;
    
    // Import history
    std::vector<ImportHistoryEntry> get_import_history() const;
    void clear_import_history();
    std::string get_import_history_json() const;
    
    // Cache management
    void clear_cache();
    void refresh_cache();
    bool is_cache_valid() const;
    
    // Utility functions
    std::string get_supported_formats() const;
    // Material presets are now handled by MaterialManager
    bool is_asset_supported(const std::string& asset_path) const;
    
    // ImportManager integration
    std::shared_ptr<ImportManager> getImportManager() const;
    ImportResult importAsset(const std::string& asset_path, const ImportOptions& options = {});
    std::vector<ImportResult> importAssetsGrid(const std::vector<std::string>& asset_paths, const ImportOptions& options = {}, int rows = 1, int cols = 1, float spacing = 5.0f);
    std::vector<ImportResult> importAssetsCircle(const std::vector<std::string>& asset_paths, const ImportOptions& options = {}, float radius = 10.0f);
    std::vector<ImportResult> importAssetsLine(const std::vector<std::string>& asset_paths, const ImportOptions& options = {}, float spacing = 5.0f);
    std::vector<ImportResult> importAssetsRandom(const std::vector<std::string>& asset_paths, const ImportOptions& options = {}, int count = 10, float area_size = 20.0f);
    
private:
    std::unique_ptr<AssetIndexer> indexer_;
    std::unique_ptr<ImportManager> import_manager_;
    std::unique_ptr<MaterialManager> material_manager_;
    // TODO: Add other subsystems when implemented (DONE)
    // std::unique_ptr<AssetValidator> validator_;
    // std::unique_ptr<AssetSearcher> searcher_;
    // std::unique_ptr<MaterialManager> material_manager_;
    // std::unique_ptr<CollectionManager> collection_manager_;
    
    // Subsystem member variables for future implementation
    // std::unique_ptr<AssetValidator> validator_;      // Asset validation subsystem
    // std::unique_ptr<AssetSearcher> searcher_;        // Advanced search subsystem
    // std::unique_ptr<MaterialManager> material_manager_; // Material management subsystem
    // std::unique_ptr<CollectionManager> collection_manager_; // Collection management subsystem
    
    std::string assets_root_path_;
    std::vector<ImportHistoryEntry> import_history_;
    // Material presets are now handled by MaterialManager
    std::map<std::string, std::string> import_handlers_;
    std::map<std::string, std::vector<std::string>> pbr_texture_mappings_;
    
    bool initialized_;
    std::chrono::system_clock::time_point last_cache_update_;
    
    // Private helper methods
    // Material presets are now handled by MaterialManager
    void initialize_import_handlers();
    void initialize_pbr_mappings();
    std::string serialize_to_json(const std::any& data) const;
    std::any deserialize_from_json(const std::string& json) const;
};

} // namespace AssetManager 