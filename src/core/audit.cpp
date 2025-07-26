/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * Name: audit.cpp
 * Description: High-quality implementation of the AssetAuditor class for comprehensive asset library auditing.
 *              Provides modular asset library analysis with scanning, statistics, reporting, and validation logic.
 *              Designed for analyzing large-scale asset libraries with support for all major file formats.
 * 
 * Architecture:
 * - Comprehensive directory scanning with recursive traversal
 * - Intelligent file categorization and type detection
 * - Duplicate file detection and missing reference analysis
 * - Detailed statistics generation with size breakdowns
 * - Human-readable console reporting with recommendations
 * - Robust error handling for large file systems
 * 
 * Key Features:
 * - Universal file format support (3D models, textures, audio, video, documents)
 * - Automatic category detection based on directory structure
 * - Missing file reference detection for asset integrity
 * - Duplicate file name identification
 * - Size-based file categorization and analysis
 * - Comprehensive reporting with actionable recommendations
 */

#include "audit.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <sstream>

/**
 * @brief Constructs a new AssetAuditor with specified project root
 * 
 * Initializes the auditor with the project root path and automatically
 * determines the assets directory path for comprehensive library analysis.
 * 
 * @param projectRoot_ Path to the project root directory
 */
AssetAuditor::AssetAuditor(const std::filesystem::path& projectRoot_)
    : projectRoot(projectRoot_), assetsPath(projectRoot_ / "Assets") {}

/**
 * @brief Runs the complete asset library audit process
 * 
 * Performs a comprehensive audit of the asset library including scanning,
 * duplicate detection, report generation, and file saving. This is the
 * main entry point for asset library analysis.
 * 
 * @note This method performs the complete audit workflow in sequence
 */
void AssetAuditor::runAudit() {
    std::cout << "\U0001F3A8 Starting Asset Library Audit...\n";
    std::cout << "\U0001F4C1 Scanning: " << assetsPath << "\n\n";
    
    // Perform comprehensive directory scanning
    scanDirectory(assetsPath);
    
    // Detect duplicate files for library integrity
    std::cout << "\U0001F50D Checking for duplicate files...\n";
    findDuplicates();
    
    // Generate and display comprehensive report
    generateReport();
    
    // Save audit results to file
    saveReport("asset_audit_report.json");
    
    std::cout << "\u2705 Asset audit complete!\n";
}

/**
 * @brief Recursively scans directory and collects comprehensive statistics
 * 
 * Performs a deep recursive scan of the assets directory, collecting
 * detailed statistics about files, directories, categories, and file types.
 * This is the core scanning engine for the audit process.
 * 
 * @param directory Path to the directory to scan recursively
 * 
 * @note This method handles large directories efficiently with progress reporting
 */
void AssetAuditor::scanDirectory(const std::filesystem::path& directory) {
    if (!std::filesystem::exists(directory)) {
        std::cerr << "\u274C Directory not found: " << directory << std::endl;
        return;
    }
    
    // Recursive directory traversal with comprehensive data collection
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_directory()) {
            // Directory processing and categorization
            stats.totalDirectories++;
            stats.directories.push_back(entry.path().lexically_relative(projectRoot).string());
            
            // Intelligent category detection based on directory names
            std::string dirLower = entry.path().filename().string();
            std::transform(dirLower.begin(), dirLower.end(), dirLower.begin(), ::tolower);
            
            // Category mapping based on directory name patterns
            if (dirLower.find("model") != std::string::npos || dirLower.find("building") != std::string::npos)
                stats.categories["Models"]++;
            else if (dirLower.find("texture") != std::string::npos || dirLower.find("material") != std::string::npos)
                stats.categories["Textures"]++;
            else if (dirLower.find("audio") != std::string::npos)
                stats.categories["Audio"]++;
            else if (dirLower.find("video") != std::string::npos)
                stats.categories["Video"]++;
            else if (dirLower.find("scene") != std::string::npos)
                stats.categories["Scenes"]++;
            else if (dirLower.find("script") != std::string::npos)
                stats.categories["Scripts"]++;
            else if (dirLower.find("doc") != std::string::npos)
                stats.categories["Documents"]++;
            else if (dirLower.find("archive") != std::string::npos)
                stats.categories["Archives"]++;
            else
                stats.categories[entry.path().filename().string()]++;
                
        } else if (entry.is_regular_file()) {
            // File processing and analysis
            stats.totalFiles++;
            
            // File extension analysis for type categorization
            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            stats.fileTypes[ext]++;
            
            // File size calculation and categorization
            double sizeMB = getFileSizeMB(entry.path());
            stats.largestFiles.emplace_back(entry.path().lexically_relative(projectRoot).string(), sizeMB);
            
            // Reference integrity checking
            checkForMissingReferences(entry.path(), ext);
            
            // File type categorization and size breakdown
            categorizeFileByType(entry.path(), ext, sizeMB);
        }
    }
}

