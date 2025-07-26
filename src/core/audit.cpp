/*
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: audit.cpp
Description: Implementation of the AssetAuditor class. Provides modular, production-level asset library auditing for Blender Asset Manager. Implements scanning, statistics, reporting, and validation logic.
*/

#include "audit.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <sstream>

// JSON removed - console output only

// Constructor: Initializes auditor with project root and assets path
AssetAuditor::AssetAuditor(const std::filesystem::path& projectRoot_)
    : projectRoot(projectRoot_), assetsPath(projectRoot_ / "Assets") {}

// Run the complete audit: scan, find duplicates, report, save
void AssetAuditor::runAudit() {
    std::cout << "\U0001F3A8 Starting Asset Library Audit...\n";
    std::cout << "\U0001F4C1 Scanning: " << assetsPath << "\n\n";
    scanDirectory(assetsPath);
    std::cout << "\U0001F50D Checking for duplicate files...\n";
    findDuplicates();
    generateReport();
    saveReport("asset_audit_report.json");
    std::cout << "\u2705 Asset audit complete!\n";
}

// Recursively scan directory and collect statistics
void AssetAuditor::scanDirectory(const std::filesystem::path& directory) {
    if (!std::filesystem::exists(directory)) {
        std::cerr << "\u274C Directory not found: " << directory << std::endl;
        return;
    }
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_directory()) {
            stats.totalDirectories++;
            stats.directories.push_back(entry.path().lexically_relative(projectRoot).string());
            // Category detection (simplified for brevity)
            std::string dirLower = entry.path().filename().string();
            std::transform(dirLower.begin(), dirLower.end(), dirLower.begin(), ::tolower);
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
            stats.totalFiles++;
            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            stats.fileTypes[ext]++;
            double sizeMB = getFileSizeMB(entry.path());
            stats.largestFiles.emplace_back(entry.path().lexically_relative(projectRoot).string(), sizeMB);
            checkForMissingReferences(entry.path(), ext);
            categorizeFileByType(entry.path(), ext, sizeMB);
        }
    }
}

// Find files with duplicate names
void AssetAuditor::findDuplicates() {
    std::map<std::string, std::vector<std::string>> fileNames;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(assetsPath)) {
        if (entry.is_regular_file()) {
            std::string name = entry.path().filename().string();
            fileNames[name].push_back(entry.path().string());
        }
    }
    for (const auto& [name, paths] : fileNames) {
        if (paths.size() > 1) {
            stats.duplicateNames.emplace_back(name, paths);
        }
    }
}

// Check for missing referenced files (simplified universal logic)
void AssetAuditor::checkForMissingReferences(const std::filesystem::path& filePath, const std::string& ext) {
    static const std::map<std::string, std::vector<std::string>> referencePatterns = {
        {".mtl", {".obj"}},
        {".blend", {".png", ".jpg", ".jpeg", ".tga", ".tiff"}},
        {".fbx", {".png", ".jpg", ".jpeg", ".tga", ".tiff"}},
        {".gltf", {".png", ".jpg", ".jpeg", ".tga", ".tiff"}},
        {".glb", {".png", ".jpg", ".jpeg", ".tga", ".tiff"}},
    };
    auto it = referencePatterns.find(ext);
    if (it != referencePatterns.end()) {
        for (const auto& refExt : it->second) {
            auto refFile = filePath.parent_path() / (filePath.stem().string() + refExt);
            if (!std::filesystem::exists(refFile)) {
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
                if (!found) {
                    stats.missingFiles.push_back(refFile.lexically_relative(projectRoot).string());
                }
            }
        }
    }
}

// Categorize file by type and size
void AssetAuditor::categorizeFileByType(const std::filesystem::path& filePath, const std::string& ext, double sizeMB) {
    static const std::set<std::string> modelExts = {".blend", ".obj", ".fbx", ".dae", ".3ds", ".stl", ".ply", ".max", ".c4d", ".ma", ".mb", ".abc", ".usd", ".gltf", ".glb"};
    static const std::set<std::string> textureExts = {".png", ".jpg", ".jpeg", ".tga", ".tiff", ".bmp", ".exr", ".hdr", ".psd", ".ai", ".svg", ".webp", ".ktx", ".dds"};
    static const std::set<std::string> audioExts = {".mp3", ".wav", ".flac", ".aac", ".ogg", ".wma", ".m4a", ".aiff", ".au", ".mid", ".midi"};
    static const std::set<std::string> videoExts = {".mp4", ".avi", ".mov", ".wmv", ".flv", ".webm", ".mkv", ".m4v", ".3gp", ".ogv", ".ts", ".mts"};
    static const std::set<std::string> docExts = {".pdf", ".doc", ".docx", ".txt", ".rtf", ".md", ".html", ".xml", ".json", ".csv", ".xlsx", ".ppt", ".pptx"};
    static const std::set<std::string> archiveExts = {".zip", ".rar", ".7z", ".tar", ".gz", ".bz2", ".xz", ".dmg", ".iso"};
    static const std::set<std::string> scriptExts = {".py", ".js", ".php", ".rb", ".java", ".cpp", ".c", ".cs", ".sh", ".bat", ".ps1"};
    if (modelExts.count(ext)) stats.assetTypes["models"]++;
    else if (textureExts.count(ext)) stats.assetTypes["textures"]++;
    else if (audioExts.count(ext)) stats.assetTypes["audio"]++;
    else if (videoExts.count(ext)) stats.assetTypes["video"]++;
    else if (docExts.count(ext)) stats.assetTypes["documents"]++;
    else if (archiveExts.count(ext)) stats.assetTypes["archives"]++;
    else if (scriptExts.count(ext)) stats.assetTypes["scripts"]++;
    else stats.assetTypes["other"]++;
    if (sizeMB < 1) stats.sizeBreakdown["tiny"]++;
    else if (sizeMB < 10) stats.sizeBreakdown["small"]++;
    else if (sizeMB < 100) stats.sizeBreakdown["medium"]++;
    else if (sizeMB < 1024) stats.sizeBreakdown["large"]++;
    else if (sizeMB < 10240) stats.sizeBreakdown["huge"]++;
    else stats.sizeBreakdown["massive"]++;
}

