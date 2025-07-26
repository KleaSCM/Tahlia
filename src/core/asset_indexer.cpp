/*
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: asset_indexer.cpp
Description: Implementation of the AssetIndexer class for high-performance asset discovery and caching.
             Features intelligent file system scanning, metadata extraction, and optimized caching.
*/

#include "../../include/asset_indexer.hpp"
#include "../../include/asset_manager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace AssetManager {

AssetIndexer::AssetIndexer() 
    : cache_expiry_duration_(std::chrono::seconds(300)) // 5 minutes default
    , cache_valid_(false) {
    
    initialize_extension_mappings();
    initialize_ignored_patterns();
}

AssetIndexer::~AssetIndexer() = default;

bool AssetIndexer::scan_assets(const std::string& root_path, bool force_refresh) {
    try {
        root_path_ = root_path;
        
        // Check if cache is still valid
        if (!force_refresh && is_cache_valid()) {
            std::cout << "Using cached asset index" << std::endl;
            return true;
        }
        
        std::cout << "Scanning assets in: " << root_path << std::endl;
        
        // Clear existing cache
        clear_cache();
        
        // Only scan the Assets directory specifically
        std::filesystem::path assets_dir = std::filesystem::path(root_path) / "Assets";
        if (!std::filesystem::exists(assets_dir)) {
            std::cout << "Assets directory not found at: " << assets_dir << std::endl;
            std::cout << "Available directories in root:" << std::endl;
            for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::path(root_path))) {
                if (entry.is_directory()) {
                    std::cout << "  - " << entry.path().filename() << std::endl;
                }
            }
            std::cout << "Scanning current directory instead..." << std::endl;
            assets_dir = root_path;
        } else {
            std::cout << "Found Assets directory at: " << assets_dir << std::endl;
        }
        
        std::cout << "Scanning directory: " << assets_dir << std::endl;
        
        size_t files_scanned = 0;
        size_t assets_found = 0;
        auto timer_start = std::chrono::high_resolution_clock::now();
        
        // FAST SCAN: Only look for supported file extensions
        for (const auto& entry : std::filesystem::recursive_directory_iterator(assets_dir)) {
            if (!entry.is_regular_file()) {
                continue;
            }
            
            std::string extension = entry.path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
            
            // Only process supported file types
            if (extension_mappings_.find(extension) == extension_mappings_.end()) {
                continue;
            }
            
            files_scanned++;
            
            // Progress indicator every 1000 files
            if (files_scanned % 1000 == 0) {
                std::cout << "Scanned " << files_scanned << " files, found " << assets_found << " assets..." << std::endl;
            }
            
            // Create minimal asset info (no heavy processing)
            AssetInfo asset_info;
            asset_info.path = std::filesystem::relative(entry.path(), std::filesystem::path(root_path)).string();
            asset_info.name = entry.path().stem().string();
            asset_info.type = extension_mappings_[extension];
            asset_info.category = categorize_asset(entry.path());
            asset_info.file_size = std::filesystem::file_size(entry.path());
            asset_info.last_modified = std::chrono::system_clock::now(); // Skip file time for speed
            asset_info.is_valid = true;
            
            assets_by_path_[asset_info.path] = asset_info;
            update_categorization_maps(asset_info);
            assets_found++;
        }
        
        auto timer_end = std::chrono::high_resolution_clock::now();
        auto scan_duration = std::chrono::duration_cast<std::chrono::milliseconds>(timer_end - timer_start);
        
        last_scan_time_ = std::chrono::system_clock::now();
        cache_valid_ = true;
        
        std::cout << "\nAsset scan complete!" << std::endl;
        std::cout << "Total files scanned: " << files_scanned << std::endl;
        std::cout << "Total assets found: " << assets_found << std::endl;
        std::cout << "Scan duration: " << scan_duration.count() << " ms" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to scan assets: " << e.what() << std::endl;
        return false;
    }
}

std::vector<AssetInfo> AssetIndexer::get_all_assets() const {
    std::vector<AssetInfo> assets;
    assets.reserve(assets_by_path_.size());
    
    for (const auto& [path, asset] : assets_by_path_) {
        assets.push_back(asset);
    }
    
    return assets;
}

