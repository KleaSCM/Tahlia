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
 * - Flexible import system with configurable options and bulk operations
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

namespace AssetManager {

// Forward declarations
class AssetIndexer;
class AssetValidator;
class AssetSearcher;
class MaterialManager;
class ImportManager;
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

struct ImportOptions {
    std::tuple<float, float, float> location = {0.0f, 0.0f, 0.0f};
    std::tuple<float, float, float> rotation = {0.0f, 0.0f, 0.0f};
    std::tuple<float, float, float> scale = {1.0f, 1.0f, 1.0f};
    bool import_materials = true;
    bool import_animations = true;
    bool import_cameras = false;
    bool import_lights = false;
    bool merge_objects = false;
    bool auto_smooth = true;
    std::string collection_name;
    bool link_instead_of_import = false;
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

struct MaterialPreset {
    std::string name;
    float metallic = 0.0f;
    float roughness = 0.5f;
    std::tuple<float, float, float, float> base_color = {0.8f, 0.8f, 0.8f, 1.0f};
    float transmission = 0.0f;
    float ior = 1.45f;
    std::map<std::string, std::any> additional_properties;
};

struct ImportHistoryEntry {
    std::string asset_path;
    std::vector<std::string> imported_objects;
    ImportOptions options;
    std::chrono::system_clock::time_point timestamp;
};

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
    
    // Asset importing (returns JSON string for Python FFI)
    std::string import_asset(const std::string& asset_path, const ImportOptions& options = {});
    std::string import_assets_bulk(const std::vector<std::string>& asset_paths, const ImportOptions& options = {});
    std::string import_assets_in_pattern(const std::vector<std::string>& asset_paths, const std::string& pattern, float spacing = 5.0f);
    
    // Material management
    std::string create_material(const std::string& name, const std::string& material_type = "pbr");
    std::string create_material_with_texture(const std::string& name, const std::string& texture_path, const std::map<std::string, std::any>& properties = {});
    std::string create_pbr_material(const std::string& name, const std::map<std::string, std::string>& texture_paths);
    std::string quick_material_setup(const std::string& material_type, const std::string& name);
    
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
    std::string get_material_presets() const;
    bool is_asset_supported(const std::string& asset_path) const;
    
private:
    std::unique_ptr<AssetIndexer> indexer_;
    // TODO: Add other subsystems when implemented
    // std::unique_ptr<AssetValidator> validator_;
    // std::unique_ptr<AssetSearcher> searcher_;
    // std::unique_ptr<MaterialManager> material_manager_;
    // std::unique_ptr<ImportManager> import_manager_;
    // std::unique_ptr<CollectionManager> collection_manager_;
    
    std::string assets_root_path_;
    std::vector<ImportHistoryEntry> import_history_;
    std::map<std::string, MaterialPreset> material_presets_;
    std::map<std::string, std::string> import_handlers_;
    std::map<std::string, std::vector<std::string>> pbr_texture_mappings_;
    
    bool initialized_;
    std::chrono::system_clock::time_point last_cache_update_;
    
    // Private helper methods
    void initialize_material_presets();
    void initialize_import_handlers();
    void initialize_pbr_mappings();
    std::string serialize_to_json(const std::any& data) const;
    std::any deserialize_from_json(const std::string& json) const;
};

} // namespace AssetManager 