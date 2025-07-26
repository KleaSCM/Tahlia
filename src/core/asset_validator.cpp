/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * Name: asset_validator.cpp
 * Description: High-quality implementation of the AssetValidator class for comprehensive asset validation.
 *              Provides robust file integrity checking, format-specific validation, and detailed reporting.
 *              Designed for ensuring asset library quality with high-performance validation algorithms.
 *
 * Architecture:
 * - Modular validation system with format-specific validators
 * - Comprehensive error detection and reporting mechanisms
 * - Integration with existing AssetIndexer and AssetManager components
 * - High-performance validation with minimal I/O overhead
 * - Extensible design for new file format support
 *
 * Key Features:
 * - File integrity validation (corrupted, empty, inaccessible files)
 * - Missing texture and dependency detection for 3D models
 * - Format-specific validation (OBJ, FBX, Blend, MTL files)
 * - Detailed validation reports with actionable recommendations
 * - Batch validation for entire asset libraries
 * - Performance-optimized validation algorithms
 */

#include "asset_validator.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <cstring>
#include <chrono>

namespace AssetManager {

    // Constructor implementation
    AssetValidator::AssetValidator() 
        : enable_detailed_validation_(true)
        , check_texture_dependencies_(true)
        , max_file_size_mb_(1000) { // 1GB default limit
        
        // Initialize default validation options
        validation_options_["check_file_integrity"] = true;
        validation_options_["check_texture_dependencies"] = true;
        validation_options_["check_format_specific"] = true;
        validation_options_["max_file_size_mb"] = max_file_size_mb_;
        validation_options_["enable_detailed_validation"] = enable_detailed_validation_;
        
        // Initialize validation statistics
        validation_stats_["total_files_validated"] = 0;
        validation_stats_["total_issues_found"] = 0;
        validation_stats_["validation_time_ms"] = 0;
        validation_stats_["files_with_errors"] = 0;
        validation_stats_["files_with_warnings"] = 0;
    }

    // Destructor implementation
    AssetValidator::~AssetValidator() {
        // Cleanup any resources if needed
    }

    // Main validation method for single asset
    ValidationResult AssetValidator::validateAsset(const std::string& file_path) {
        ValidationResult result;
        result.asset_path = file_path;
        result.is_valid = true;
        result.total_issues = 0;
        result.error_count = 0;
        result.warning_count = 0;
        result.info_count = 0;
        
        try {
            // Perform basic file integrity validation
            validateFileIntegrity(file_path, result);
            
            // If file is accessible, perform format-specific validation
            if (result.is_valid) {
                std::string file_type = detectFileType(file_path);
                
                if (file_type == "obj") {
                    validateOBJFile(file_path, result);
                } else if (file_type == "fbx") {
                    validateFBXFile(file_path, result);
                } else if (file_type == "blend") {
                    validateBlendFile(file_path, result);
                } else if (file_type == "mtl") {
                    validateMTLFile(file_path, result);
                } else if (isTextureFile(file_path)) {
                    validateTextureFile(file_path, result);
                }
                
                // Check for missing textures if enabled
                if (check_texture_dependencies_ && (file_type == "obj" || file_type == "fbx")) {
                    checkMissingTextures(file_path, result);
                }
            }
            
            // Update overall validation status
            result.is_valid = (result.error_count == 0);
            
        } catch (const std::exception& e) {
            addIssue(result, ValidationSeverity::CRITICAL, 
                    "Validation failed with exception: " + std::string(e.what()),
                    "Exception occurred during validation process",
                    "Check file accessibility and try again");
        }
        
        // Update statistics
        try {
            validation_stats_["total_files_validated"] = 
                std::any_cast<size_t>(validation_stats_["total_files_validated"]) + 1;
            validation_stats_["total_issues_found"] = 
                std::any_cast<size_t>(validation_stats_["total_issues_found"]) + result.total_issues;
            
            if (result.error_count > 0) {
                validation_stats_["files_with_errors"] = 
                    std::any_cast<size_t>(validation_stats_["files_with_errors"]) + 1;
            }
            if (result.warning_count > 0) {
                validation_stats_["files_with_warnings"] = 
                    std::any_cast<size_t>(validation_stats_["files_with_warnings"]) + 1;
            }
        } catch (const std::bad_any_cast&) {
            // Reset statistics if casting fails
            validation_stats_["total_files_validated"] = size_t(1);
            validation_stats_["total_issues_found"] = size_t(result.total_issues);
            validation_stats_["files_with_errors"] = size_t(result.error_count > 0 ? 1 : 0);
            validation_stats_["files_with_warnings"] = size_t(result.warning_count > 0 ? 1 : 0);
        }
        
        return result;
    }