std::vector<AssetInfo> AssetIndexer::get_assets_by_category(const std::string& category) const {
    auto it = assets_by_category_.find(category);
    if (it != assets_by_category_.end()) {
        return it->second;
    }
    
    return {};
}

std::vector<AssetInfo> AssetIndexer::get_assets_by_type(const std::string& type) const {
    auto it = assets_by_type_.find(type);
    if (it != assets_by_type_.end()) {
        return it->second;
    }
    
    return {};
}

std::optional<AssetInfo> AssetIndexer::get_asset_by_path(const std::string& path) const {
    auto it = assets_by_path_.find(path);
    if (it != assets_by_path_.end()) {
        return it->second;
    }
    
    return std::nullopt;
}

bool AssetIndexer::is_cache_valid() const {
    if (!cache_valid_) {
        return false;
    }
    
    auto now = std::chrono::system_clock::now();
    auto time_since_scan = now - last_scan_time_;
    
    return time_since_scan < cache_expiry_duration_;
}

void AssetIndexer::clear_cache() {
    assets_by_path_.clear();
    assets_by_category_.clear();
    assets_by_type_.clear();
    cache_valid_ = false;
}

void AssetIndexer::update_asset(const std::string& path) {
    std::filesystem::path file_path(path);
    if (std::filesystem::exists(file_path) && is_supported_format(file_path)) {
        AssetInfo asset_info = create_asset_info(file_path);
        assets_by_path_[asset_info.path] = asset_info;
        update_categorization_maps(asset_info);
    }
}

void AssetIndexer::remove_asset(const std::string& path) {
    remove_from_categorization_maps(path);
    assets_by_path_.erase(path);
}