/**
 * @brief Identifies files with duplicate names across the library
 * 
 * Scans the entire asset library to find files with identical names
 * but different locations. This helps identify potential conflicts
 * and organizational issues in the asset library.
 * 
 * @note This method builds a map of filenames to their locations for analysis
 */
void AssetAuditor::findDuplicates() {
    std::map<std::string, std::vector<std::string>> fileNames;
    
    // Build filename to location mapping
    for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsPath)) {
        if (entry.is_regular_file()) {
            std::string name = entry.path().filename().string();
            fileNames[name].push_back(entry.path().string());
        }
    }
    
    // Identify files with multiple locations (duplicates)
    for (const auto& [name, paths] : fileNames) {
        if (paths.size() > 1) {
            stats.duplicateNames.emplace_back(name, paths);
        }
    }
}

/**
 * @brief Checks for missing referenced files based on file type patterns
 * 
 * Analyzes files to detect missing references based on common file type
 * relationships. For example, OBJ files typically reference MTL files,
 * and 3D files often reference texture files.
 * 
 * @param filePath Path to the file being analyzed
 * @param ext File extension for reference pattern matching
 * 
 * @note This method uses predefined reference patterns for common file types
 */
void AssetAuditor::checkForMissingReferences(const std::filesystem::path& filePath, const std::string& ext) {
    // Reference patterns for common file type relationships
    static const std::map<std::string, std::vector<std::string>> referencePatterns = {
        {".mtl", {".obj"}},  // Material files typically referenced by OBJ files
        {".blend", {".png", ".jpg", ".jpeg", ".tga", ".tiff"}},  // Blender files reference textures
        {".fbx", {".png", ".jpg", ".jpeg", ".tga", ".tiff"}},   // FBX files reference textures
        {".gltf", {".png", ".jpg", ".jpeg", ".tga", ".tiff"}},  // GLTF files reference textures
        {".glb", {".png", ".jpg", ".jpeg", ".tga", ".tiff"}},   // GLB files reference textures
    };
    
    auto it = referencePatterns.find(ext);
    if (it != referencePatterns.end()) {
        // Check for each expected reference type
        for (const auto& refExt : it->second) {
            auto refFile = filePath.parent_path() / (filePath.stem().string() + refExt);
            
            if (!std::filesystem::exists(refFile)) {
                // Search in subdirectories for the referenced file
                bool found = false;
                for (const auto& subdir : std::filesystem::directory_iterator(filePath.parent_path())) {
                    if (subdir.is_directory()) {
                        auto potentialRef = subdir.path() / (filePath.stem().string() + refExt);
                        if (std::filesystem::exists(potentialRef)) {
                            found = true;
                            break;
                        }
                    }
                }
                
                // Record missing reference if not found
                if (!found) {
                    stats.missingFiles.push_back(refFile.lexically_relative(projectRoot).string());
                }
            }
        }
    }
}