// Get file size in MB
double AssetAuditor::getFileSizeMB(const std::filesystem::path& filePath) const {
    try {
        return static_cast<double>(std::filesystem::file_size(filePath)) / (1024.0 * 1024.0);
    } catch (...) {
        return 0.0;
    }
}

// Format file size for display
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

// Generate a human-readable report to stdout
void AssetAuditor::generateReport() const {
    std::cout << "\n" << std::string(80, '=') << "\n";
    std::cout << "\U0001F3A8 ASSET LIBRARY AUDIT REPORT\n";
    std::cout << std::string(80, '=') << "\n";
    std::cout << "\U0001F4C5 Generated: " << getTimestamp() << "\n";
    std::cout << "\U0001F4C1 Assets Directory: " << assetsPath << "\n\n";
    std::cout << "\U0001F4CA BASIC STATISTICS\n" << std::string(40, '-') << "\n";
    std::cout << "\U0001F4C1 Total Directories: " << stats.totalDirectories << "\n";
    std::cout << "\U0001F4C4 Total Files: " << stats.totalFiles << "\n\n";
    std::cout << "\U0001F4C4 FILE TYPE BREAKDOWN\n" << std::string(40, '-') << "\n";
    for (const auto& [ext, count] : stats.fileTypes)
        std::cout << "  " << std::setw(8) << ext << ": " << std::setw(4) << count << " files\n";
    std::cout << "\n\U0001F3A8 ASSET TYPE BREAKDOWN\n" << std::string(40, '-') << "\n";
    for (const auto& [type, count] : stats.assetTypes)
        if (count > 0) std::cout << "  " << std::setw(12) << type << ": " << std::setw(4) << count << " files\n";
    std::cout << "\n\U0001F4CF SIZE BREAKDOWN\n" << std::string(40, '-') << "\n";
    for (const auto& [cat, count] : stats.sizeBreakdown)
        if (count > 0) std::cout << "  " << std::setw(12) << cat << ": " << std::setw(4) << count << " files\n";
    std::cout << "\n\U0001F4C2 DIRECTORY CATEGORIES\n" << std::string(40, '-') << "\n";
    for (const auto& [cat, count] : stats.categories)
        if (count > 0) std::cout << "  " << std::setw(12) << cat << ": " << std::setw(4) << count << " directories\n";
    std::cout << "\n\U0001F4CF LARGEST FILES (Top 10)\n" << std::string(40, '-') << "\n";
    auto largest = stats.largestFiles;
    std::sort(largest.begin(), largest.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
    for (size_t i = 0; i < std::min<size_t>(10, largest.size()); ++i)
        std::cout << "  " << std::setw(10) << formatFileSize(largest[i].second) << ": " << largest[i].first << "\n";
    std::cout << "\n\U0001F4C1 DIRECTORY STRUCTURE\n" << std::string(40, '-') << "\n";
    auto dirs = stats.directories;
    std::sort(dirs.begin(), dirs.end());
    for (const auto& dir : dirs)
        std::cout << "  \U0001F4C1 " << dir << "\n";
    if (!stats.missingFiles.empty()) {
        std::cout << "\n\u26A0\uFE0F  MISSING FILES\n" << std::string(40, '-') << "\n";
        for (const auto& f : stats.missingFiles)
            std::cout << "  \u274C " << f << "\n";
    }
    if (!stats.duplicateNames.empty()) {
        std::cout << "\n\U0001F501 DUPLICATE FILE NAMES\n" << std::string(40, '-') << "\n";
        for (const auto& [name, locations] : stats.duplicateNames) {
            std::cout << "  \U0001F4C4 " << name << "\n";
            for (const auto& loc : locations)
                std::cout << "      \U0001F4C1 " << loc << "\n";
        }
    }
    double totalSizeMB = 0.0;
    for (const auto& f : stats.largestFiles) totalSizeMB += f.second;
    std::cout << "\n\U0001F4CB SUMMARY\n" << std::string(40, '-') << "\n";
    std::cout << "\U0001F4CA Total Library Size: " << formatFileSize(totalSizeMB) << "\n";
    std::cout << "\U0001F4C1 Directory Count: " << stats.totalDirectories << "\n";
    std::cout << "\U0001F4C4 File Count: " << stats.totalFiles << "\n";
    std::cout << "\U0001F3A8 Asset Types: " << stats.assetTypes.size() << "\n";
    std::cout << "\u26A0\uFE0F  Issues Found: " << (stats.missingFiles.size() + stats.duplicateNames.size()) << "\n\n";
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

// Save audit report as simple text file
void AssetAuditor::saveReport(const std::string& filename) const {
    std::ofstream out(filename);
    out << "Asset Audit Report - " << getTimestamp() << std::endl;
    out << "Assets Path: " << assetsPath << std::endl;
    out << "Total Files: " << stats.totalFiles << std::endl;
    out << "Total Directories: " << stats.totalDirectories << std::endl;
    out << "Issues Found: " << (stats.missingFiles.size() + stats.duplicateNames.size()) << std::endl;
    std::cout << "\U0001F4BE Report saved to: " << filename << std::endl;
}

// Utility: Get current timestamp as string
std::string AssetAuditor::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return std::string(buf);
} 