bool AssetIndexer::save_cache_to_file(const std::string& cache_file_path) const {
    
    try {
        json cache_data;
        cache_data["version"] = "1.0";
        cache_data["scan_time"] = std::chrono::duration_cast<std::chrono::seconds>(
            last_scan_time_.time_since_epoch()).count();
        cache_data["assets"] = json::array();
        
        for (const auto& [path, asset] : assets_by_path_) {
            json asset_json;
            asset_json["path"] = asset.path;
            asset_json["name"] = asset.name;
            asset_json["type"] = asset.type;
            asset_json["category"] = asset.category;
            asset_json["file_size"] = asset.file_size;
            asset_json["last_modified"] = std::chrono::duration_cast<std::chrono::seconds>(
                asset.last_modified.time_since_epoch()).count();
            asset_json["is_valid"] = asset.is_valid;
            asset_json["issues"] = asset.issues;
            asset_json["warnings"] = asset.warnings;
            
            cache_data["assets"].push_back(asset_json);
        }
        
        std::ofstream file(cache_file_path);
        if (file.is_open()) {
            file << cache_data.dump(2);
            file.close();
            return true;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to save cache: " << e.what() << std::endl;
    }
    
    return false;
}

bool AssetIndexer::load_cache_from_file(const std::string& cache_file_path) {
    
    try {
        std::ifstream file(cache_file_path);
        if (!file.is_open()) {
            return false;
        }
        
        json cache_data = json::parse(file);
        
        // Clear existing cache
        clear_cache();
        
        // Load assets
        for (const auto& asset_json : cache_data["assets"]) {
            AssetInfo asset;
            asset.path = asset_json["path"];
            asset.name = asset_json["name"];
            asset.type = asset_json["type"];
            asset.category = asset_json["category"];
            asset.file_size = asset_json["file_size"];
            asset.is_valid = asset_json["is_valid"];
            asset.issues = asset_json["issues"].get<std::vector<std::string>>();
            asset.warnings = asset_json["warnings"].get<std::vector<std::string>>();
            
            // Convert timestamp
            auto timestamp_seconds = asset_json["last_modified"].get<int64_t>();
            asset.last_modified = std::chrono::system_clock::from_time_t(timestamp_seconds);
            
            assets_by_path_[asset.path] = asset;
            update_categorization_maps(asset);
        }
        
        // Set scan time
        auto scan_time_seconds = cache_data["scan_time"].get<int64_t>();
        last_scan_time_ = std::chrono::system_clock::from_time_t(scan_time_seconds);
        cache_valid_ = true;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to load cache: " << e.what() << std::endl;
        return false;
    }
}

std::string AssetIndexer::categorize_asset(const std::filesystem::path& file_path) const {
    std::string filename = file_path.filename().string();
    std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
    
    // Check for common category keywords in filename
    if (filename.find("building") != std::string::npos || 
        filename.find("house") != std::string::npos ||
        filename.find("skyscraper") != std::string::npos) {
        return "Buildings";
    }
    
    if (filename.find("character") != std::string::npos || 
        filename.find("person") != std::string::npos ||
        filename.find("human") != std::string::npos) {
        return "Characters";
    }
    
    if (filename.find("prop") != std::string::npos || 
        filename.find("object") != std::string::npos ||
        filename.find("item") != std::string::npos) {
        return "Props";
    }
    
    if (filename.find("tree") != std::string::npos || 
        filename.find("plant") != std::string::npos ||
        filename.find("nature") != std::string::npos) {
        return "Environment";
    }
    
    if (filename.find("vehicle") != std::string::npos || 
        filename.find("car") != std::string::npos ||
        filename.find("truck") != std::string::npos) {
        return "Vehicles";
    }
    
    // Check directory structure
    auto relative_path = file_path.lexically_relative(root_path_);
    if (!relative_path.empty()) {
        std::string first_dir = relative_path.begin()->string();
        if (first_dir == "Models") {
            if (relative_path.string().find("Buildings") != std::string::npos) return "Buildings";
            if (relative_path.string().find("Characters") != std::string::npos) return "Characters";
            if (relative_path.string().find("Props") != std::string::npos) return "Props";
            if (relative_path.string().find("Environment") != std::string::npos) return "Environment";
            if (relative_path.string().find("Vehicles") != std::string::npos) return "Vehicles";
        }
    }
    
    return "Misc";
}

std::string AssetIndexer::determine_asset_type(const std::filesystem::path& file_path) const {
    std::string extension = file_path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    auto it = extension_mappings_.find(extension);
    if (it != extension_mappings_.end()) {
        return it->second;
    }
    
    return "Unknown";
}

bool AssetIndexer::is_supported_format(const std::filesystem::path& file_path) const {
    std::string extension = file_path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return extension_mappings_.find(extension) != extension_mappings_.end();
}

void AssetIndexer::set_cache_expiry_duration(std::chrono::seconds duration) {
    cache_expiry_duration_ = duration;
}

std::chrono::seconds AssetIndexer::get_cache_expiry_duration() const {
    return cache_expiry_duration_;
}

size_t AssetIndexer::get_cache_size() const {
    return assets_by_path_.size();
}

void AssetIndexer::initialize_extension_mappings() {
    extension_mappings_.clear();
    
    // 3D Models
    extension_mappings_[".blend"] = "Blend";
    extension_mappings_[".obj"] = "OBJ";
    extension_mappings_[".fbx"] = "FBX";
    extension_mappings_[".dae"] = "Collada";
    extension_mappings_[".3ds"] = "3DS";
    extension_mappings_[".stl"] = "STL";
    extension_mappings_[".ply"] = "PLY";
    
    // Textures
    extension_mappings_[".png"] = "Texture";
    extension_mappings_[".jpg"] = "Texture";
    extension_mappings_[".jpeg"] = "Texture";
    extension_mappings_[".tga"] = "Texture";
    extension_mappings_[".tiff"] = "Texture";
    extension_mappings_[".bmp"] = "Texture";
    extension_mappings_[".exr"] = "Texture";
    extension_mappings_[".hdr"] = "Texture";
    
    // Audio
    extension_mappings_[".mp3"] = "Audio";
    extension_mappings_[".wav"] = "Audio";
    extension_mappings_[".flac"] = "Audio";
    extension_mappings_[".aac"] = "Audio";
    extension_mappings_[".ogg"] = "Audio";
    
    // Video
    extension_mappings_[".mp4"] = "Video";
    extension_mappings_[".avi"] = "Video";
    extension_mappings_[".mov"] = "Video";
    extension_mappings_[".wmv"] = "Video";
    extension_mappings_[".flv"] = "Video";
    extension_mappings_[".webm"] = "Video";
    extension_mappings_[".mkv"] = "Video";
}

void AssetIndexer::initialize_ignored_patterns() {
    ignored_patterns_.clear();
    
    // Common files to ignore
    ignored_patterns_.push_back(".*\\.DS_Store$");
    ignored_patterns_.push_back(".*\\.Thumbs\\.db$");
    ignored_patterns_.push_back(".*\\.desktop\\.ini$");
    ignored_patterns_.push_back(".*\\.tmp$");
    ignored_patterns_.push_back(".*\\.temp$");
    ignored_patterns_.push_back(".*\\.bak$");
    ignored_patterns_.push_back(".*\\.backup$");
    ignored_patterns_.push_back(".*~$");
    
    // Hidden files and directories
    ignored_patterns_.push_back(".*/\\.git/.*");
    ignored_patterns_.push_back(".*/\\.svn/.*");
    ignored_patterns_.push_back(".*/\\.hg/.*");
    ignored_patterns_.push_back(".*/\\.bzr/.*");
}

AssetInfo AssetIndexer::create_asset_info(const std::filesystem::path& file_path) const {
    AssetInfo asset;
    
    asset.path = std::filesystem::relative(file_path, root_path_).string();
    asset.name = file_path.stem().string();
    asset.type = determine_asset_type(file_path);
    asset.category = categorize_asset(file_path);
    asset.file_size = get_file_size(file_path);
    asset.last_modified = get_file_modification_time(file_path);
    asset.metadata = extract_metadata(file_path);
    asset.dependencies = find_dependencies(file_path);
    asset.is_valid = true;
    
    // Basic validation
    if (asset.file_size == 0) {
        asset.is_valid = false;
        asset.issues.push_back("File is empty");
    }
    
    if (asset.type == "Unknown") {
        asset.warnings.push_back("Unsupported file format");
    }
    
    return asset;
}

size_t AssetIndexer::get_file_size(const std::filesystem::path& file_path) const {
    try {
        return std::filesystem::file_size(file_path);
    } catch (const std::exception& e) {
        std::cerr << "Failed to get file size for " << file_path << ": " << e.what() << std::endl;
        return 0;
    }
}

std::chrono::system_clock::time_point AssetIndexer::get_file_modification_time(const std::filesystem::path& file_path) const {
    try {
        auto ftime = std::filesystem::last_write_time(file_path);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
        return sctp;
    } catch (const std::exception& e) {
        std::cerr << "Failed to get modification time for " << file_path << ": " << e.what() << std::endl;
        return std::chrono::system_clock::now();
    }
}

std::map<std::string, std::any> AssetIndexer::extract_metadata(const std::filesystem::path& file_path) const {
    std::map<std::string, std::any> metadata;
    
    std::string extension = file_path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".obj") {
        metadata = extract_obj_metadata(file_path);
    } else if (extension == ".fbx") {
        metadata = extract_fbx_metadata(file_path);
    } else if (extension == ".blend") {
        metadata = extract_blend_metadata(file_path);
    }
    
    return metadata;
}