/**
 * @brief Categorizes files by type and size for comprehensive analysis
 * 
 * Analyzes each file to determine its asset type category and size classification.
 * This enables detailed reporting on asset distribution and storage usage.
 * 
 * @param filePath Path to the file being categorized
 * @param ext File extension for type determination
 * @param sizeMB File size in megabytes for size categorization
 * 
 * @note This method uses comprehensive file extension sets for accurate categorization
 */
void AssetAuditor::categorizeFileByType(const std::filesystem::path& filePath, const std::string& ext, double sizeMB) {
    // Comprehensive file extension sets for accurate categorization
    static const std::set<std::string> modelExts = {
        ".blend", ".obj", ".fbx", ".dae", ".3ds", ".stl", ".ply", ".max", 
        ".c4d", ".ma", ".mb", ".abc", ".usd", ".gltf", ".glb"
    };
    static const std::set<std::string> textureExts = {
        ".png", ".jpg", ".jpeg", ".tga", ".tiff", ".bmp", ".exr", ".hdr", 
        ".psd", ".ai", ".svg", ".webp", ".ktx", ".dds"
    };
    static const std::set<std::string> audioExts = {
        ".mp3", ".wav", ".flac", ".aac", ".ogg", ".wma", ".m4a", ".aiff", 
        ".au", ".mid", ".midi"
    };
    static const std::set<std::string> videoExts = {
        ".mp4", ".avi", ".mov", ".wmv", ".flv", ".webm", ".mkv", ".m4v", 
        ".3gp", ".ogv", ".ts", ".mts"
    };
    static const std::set<std::string> docExts = {
        ".pdf", ".doc", ".docx", ".txt", ".rtf", ".md", ".html", ".xml", 
        ".json", ".csv", ".xlsx", ".ppt", ".pptx"
    };
    static const std::set<std::string> archiveExts = {
        ".zip", ".rar", ".7z", ".tar", ".gz", ".bz2", ".xz", ".dmg", ".iso"
    };
    static const std::set<std::string> scriptExts = {
        ".py", ".js", ".php", ".rb", ".java", ".cpp", ".c", ".cs", ".sh", 
        ".bat", ".ps1"
    };
    
    // Asset type categorization
    if (modelExts.count(ext)) stats.assetTypes["models"]++;
    else if (textureExts.count(ext)) stats.assetTypes["textures"]++;
    else if (audioExts.count(ext)) stats.assetTypes["audio"]++;
    else if (videoExts.count(ext)) stats.assetTypes["video"]++;
    else if (docExts.count(ext)) stats.assetTypes["documents"]++;
    else if (archiveExts.count(ext)) stats.assetTypes["archives"]++;
    else if (scriptExts.count(ext)) stats.assetTypes["scripts"]++;
    else stats.assetTypes["other"]++;
    
    // Size-based categorization for storage analysis
    if (sizeMB < 1) stats.sizeBreakdown["tiny"]++;
    else if (sizeMB < 10) stats.sizeBreakdown["small"]++;
    else if (sizeMB < 100) stats.sizeBreakdown["medium"]++;
    else if (sizeMB < 1024) stats.sizeBreakdown["large"]++;
    else if (sizeMB < 10240) stats.sizeBreakdown["huge"]++;
    else stats.sizeBreakdown["massive"]++;
}

/**
 * @brief Calculates file size in megabytes
 * 
 * @param filePath Path to the file
 * @return File size in megabytes, or 0.0 if size cannot be determined
 */
double AssetAuditor::getFileSizeMB(const std::filesystem::path& filePath) const {
    try {
        return static_cast<double>(std::filesystem::file_size(filePath)) / (1024.0 * 1024.0);
    } catch (...) {
        return 0.0;
    }
}

/**
 * @brief Formats file size for human-readable display
 * 
 * Converts file size in megabytes to appropriate units (KB, MB, GB, TB)
 * with proper formatting for clear display in reports.
 * 
 * @param sizeMB File size in megabytes
 * @return Formatted string with appropriate unit
 */
