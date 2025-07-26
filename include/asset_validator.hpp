/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * Name: asset_validator.hpp
 * Description: Header file for the AssetValidator class providing comprehensive asset validation capabilities.
 *              Validates file integrity, missing dependencies, format-specific issues, and generates detailed reports.
 *              Designed for ensuring asset library quality and identifying problematic files.
 *
 * Architecture:
 * - Modular validation system with format-specific validators
 * - Comprehensive error detection and reporting
 * - Integration with existing AssetIndexer and AssetManager
 * - High-performance validation with minimal I/O overhead
 * - Extensible design for new file format support
 *
 * Key Features:
 * - File integrity validation (corrupted, empty, inaccessible files)
 * - Missing texture and dependency detection
 * - Format-specific validation (OBJ, FBX, Blend, MTL files)
 * - Detailed validation reports with actionable recommendations
 * - Batch validation for entire asset libraries
 * - Performance-optimized validation algorithms
 */

#ifndef ASSET_VALIDATOR_HPP
#define ASSET_VALIDATOR_HPP

#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <memory>
#include <any>

namespace AssetManager {

    /**
     * @brief Validation severity levels for categorizing issues
     * 
     * Defines the severity of validation issues found during asset analysis.
     * Used for prioritizing fixes and filtering validation reports.
     */
    enum class ValidationSeverity {
        INFO,       ///< Informational message, no action required
        WARNING,    ///< Potential issue that should be reviewed
        ERROR,      ///< Definite problem that needs fixing
        CRITICAL    ///< Critical issue that prevents asset usage
    };

    /**
     * @brief Validation issue structure for detailed reporting
     * 
     * Contains comprehensive information about validation issues found
     * during asset analysis, including severity, description, and context.
     */
    struct ValidationIssue {
        ValidationSeverity severity;    ///< Severity level of the issue
        std::string description;        ///< Human-readable description
        std::string file_path;          ///< Path to the problematic file
        std::string context;            ///< Additional context information
        std::string recommendation;     ///< Suggested fix or action
        std::map<std::string, std::any> metadata; ///< Additional metadata
    };

    /**
     * @brief Validation result structure for comprehensive reporting
     * 
     * Aggregates all validation results for a single asset or batch of assets,
     * providing statistics and detailed issue reporting.
     */
    struct ValidationResult {
        std::string asset_path;                     ///< Path to the validated asset
        bool is_valid;                              ///< Overall validation status
        size_t total_issues;                        ///< Total number of issues found
        size_t error_count;                         ///< Number of errors
        size_t warning_count;                       ///< Number of warnings
        size_t info_count;                          ///< Number of info messages
        std::vector<ValidationIssue> issues;        ///< Detailed list of issues
        std::map<std::string, std::any> metadata;   ///< Additional validation metadata
    };

    /**
     * @brief AssetValidator class for comprehensive asset validation
     * 
     * Provides high-performance validation capabilities for various asset types,
     * including file integrity checks, dependency validation, and format-specific analysis.
     * Integrates seamlessly with existing AssetIndexer and AssetManager components.
     */
    class AssetValidator {
    public:
        /**
         * @brief Default constructor for AssetValidator
         * 
         * Initializes the validator with default settings and prepares
         * format-specific validation engines.
         */
        AssetValidator();

        /**
         * @brief Destructor for cleanup and resource management
         */
        ~AssetValidator();

        /**
         * @brief Validates a single asset file
         * 
         * Performs comprehensive validation on a single asset file, checking
         * file integrity, format-specific requirements, and dependencies.
         * 
         * @param file_path Path to the asset file to validate
         * @return ValidationResult containing detailed validation information
         */
        ValidationResult validateAsset(const std::string& file_path);

        /**
         * @brief Validates multiple assets in batch
         * 
         * Efficiently validates multiple assets, providing progress feedback
         * and comprehensive reporting for the entire batch.
         * 
         * @param file_paths Vector of file paths to validate
         * @return Vector of ValidationResult objects for each asset
         */
        std::vector<ValidationResult> validateAssets(const std::vector<std::string>& file_paths);

        /**
         * @brief Validates an entire directory recursively
         * 
         * Scans and validates all supported assets in a directory tree,
         * providing comprehensive reporting for the entire asset library.
         * 
         * @param directory_path Path to the directory to validate
         * @return Vector of ValidationResult objects for all found assets
         */
        std::vector<ValidationResult> validateDirectory(const std::string& directory_path);

        /**
         * @brief Generates a comprehensive validation report
         * 
         * Creates a detailed human-readable report summarizing validation results,
         * including statistics, issue categorization, and actionable recommendations.
         * 
         * @param results Vector of validation results to report on
         * @return Formatted report string with comprehensive analysis
         */
        std::string generateReport(const std::vector<ValidationResult>& results);