std::vector<std::string> AssetIndexer::find_dependencies(const std::filesystem::path& file_path) const {
    std::vector<std::string> dependencies;
    
    std::string extension = file_path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".obj") {
        dependencies = find_obj_dependencies(file_path);
    }
    
    return dependencies;
}

bool AssetIndexer::should_ignore_file(const std::filesystem::path& file_path) const {
    std::string path_str = file_path.string();
    
    for (const auto& pattern : ignored_patterns_) {
        try {
            std::regex regex_pattern(pattern);
            if (std::regex_match(path_str, regex_pattern)) {
                return true;
            }
        } catch (const std::regex_error& e) {
            std::cerr << "Invalid regex pattern: " << pattern << " - " << e.what() << std::endl;
        }
    }
    
    return false;
}

void AssetIndexer::update_categorization_maps(const AssetInfo& asset_info) {
    // Update category map
    assets_by_category_[asset_info.category].push_back(asset_info);
    
    // Update type map
    assets_by_type_[asset_info.type].push_back(asset_info);
}

void AssetIndexer::remove_from_categorization_maps(const std::string& path) {
    // Find and remove from category map
    for (auto& [category, assets] : assets_by_category_) {
        assets.erase(
            std::remove_if(assets.begin(), assets.end(),
                [&path](const AssetInfo& asset) { return asset.path == path; }),
            assets.end()
        );
    }
    
    // Find and remove from type map
    for (auto& [type, assets] : assets_by_type_) {
        assets.erase(
            std::remove_if(assets.begin(), assets.end(),
                [&path](const AssetInfo& asset) { return asset.path == path; }),
            assets.end()
        );
    }
}