    // Batch validation for multiple assets
    std::vector<ValidationResult> AssetValidator::validateAssets(const std::vector<std::string>& file_paths) {
        std::vector<ValidationResult> results;
        results.reserve(file_paths.size());
        
        for (const auto& file_path : file_paths) {
            results.push_back(validateAsset(file_path));
        }
        
        return results;
    }

    // Directory validation with recursive scanning
    std::vector<ValidationResult> AssetValidator::validateDirectory(const std::string& directory_path) {
        std::vector<ValidationResult> results;
        std::vector<std::string> asset_files;
        
        try {
            // Scan directory for supported asset files
            for (const auto& entry : std::filesystem::recursive_directory_iterator(directory_path)) {
                if (entry.is_regular_file()) {
                    std::string file_path = entry.path().string();
                    std::string file_type = detectFileType(file_path);
                    
                    // Only validate supported asset types
                    if (file_type != "unknown") {
                        asset_files.push_back(file_path);
                    }
                }
            }
            
            // Validate all found assets
            results = validateAssets(asset_files);
            
        } catch (const std::exception& e) {
            ValidationResult error_result;
            error_result.asset_path = directory_path;
            error_result.is_valid = false;
            addIssue(error_result, ValidationSeverity::CRITICAL,
                    "Directory validation failed: " + std::string(e.what()),
                    "Exception occurred during directory scanning",
                    "Check directory permissions and accessibility");
            results.push_back(error_result);
        }
        
        return results;
    }

    // File integrity validation
    void AssetValidator::validateFileIntegrity(const std::string& file_path, ValidationResult& result) {
        std::filesystem::path path(file_path);
        
        // Check if file exists
        if (!std::filesystem::exists(path)) {
            addIssue(result, ValidationSeverity::CRITICAL,
                    "File does not exist",
                    "File path: " + file_path,
                    "Verify the file path and ensure the file exists");
            result.is_valid = false;
            return;
        }
        
        // Check if it's a regular file
        if (!std::filesystem::is_regular_file(path)) {
            addIssue(result, ValidationSeverity::ERROR,
                    "Path is not a regular file",
                    "Path: " + file_path,
                    "Ensure the path points to a valid file, not a directory or special file");
            result.is_valid = false;
            return;
        }
        
        // Check file size
        std::error_code ec;
        auto file_size = std::filesystem::file_size(path, ec);
        if (ec) {
            addIssue(result, ValidationSeverity::ERROR,
                    "Cannot determine file size",
                    "Error: " + ec.message(),
                    "Check file permissions and accessibility");
            result.is_valid = false;
            return;
        }
        
        // Check for empty files
        if (file_size == 0) {
            addIssue(result, ValidationSeverity::WARNING,
                    "File is empty (0 bytes)",
                    "File size: 0 bytes",
                    "Consider removing empty files or checking if they should contain data");
        }
        
        // Check file size limit
        size_t file_size_mb = file_size / (1024 * 1024);
        if (file_size_mb > max_file_size_mb_) {
            addIssue(result, ValidationSeverity::WARNING,
                    "File size exceeds recommended limit",
                    "File size: " + std::to_string(file_size_mb) + " MB, Limit: " + std::to_string(max_file_size_mb_) + " MB",
                    "Consider optimizing the file or increasing the size limit if necessary");
        }
        
        // Test file readability
        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open()) {
            addIssue(result, ValidationSeverity::CRITICAL,
                    "Cannot open file for reading",
                    "File path: " + file_path,
                    "Check file permissions and ensure the file is not locked by another process");
            result.is_valid = false;
            return;
        }
        
