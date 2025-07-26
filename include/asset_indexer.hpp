/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * Name: asset_indexer.hpp
 * Description: Header file for the AssetIndexer class providing high-performance asset discovery and caching capabilities.
 *              Features intelligent file system scanning, metadata extraction, and optimized caching for large asset libraries.
 *              Designed for managing diverse asset types across multiple file formats with minimal I/O overhead.
 *
 * Architecture:
 * - Modular indexing system with format-specific metadata extractors
 * - Intelligent caching with configurable expiry and lazy loading
 * - Multi-threaded file system scanning with pattern-based filtering
 * - Hierarchical categorization by type, category, and metadata
 * - Dependency tracking and validation for complex asset relationships
 * - Extensible design for new file format support
 *
 * Key Features:
 * - High-performance asset discovery with configurable scanning patterns
 * - Intelligent caching with automatic invalidation and refresh
 * - Format-specific metadata extraction (OBJ, FBX, Blend, MTL files)
 * - Dependency tracking for textures, materials, and linked assets
 * - Multi-criteria asset categorization and filtering
 * - Thread-safe operations with mutex-protected cache access
 * - Configurable file type mappings and ignored patterns
 * - Comprehensive asset information with modification tracking
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <filesystem>
#include <optional>
#include <mutex>
#include <any>

namespace AssetManager {

struct AssetInfo;

class AssetIndexer {
public:
    AssetIndexer();
    ~AssetIndexer();
    
    // Core indexing functionality
    bool scan_assets(const std::string& root_path, bool force_refresh = false);
    std::vector<AssetInfo> get_all_assets() const;
    std::vector<AssetInfo> get_assets_by_category(const std::string& category) const;
    std::vector<AssetInfo> get_assets_by_type(const std::string& type) const;
    std::optional<AssetInfo> get_asset_by_path(const std::string& path) const;
    
    // Cache management
    bool is_cache_valid() const;
    void clear_cache();
    void update_asset(const std::string& path);
    void remove_asset(const std::string& path);
    bool save_cache_to_file(const std::string& cache_file_path) const;
    bool load_cache_from_file(const std::string& cache_file_path);
    
    // Asset categorization
    std::string categorize_asset(const std::filesystem::path& file_path) const;
    std::string determine_asset_type(const std::filesystem::path& file_path) const;
    bool is_supported_format(const std::filesystem::path& file_path) const;
    
    // Performance optimization
    void set_cache_expiry_duration(std::chrono::seconds duration);
    std::chrono::seconds get_cache_expiry_duration() const;
    size_t get_cache_size() const;
    
private:
    // Asset storage
    std::map<std::string, AssetInfo> assets_by_path_;
    std::map<std::string, std::vector<AssetInfo>> assets_by_category_;
    std::map<std::string, std::vector<AssetInfo>> assets_by_type_;
    
    // Cache management
    std::string cache_file_path_;
    std::chrono::system_clock::time_point last_scan_time_;
    std::chrono::seconds cache_expiry_duration_;
    bool cache_valid_;
    
    // File system scanning
    std::string root_path_;
    std::vector<std::string> ignored_patterns_;
    std::map<std::string, std::string> extension_mappings_;
    
    // Thread safety
    mutable std::mutex cache_mutex_;
    
    // Private helper methods
    void initialize_extension_mappings();
    void initialize_ignored_patterns();
    AssetInfo create_asset_info(const std::filesystem::path& file_path) const;
    size_t get_file_size(const std::filesystem::path& file_path) const;
    std::chrono::system_clock::time_point get_file_modification_time(const std::filesystem::path& file_path) const;
    std::map<std::string, std::any> extract_metadata(const std::filesystem::path& file_path) const;
    std::vector<std::string> find_dependencies(const std::filesystem::path& file_path) const;
    bool should_ignore_file(const std::filesystem::path& file_path) const;
    void update_categorization_maps(const AssetInfo& asset_info);
    void remove_from_categorization_maps(const std::string& path);
    
    // File format specific helpers
    std::map<std::string, std::any> extract_obj_metadata(const std::filesystem::path& file_path) const;
    std::map<std::string, std::any> extract_fbx_metadata(const std::filesystem::path& file_path) const;
    std::map<std::string, std::any> extract_blend_metadata(const std::filesystem::path& file_path) const;
    std::vector<std::string> find_obj_dependencies(const std::filesystem::path& file_path) const;
    std::vector<std::string> find_material_dependencies(const std::filesystem::path& file_path) const;
};

} // namespace AssetManager 