std::map<std::string, std::any> AssetIndexer::extract_obj_metadata(const std::filesystem::path& file_path) const {
    std::map<std::string, std::any> metadata;
    
    try {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            return metadata;
        }
        
        std::string line;
        int vertex_count = 0;
        int face_count = 0;
        int material_count = 0;
        
        while (std::getline(file, line)) {
            if (line.substr(0, 2) == "v ") {
                vertex_count++;
            } else if (line.substr(0, 2) == "f ") {
                face_count++;
            } else if (line.substr(0, 7) == "usemtl ") {
                material_count++;
            }
        }
        
        metadata["vertex_count"] = vertex_count;
        metadata["face_count"] = face_count;
        metadata["material_count"] = material_count;
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to extract OBJ metadata: " << e.what() << std::endl;
    }
    
    return metadata;
}

std::map<std::string, std::any> AssetIndexer::extract_fbx_metadata(const std::filesystem::path& file_path) const {
    (void)file_path; // Suppress unused parameter warning
    std::map<std::string, std::any> metadata;
    
    // FBX metadata extraction would require FBX SDK
    // For now, return basic info
    metadata["format"] = "FBX";
    metadata["note"] = "Detailed metadata requires FBX SDK";
    
    return metadata;
}

std::map<std::string, std::any> AssetIndexer::extract_blend_metadata(const std::filesystem::path& file_path) const {
    (void)file_path; // Suppress unused parameter warning
    std::map<std::string, std::any> metadata;
    
    // Blend file metadata extraction would require Blender Python API
    // For now, return basic info
    metadata["format"] = "Blend";
    metadata["note"] = "Detailed metadata requires Blender Python API";
    
    return metadata;
}

std::vector<std::string> AssetIndexer::find_obj_dependencies(const std::filesystem::path& file_path) const {
    std::vector<std::string> dependencies;
    
    try {
        // Check for MTL file
        auto mtl_path = file_path;
        mtl_path.replace_extension(".mtl");
        if (std::filesystem::exists(mtl_path)) {
            dependencies.push_back(std::filesystem::relative(mtl_path, root_path_).string());
        }
        
        // Check for texture files referenced in MTL
        if (std::filesystem::exists(mtl_path)) {
            std::ifstream mtl_file(mtl_path);
            std::string line;
            
            while (std::getline(mtl_file, line)) {
                if (line.substr(0, 7) == "map_Kd " || line.substr(0, 8) == "map_Bump ") {
                    std::string texture_path = line.substr(line.find_last_of(' ') + 1);
                    auto full_texture_path = file_path.parent_path() / texture_path;
                    
                    if (std::filesystem::exists(full_texture_path)) {
                        dependencies.push_back(std::filesystem::relative(full_texture_path, root_path_).string());
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Failed to find OBJ dependencies: " << e.what() << std::endl;
    }
    
    return dependencies;
}

std::vector<std::string> AssetIndexer::find_material_dependencies(const std::filesystem::path& file_path) const {
    (void)file_path; // Suppress unused parameter warning
    std::vector<std::string> dependencies;
    
    // This would scan for texture references in material files
    // Implementation depends on specific material format
    
    return dependencies;
}

} // namespace AssetManager 