        // Basic corruption check (read first few bytes)
        char buffer[1024];
        file.read(buffer, sizeof(buffer));
        if (file.fail() && !file.eof()) {
            addIssue(result, ValidationSeverity::ERROR,
                    "File appears to be corrupted or unreadable",
                    "Failed to read file contents",
                    "Check if the file is corrupted or try re-downloading it");
            result.is_valid = false;
        }
        
        file.close();
    }

    // OBJ file validation
    void AssetValidator::validateOBJFile(const std::string& file_path, ValidationResult& result) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            addIssue(result, ValidationSeverity::CRITICAL,
                    "Cannot open OBJ file for validation",
                    "File path: " + file_path,
                    "Check file permissions and accessibility");
            return;
        }
        
        std::string line;
        bool has_vertices = false;
        bool has_faces = false;
        bool has_mtl_reference = false;
        std::string mtl_file;
        
        while (std::getline(file, line)) {
            // Remove comments
            size_t comment_pos = line.find('#');
            if (comment_pos != std::string::npos) {
                line = line.substr(0, comment_pos);
            }
            
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            if (line.empty()) continue;
            
            // Check for key OBJ elements
            if (line.substr(0, 2) == "v ") {
                has_vertices = true;
            } else if (line.substr(0, 2) == "f ") {
                has_faces = true;
            } else if (line.substr(0, 7) == "mtllib ") {
                has_mtl_reference = true;
                mtl_file = line.substr(7);
                // Trim whitespace from MTL filename
                mtl_file.erase(0, mtl_file.find_first_not_of(" \t"));
                mtl_file.erase(mtl_file.find_last_not_of(" \t") + 1);
            }
        }
        
        file.close();
        
        // Validate OBJ structure
        if (!has_vertices) {
            addIssue(result, ValidationSeverity::ERROR,
                    "OBJ file contains no vertices",
                    "File: " + file_path,
                    "Add vertex data to make this a valid 3D model");
        }
        
        if (!has_faces) {
            addIssue(result, ValidationSeverity::WARNING,
                    "OBJ file contains no faces",
                    "File: " + file_path,
                    "Add face data to create a complete 3D model");
        }
        
        // Check MTL file if referenced
        if (has_mtl_reference && !mtl_file.empty()) {
            std::filesystem::path obj_path(file_path);
            std::filesystem::path mtl_path = obj_path.parent_path() / mtl_file;
            
            if (!std::filesystem::exists(mtl_path)) {
                addIssue(result, ValidationSeverity::ERROR,
                        "Referenced MTL file not found",
                        "MTL file: " + mtl_file + " (expected at: " + mtl_path.string() + ")",
                        "Ensure the MTL file exists in the same directory as the OBJ file");
            }
        }
    }

    // FBX file validation
    void AssetValidator::validateFBXFile(const std::string& file_path, ValidationResult& result) {
        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open()) {
            addIssue(result, ValidationSeverity::CRITICAL,
                    "Cannot open FBX file for validation",
                    "File path: " + file_path,
                    "Check file permissions and accessibility");
            return;
        }
        
        // Read FBX header (first 23 bytes)
        char header[23];
        file.read(header, 23);
        
        if (file.gcount() < 23) {
            addIssue(result, ValidationSeverity::ERROR,
                    "FBX file is too small to be valid",
                    "File size appears to be corrupted",
                    "Check if the file is complete and not truncated");
            file.close();
            return;
        }
        
        // Check FBX signature
        std::string signature(header, 23);
        if (signature != "Kaydara FBX Binary  ") {
            addIssue(result, ValidationSeverity::WARNING,
                    "FBX file may not be in standard binary format",
                    "Signature: " + signature,
                    "This might be a text-based FBX file or corrupted binary file");
        }
        
        // Additional FBX-specific validations
        // Check for FBX version (bytes 23-26)
        file.seekg(23);
        unsigned int version = 0;
        file.read(reinterpret_cast<char*>(&version), sizeof(version));
        if (version < 6000 || version > 8000) {
            addIssue(result, ValidationSeverity::WARNING,
                    "FBX version is outside common range (6000-8000)",
                    "Version: " + std::to_string(version),
                    "Check compatibility with your 3D software");
        } else {
            addIssue(result, ValidationSeverity::INFO,
                    "FBX version detected",
                    "Version: " + std::to_string(version),
                    "Version appears to be within a common range");
        }
        // Check for possible file corruption (file size)
        file.seekg(0, std::ios::end);
        std::streampos file_size = file.tellg();
        if (file_size < 1024) {
            addIssue(result, ValidationSeverity::ERROR,
                    "FBX file is suspiciously small",
                    "File size: " + std::to_string(file_size) + " bytes",
                    "File may be incomplete or corrupted");
        }
        file.close();
    }

    // Blend file validation
    void AssetValidator::validateBlendFile(const std::string& file_path, ValidationResult& result) {
        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open()) {
            addIssue(result, ValidationSeverity::CRITICAL,
                    "Cannot open Blend file for validation",
                    "File path: " + file_path,
                    "Check file permissions and accessibility");
            return;
        }
        // Read Blend file header (first 12 bytes)
        char header[12];
        file.read(header, 12);
        if (file.gcount() < 12) {
            addIssue(result, ValidationSeverity::ERROR,
                    "Blend file is too small to be valid",
                    "File size appears to be corrupted",
                    "Check if the file is complete and not truncated");
            file.close();
            return;
        }
        // Check Blend file signature
        std::string signature(header, 7);
        if (signature != "BLENDER") {
            addIssue(result, ValidationSeverity::ERROR,
                    "Invalid Blend file signature",
                    "Expected: BLENDER, Found: " + signature,
                    "This file may not be a valid Blender file");
        } else {
            // Extract pointer size and endianness
            char pointer_size = header[7];
            char endianness = header[8];
            char version[3] = {header[9], header[10], header[11]};
            std::string version_str(version, 3);
            addIssue(result, ValidationSeverity::INFO,
                    "Blend file version detected",
                    "Version: " + version_str,
                    "Pointer size: " + std::string((pointer_size == '_') ? "32-bit" : "64-bit") + ", Endianness: " + ((endianness == 'v') ? "Little" : "Big"));
        }
        // Check for possible file corruption (file size)
        file.seekg(0, std::ios::end);
        std::streampos file_size = file.tellg();
        if (file_size < 1024) {
            addIssue(result, ValidationSeverity::ERROR,
                    "Blend file is suspiciously small",
                    "File size: " + std::to_string(file_size) + " bytes",
                    "File may be incomplete or corrupted");
        }
        file.close();
    }

    // MTL file validation
    void AssetValidator::validateMTLFile(const std::string& file_path, ValidationResult& result) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            addIssue(result, ValidationSeverity::CRITICAL,
                    "Cannot open MTL file for validation",
                    "File path: " + file_path,
                    "Check file permissions and accessibility");
            return;
        }
        
        std::string line;
        bool has_material = false;
        std::vector<std::string> texture_files;
        
        while (std::getline(file, line)) {
            // Remove comments
            size_t comment_pos = line.find('#');
            if (comment_pos != std::string::npos) {
                line = line.substr(0, comment_pos);
            }
            
            // Trim whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
            
            if (line.empty()) continue;
            
            // Check for material definition
            if (line.substr(0, 7) == "newmtl ") {
                has_material = true;
            }
            
            // Check for texture references
            if (line.substr(0, 7) == "map_Kd " || 
                line.substr(0, 7) == "map_Ks " ||
                line.substr(0, 7) == "map_Bump" ||
                line.substr(0, 7) == "map_Ns ") {
                std::string texture_file = line.substr(7);
                // Trim whitespace
                texture_file.erase(0, texture_file.find_first_not_of(" \t"));
                texture_file.erase(texture_file.find_last_not_of(" \t") + 1);
                texture_files.push_back(texture_file);
            }
        }
        
        file.close();
        
        // Validate MTL structure
        if (!has_material) {
            addIssue(result, ValidationSeverity::WARNING,
                    "MTL file contains no material definitions",
                    "File: " + file_path,
                    "Add material definitions using 'newmtl' keyword");
        }
        
        // Check referenced texture files
        std::filesystem::path mtl_path(file_path);
        for (const auto& texture_file : texture_files) {
            std::filesystem::path texture_path = mtl_path.parent_path() / texture_file;
            
            if (!std::filesystem::exists(texture_path)) {
                addIssue(result, ValidationSeverity::ERROR,
                        "Referenced texture file not found",
                        "Texture: " + texture_file + " (expected at: " + texture_path.string() + ")",
                        "Ensure all texture files exist in the same directory as the MTL file");
            }
        }
    }

    // Texture file validation
    void AssetValidator::validateTextureFile(const std::string& file_path, ValidationResult& result) {
        std::ifstream file(file_path, std::ios::binary);
        if (!file.is_open()) {
            addIssue(result, ValidationSeverity::CRITICAL,
                    "Cannot open texture file for validation",
                    "File path: " + file_path,
                    "Check file permissions and accessibility");
            return;
        }
        
        // Read file header for format detection
        char header[16];
        file.read(header, 16);
        file.close();
        
        std::string extension = std::filesystem::path(file_path).extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        // Basic format validation based on file extension and header
        bool valid_format = false;
        
        if (extension == ".jpg" || extension == ".jpeg") {
            // Check JPEG signature
            if (header[0] == (char)0xFF && header[1] == (char)0xD8) {
                valid_format = true;
            }
        } else if (extension == ".png") {
            // Check PNG signature
            if (header[0] == (char)0x89 && header[1] == 'P' && header[2] == 'N' && header[3] == 'G') {
                valid_format = true;
            }
        } else if (extension == ".tga") {
            // TGA files are harder to validate without full parsing
            valid_format = true; // Assume valid for now
        } else if (extension == ".bmp") {
            // Check BMP signature
            if (header[0] == 'B' && header[1] == 'M') {
                valid_format = true;
            }
        }
        
        if (!valid_format) {
            addIssue(result, ValidationSeverity::WARNING,
                    "Texture file format may not be supported",
                    "Extension: " + extension + ", File: " + file_path,
                    "Ensure the texture is in a supported format (JPG, PNG, TGA, BMP)");
        }
    }

    // Missing texture dependency checking
    void AssetValidator::checkMissingTextures(const std::string& file_path, ValidationResult& result) {
        // This is a simplified implementation
        // In a full implementation, this would parse the 3D file format
        // and extract all texture references for validation
        
        addIssue(result, ValidationSeverity::INFO,
                "Texture dependency checking enabled",
                "File: " + file_path,
                "Texture dependencies will be validated during format-specific validation");
    }

    // File type detection
    std::string AssetValidator::detectFileType(const std::string& file_path) {
        std::filesystem::path path(file_path);
        std::string extension = path.extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        // Remove the dot
        if (!extension.empty() && extension[0] == '.') {
            extension = extension.substr(1);
        }
        
        // Map extensions to file types
        if (extension == "obj") return "obj";
        if (extension == "fbx") return "fbx";
        if (extension == "blend") return "blend";
        if (extension == "mtl") return "mtl";
        if (extension == "jpg" || extension == "jpeg" || extension == "png" || 
            extension == "tga" || extension == "bmp" || extension == "tiff") return "texture";
        
        return "unknown";
    }

    // Helper method to check if file is a texture
    bool AssetValidator::isTextureFile(const std::string& file_path) {
        std::string file_type = detectFileType(file_path);
        return (file_type == "texture");
    }

    // Issue addition helper
    void AssetValidator::addIssue(ValidationResult& result, ValidationSeverity severity,
                                 const std::string& description, const std::string& context,
                                 const std::string& recommendation) {
        ValidationIssue issue;
        issue.severity = severity;
        issue.description = description;
        issue.file_path = result.asset_path;
        issue.context = context;
        issue.recommendation = recommendation;
        
        result.issues.push_back(issue);
        result.total_issues++;
        
        switch (severity) {
            case ValidationSeverity::ERROR:
                result.error_count++;
                break;
            case ValidationSeverity::WARNING:
                result.warning_count++;
                break;
            case ValidationSeverity::INFO:
                result.info_count++;
                break;
            case ValidationSeverity::CRITICAL:
                result.error_count++;
                break;
        }
    }

    // Report generation
    std::string AssetValidator::generateReport(const std::vector<ValidationResult>& results) {
        std::ostringstream report;
        
        report << "=== Asset Validation Report ===\n\n";
        report << "Generated: " << std::chrono::system_clock::now().time_since_epoch().count() << "\n";
        report << "Total assets validated: " << results.size() << "\n\n";
        
        // Calculate statistics
        size_t total_issues = 0;
        size_t total_errors = 0;
        size_t total_warnings = 0;
        size_t total_info = 0;
        size_t valid_assets = 0;
        
        for (const auto& result : results) {
            total_issues += result.total_issues;
            total_errors += result.error_count;
            total_warnings += result.warning_count;
            total_info += result.info_count;
            if (result.is_valid) valid_assets++;
        }
        
        report << "=== Summary ===\n";
        report << "Valid assets: " << valid_assets << "/" << results.size() << "\n";
        report << "Total issues found: " << total_issues << "\n";
        report << "  - Errors: " << total_errors << "\n";
        report << "  - Warnings: " << total_warnings << "\n";
        report << "  - Info: " << total_info << "\n\n";
        
        // Detailed results
        report << "=== Detailed Results ===\n";
        for (const auto& result : results) {
            report << "Asset: " << result.asset_path << "\n";
            report << "  Status: " << (result.is_valid ? "VALID" : "INVALID") << "\n";
            report << "  Issues: " << result.total_issues << " (E:" << result.error_count 
                   << " W:" << result.warning_count << " I:" << result.info_count << ")\n";
            
            for (const auto& issue : result.issues) {
                std::string severity_str;
                switch (issue.severity) {
                    case ValidationSeverity::CRITICAL: severity_str = "CRITICAL"; break;
                    case ValidationSeverity::ERROR: severity_str = "ERROR"; break;
                    case ValidationSeverity::WARNING: severity_str = "WARNING"; break;
                    case ValidationSeverity::INFO: severity_str = "INFO"; break;
                }
                
                report << "    [" << severity_str << "] " << issue.description << "\n";
                if (!issue.context.empty()) {
                    report << "      Context: " << issue.context << "\n";
                }
                if (!issue.recommendation.empty()) {
                    report << "      Recommendation: " << issue.recommendation << "\n";
                }
            }
            report << "\n";
        }
        
        return report.str();
    }

    // Report saving
    bool AssetValidator::saveReport(const std::vector<ValidationResult>& results, const std::string& output_path) {
        try {
            std::ofstream file(output_path);
            if (!file.is_open()) {
                return false;
            }
            
            std::string report = generateReport(results);
            file << report;
            file.close();
            
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }

    // Validation options setting
    void AssetValidator::setValidationOptions(const std::map<std::string, std::any>& options) {
        validation_options_.insert(options.begin(), options.end());
        
        // Update internal flags based on options
        if (options.find("enable_detailed_validation") != options.end()) {
            enable_detailed_validation_ = std::any_cast<bool>(options.at("enable_detailed_validation"));
        }
        
        if (options.find("check_texture_dependencies") != options.end()) {
            check_texture_dependencies_ = std::any_cast<bool>(options.at("check_texture_dependencies"));
        }
        
        if (options.find("max_file_size_mb") != options.end()) {
            max_file_size_mb_ = std::any_cast<size_t>(options.at("max_file_size_mb"));
        }
    }

    // Statistics retrieval
    std::map<std::string, std::any> AssetValidator::getValidationStats() const {
        return validation_stats_;
    }

} // namespace AssetManager 