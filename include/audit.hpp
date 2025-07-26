/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * Name: audit.hpp
 * Description: Header file for the AssetAuditor class providing comprehensive asset library auditing capabilities.
 *              Features detailed asset analysis, statistics collection, issue detection, and report generation.
 *              Designed for ensuring asset library quality and identifying problematic files and dependencies.
 *
 * Architecture:
 * - Modular auditing system with comprehensive file analysis
 * - Intelligent statistics collection with categorized breakdowns
 * - Duplicate detection and missing file reference checking
 * - Format-specific validation and dependency analysis
 * - Detailed reporting with actionable recommendations
 * - Extensible design for new audit criteria and file types
 *
 * Key Features:
 * - Comprehensive asset library scanning and analysis
 * - Detailed statistics collection (file types, sizes, categories)
 * - Duplicate file detection with location tracking
 * - Missing file reference detection and reporting
 * - File size analysis with largest file identification
 * - Category and asset type breakdown and statistics
 * - Human-readable report generation with formatting
 * - Configurable audit criteria and filtering options
 * - High-performance scanning with minimal I/O overhead
 * - Timestamp-based audit tracking and history
 */

#ifndef AUDIT_HPP
#define AUDIT_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include <filesystem>
// JSON reporting removed - console output only

// AssetAuditor: Modular class for asset library auditing
// Scans directories, collects statistics, detects issues, and generates reports
class AssetAuditor {
public:
    // Constructor: Initializes auditor with project root
    explicit AssetAuditor(const std::filesystem::path& projectRoot);

    // Run the complete audit (scan, analyze, report, save)
    void runAudit();

    // Generate a human-readable report to stdout
    void generateReport() const;

    // Save audit report as JSON
    void saveReport(const std::string& filename) const;

private:
    std::filesystem::path projectRoot;
    std::filesystem::path assetsPath;

    // Statistics and results
    struct Stats {
        size_t totalFiles = 0;
        size_t totalDirectories = 0;
        std::map<std::string, size_t> fileTypes;
        std::vector<std::string> directories;
        std::vector<std::pair<std::string, double>> largestFiles; // path, size MB
        std::vector<std::string> missingFiles;
        std::vector<std::pair<std::string, std::vector<std::string>>> duplicateNames; // name, locations
        std::map<std::string, size_t> categories;
        std::map<std::string, size_t> assetTypes;
        std::map<std::string, size_t> sizeBreakdown;
    } stats;

    // Internal helpers
    void scanDirectory(const std::filesystem::path& directory);
    void findDuplicates();
    void checkForMissingReferences(const std::filesystem::path& filePath, const std::string& ext);
    void categorizeFileByType(const std::filesystem::path& filePath, const std::string& ext, double sizeMB);
    double getFileSizeMB(const std::filesystem::path& filePath) const;
    std::string formatFileSize(double sizeMB) const;

    // Utility: Get current timestamp as string
    std::string getTimestamp() const;
};

#endif // AUDIT_HPP 