        /**
         * @brief Saves validation report to file
         * 
         * Writes the validation report to a file for persistent storage
         * and sharing with team members or automated systems.
         * 
         * @param results Vector of validation results to report on
         * @param output_path Path where the report should be saved
         * @return True if report was successfully saved, false otherwise
         */
        bool saveReport(const std::vector<ValidationResult>& results, const std::string& output_path);

        /**
         * @brief Sets validation options and preferences
         * 
         * Configures validation behavior, including which checks to perform,
         * severity thresholds, and performance optimizations.
         * 
         * @param options Map of validation options and their values
         */
        void setValidationOptions(const std::map<std::string, std::any>& options);

        /**
         * @brief Gets current validation statistics
         * 
         * Returns comprehensive statistics about validation performance
         * and results for monitoring and optimization purposes.
         * 
         * @return Map containing validation statistics and metrics
         */
        std::map<std::string, std::any> getValidationStats() const;

        /**
         * @brief Determines file type based on extension and content
         * 
         * Analyzes file extension and optionally file content to determine
         * the appropriate validation strategy for the asset.
         * 
         * @param file_path Path to the file to analyze
         * @return String identifier for the detected file type
         */
        std::string detectFileType(const std::string& file_path);

        /**
         * @brief Checks if a file is a texture file
         * 
         * Determines if the given file is a texture based on its extension
         * and file type detection.
         * 
         * @param file_path Path to the file to check
         * @return True if the file is a texture, false otherwise
         */
        bool isTextureFile(const std::string& file_path);

    private:
        /**
         * @brief Validates file integrity and basic properties
         * 
         * Performs fundamental file validation including existence, readability,
         * file size, and basic corruption detection.
         * 
         * @param file_path Path to the file to validate
         * @param result Reference to ValidationResult to populate
         */
        void validateFileIntegrity(const std::string& file_path, ValidationResult& result);

        /**
         * @brief Validates OBJ file format and dependencies
         * 
         * Performs format-specific validation for OBJ files, including
         * MTL file references, texture dependencies, and geometry validation.
         * 
         * @param file_path Path to the OBJ file to validate
         * @param result Reference to ValidationResult to populate
         */
        void validateOBJFile(const std::string& file_path, ValidationResult& result);

        /**
         * @brief Validates FBX file format and structure
         * 
         * Performs format-specific validation for FBX files, including
         * file structure, embedded textures, and animation data validation.
         * 
         * @param file_path Path to the FBX file to validate
         * @param result Reference to ValidationResult to populate
         */
        void validateFBXFile(const std::string& file_path, ValidationResult& result);

        /**
         * @brief Validates Blend file format and Blender compatibility
         * 
         * Performs format-specific validation for Blender files, including
         * version compatibility, embedded assets, and scene structure validation.
         * 
         * @param file_path Path to the Blend file to validate
         * @param result Reference to ValidationResult to populate
         */
        void validateBlendFile(const std::string& file_path, ValidationResult& result);

        /**
         * @brief Validates MTL material file and texture references
         * 
         * Performs format-specific validation for MTL files, including
         * texture file existence, material parameter validation, and dependency checking.
         * 
         * @param file_path Path to the MTL file to validate
         * @param result Reference to ValidationResult to populate
         */
        void validateMTLFile(const std::string& file_path, ValidationResult& result);

        /**
         * @brief Validates texture file format and properties
         * 
         * Performs format-specific validation for texture files, including
         * format support, resolution validation, and file corruption detection.
         * 
         * @param file_path Path to the texture file to validate
         * @param result Reference to ValidationResult to populate
         */
        void validateTextureFile(const std::string& file_path, ValidationResult& result);

        /**
         * @brief Checks for missing texture dependencies
         * 
         * Analyzes asset files for texture references and validates
         * that all referenced texture files exist and are accessible.
         * 
         * @param file_path Path to the asset file to check
         * @param result Reference to ValidationResult to populate
         */
        void checkMissingTextures(const std::string& file_path, ValidationResult& result);

        /**
         * @brief Adds a validation issue to the result
         * 
         * Helper method for adding validation issues with proper formatting
         * and metadata management.
         * 
         * @param result Reference to ValidationResult to modify
         * @param severity Severity level of the issue
         * @param description Human-readable description of the issue
         * @param context Additional context information
         * @param recommendation Suggested fix or action
         */
        void addIssue(ValidationResult& result, ValidationSeverity severity, 
                     const std::string& description, const std::string& context = "",
                     const std::string& recommendation = "");

        // Private member variables for configuration and state management
        std::map<std::string, std::any> validation_options_;  ///< Validation configuration options
        std::map<std::string, std::any> validation_stats_;    ///< Validation performance statistics
        bool enable_detailed_validation_;                     ///< Flag for detailed validation mode
        bool check_texture_dependencies_;                     ///< Flag for texture dependency checking
        size_t max_file_size_mb_;                            ///< Maximum file size for validation
    };

} // namespace AssetManager

#endif // ASSET_VALIDATOR_HPP 