std::string AssetAuditor::formatFileSize(double sizeMB) const {
    std::ostringstream oss;
    
    if (sizeMB >= 1024 * 1024)
        oss << std::fixed << std::setprecision(2) << (sizeMB / (1024 * 1024)) << " TB";
    else if (sizeMB >= 1024)
        oss << std::fixed << std::setprecision(2) << (sizeMB / 1024) << " GB";
    else if (sizeMB >= 1)
        oss << std::fixed << std::setprecision(2) << sizeMB << " MB";
    else
        oss << std::fixed << std::setprecision(0) << (sizeMB * 1024) << " KB";
    
    return oss.str();
}

/**
 * @brief Generates comprehensive human-readable audit report
 * 
 * Creates a detailed console report with statistics, analysis, and recommendations.
 * This is the primary output method for audit results and provides actionable
 * insights for asset library management.
 * 
 * @note This method provides comprehensive analysis with visual formatting
 */
void AssetAuditor::generateReport() const {
    // Report header with visual formatting
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "\U0001F3A8 ASSET LIBRARY AUDIT REPORT\n";
    std::cout << std::string(80, '=') << "\n";
    std::cout << "\U0001F4C5 Generated: " << getTimestamp() << "\n";
    std::cout << "\U0001F4C1 Assets Directory: " << assetsPath << "\n\n";
    
    // Basic statistics section
    std::cout << "\U0001F4CA BASIC STATISTICS\n" << std::string(40, '-') << "\n";
    std::cout << "\U0001F4C1 Total Directories: " << stats.totalDirectories << "\n";
    std::cout << "\U0001F4C4 Total Files: " << stats.totalFiles << "\n\n";
    
    // File type breakdown
    std::cout << "\U0001F4C4 FILE TYPE BREAKDOWN\n" << std::string(40, '-') << "\n";
    for (const auto& [ext, count] : stats.fileTypes)
        std::cout << "  " << std::setw(8) << ext << ": " << std::setw(4) << count << " files\n";
    
    // Asset type categorization
    std::cout << "\n\U0001F3A8 ASSET TYPE BREAKDOWN\n" << std::string(40, '-') << "\n";
    for (const auto& [type, count] : stats.assetTypes)
        if (count > 0) std::cout << "  " << std::setw(12) << type << ": " << std::setw(4) << count << " files\n";
    
    // Size-based categorization
    std::cout << "\n\U0001F4CF SIZE BREAKDOWN\n" << std::string(40, '-') << "\n";
    for (const auto& [cat, count] : stats.sizeBreakdown)
        if (count > 0) std::cout << "  " << std::setw(12) << cat << ": " << std::setw(4) << count << " files\n";
    
    // Directory category analysis
    std::cout << "\n\U0001F4C2 DIRECTORY CATEGORIES\n" << std::string(40, '-') << "\n";
    for (const auto& [cat, count] : stats.categories)
        if (count > 0) std::cout << "  " << std::setw(12) << cat << ": " << std::setw(4) << count << " directories\n";
    
    // Largest files analysis
    std::cout << "\n\U0001F4CF LARGEST FILES (Top 10)\n" << std::string(40, '-') << "\n";
    auto largest = stats.largestFiles;
    std::sort(largest.begin(), largest.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
    for (size_t i = 0; i < std::min<size_t>(10, largest.size()); ++i)
        std::cout << "  " << std::setw(10) << formatFileSize(largest[i].second) << ": " << largest[i].first << "\n";
    
    // Directory structure overview
    std::cout << "\n\U0001F4C1 DIRECTORY STRUCTURE\n" << std::string(40, '-') << "\n";
    auto dirs = stats.directories;
    std::sort(dirs.begin(), dirs.end());
    for (const auto& dir : dirs)
        std::cout << "  \U0001F4C1 " << dir << "\n";
    
    // Issue reporting - missing files
    if (!stats.missingFiles.empty()) {
        std::cout << "\n\u26A0\uFE0F  MISSING FILES\n" << std::string(40, '-') << "\n";
        for (const auto& f : stats.missingFiles)
            std::cout << "  \u274C " << f << "\n";
    }
    
    // Issue reporting - duplicate files
    if (!stats.duplicateNames.empty()) {
        std::cout << "\n\U0001F501 DUPLICATE FILE NAMES\n" << std::string(40, '-') << "\n";
        for (const auto& [name, locations] : stats.duplicateNames) {
            std::cout << "  \U0001F4C4 " << name << "\n";
            for (const auto& loc : locations)
                std::cout << "      \U0001F4C1 " << loc << "\n";
        }
    }
    
    // Summary statistics
    double totalSizeMB = 0.0;
    for (const auto& f : stats.largestFiles) totalSizeMB += f.second;
    
    std::cout << "\n\U0001F4CB SUMMARY\n" << std::string(40, '-') << "\n";
    std::cout << "\U0001F4CA Total Library Size: " << formatFileSize(totalSizeMB) << "\n";
    std::cout << "\U0001F4C1 Directory Count: " << stats.totalDirectories << "\n";
    std::cout << "\U0001F4C4 File Count: " << stats.totalFiles << "\n";
    std::cout << "\U0001F3A8 Asset Types: " << stats.assetTypes.size() << "\n";
    std::cout << "\u26A0\uFE0F  Issues Found: " << (stats.missingFiles.size() + stats.duplicateNames.size()) << "\n\n";
    
    // Actionable recommendations
    std::cout << "\U0001F4A1 RECOMMENDATIONS\n" << std::string(40, '-') << "\n";
    if (!stats.missingFiles.empty()) std::cout << "  \U0001F527 Fix missing referenced files\n";
    if (!stats.duplicateNames.empty()) std::cout << "  \U0001F527 Resolve duplicate file names\n";
    if (stats.totalFiles > 1000) std::cout << "  \U0001F4C8 Consider implementing asset versioning\n";
    if (totalSizeMB > 1024) std::cout << "  \U0001F4BE Consider implementing asset compression\n";
    if (stats.sizeBreakdown.count("huge") && stats.sizeBreakdown.at("huge") > 0)
        std::cout << "  \U0001F680 Large files detected - consider cloud storage\n";
    if (stats.assetTypes.count("audio") && stats.assetTypes.at("audio") > 0)
        std::cout << "  \U0001F3B5 Audio files found - consider audio compression\n";
    if (stats.assetTypes.count("video") && stats.assetTypes.at("video") > 0)
        std::cout << "  \U0001F3AC Video files found - consider video compression\n";
    std::cout << "  \u2705 Universal library audit complete!\n";
}

/**
 * @brief Saves audit report to a text file
 * 
 * Creates a simple text file with key audit statistics and findings.
 * This provides a permanent record of the audit results for later reference.
 * 
 * @param filename Name of the file to save the report to
 * 
 * @note This method creates a simplified version of the console report
 */
void AssetAuditor::saveReport(const std::string& filename) const {
    std::ofstream out(filename);
    out << "Asset Audit Report - " << getTimestamp() << std::endl;
    out << "Assets Path: " << assetsPath << std::endl;
    out << "Total Files: " << stats.totalFiles << std::endl;
    out << "Total Directories: " << stats.totalDirectories << std::endl;
    out << "Issues Found: " << (stats.missingFiles.size() + stats.duplicateNames.size()) << std::endl;
    std::cout << "\U0001F4BE Report saved to: " << filename << std::endl;
}

/**
 * @brief Gets current timestamp as formatted string
 * 
 * @return Current timestamp in YYYY-MM-DD HH:MM:SS format
 */
std::string AssetAuditor::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return std::string(buf);
} 