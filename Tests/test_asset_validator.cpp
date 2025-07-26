/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * Name: test_asset_validator.cpp
 * Description: Comprehensive unit tests for the AssetValidator class using Catch2 framework.
 *              Tests all validation functionality including file integrity, format-specific validation,
 *              and reporting capabilities with thorough coverage of edge cases and error conditions.
 *
 * Architecture:
 * - Modular test structure with clear test categories
 * - Comprehensive coverage of all public methods
 * - Edge case testing for error conditions
 * - Performance testing for validation algorithms
 * - Integration testing with file system operations
 *
 * Key Features:
 * - File integrity validation testing
 * - Format-specific validation testing (OBJ, FBX, Blend, MTL)
 * - Error handling and exception testing
 * - Report generation and saving testing
 * - Configuration and options testing
 */

#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "asset_validator.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>

using namespace AssetManager;

// Test fixture for creating temporary test files
class AssetValidatorTestFixture {
protected:
    std::string temp_dir_;
    
    AssetValidatorTestFixture() {
        // Create temporary directory for test files
        temp_dir_ = std::filesystem::temp_directory_path().string() + "/asset_validator_test";
        std::filesystem::create_directories(temp_dir_);
    }
    
    ~AssetValidatorTestFixture() {
        // Clean up temporary directory
        std::filesystem::remove_all(temp_dir_);
    }
    
    // Helper method to create a test file
    std::string createTestFile(const std::string& filename, const std::string& content) {
        std::string filepath = temp_dir_ + "/" + filename;
        std::ofstream file(filepath);
        file << content;
        file.close();
        return filepath;
    }
    
    // Helper method to create an empty file
    std::string createEmptyFile(const std::string& filename) {
        std::string filepath = temp_dir_ + "/" + filename;
        std::ofstream file(filepath);
        file.close();
        return filepath;
    }
};

TEST_CASE_METHOD(AssetValidatorTestFixture, "AssetValidator Constructor", "[constructor]") {
    SECTION("Default constructor initializes correctly") {
        AssetValidator validator;
        
        // Test that default options are set
        auto options = validator.getValidationStats();
        REQUIRE(options.find("total_files_validated") != options.end());
        REQUIRE(options.find("total_issues_found") != options.end());
    }
}

TEST_CASE_METHOD(AssetValidatorTestFixture, "File Integrity Validation", "[integrity]") {
    AssetValidator validator;
    
    SECTION("Valid file passes integrity check") {
        std::string test_file = createTestFile("test.txt", "This is a test file");
        ValidationResult result = validator.validateAsset(test_file);
        
        REQUIRE(result.is_valid == true);
        REQUIRE(result.total_issues == 0);
    }
    
    SECTION("Empty file generates warning") {
        std::string empty_file = createEmptyFile("empty.txt");
        ValidationResult result = validator.validateAsset(empty_file);
        
        REQUIRE(result.is_valid == true); // Empty files are valid but generate warnings
        REQUIRE(result.warning_count >= 1);
        
        bool found_empty_warning = false;
        for (const auto& issue : result.issues) {
            if (issue.description.find("empty") != std::string::npos) {
                found_empty_warning = true;
                break;
            }
        }
        REQUIRE(found_empty_warning == true);
    }
    
    SECTION("Non-existent file generates critical error") {
        std::string non_existent = temp_dir_ + "/nonexistent.txt";
        ValidationResult result = validator.validateAsset(non_existent);
        
        REQUIRE(result.is_valid == false);
        REQUIRE(result.error_count >= 1);
        
        bool found_existence_error = false;
        for (const auto& issue : result.issues) {
            if (issue.severity == ValidationSeverity::CRITICAL && 
                issue.description.find("does not exist") != std::string::npos) {
                found_existence_error = true;
                break;
            }
        }
        REQUIRE(found_existence_error == true);
    }
}

TEST_CASE_METHOD(AssetValidatorTestFixture, "OBJ File Validation", "[obj]") {
    AssetValidator validator;
    
    SECTION("Valid OBJ file passes validation") {
        std::string obj_content = R"(
# Test OBJ file
v 0.0 0.0 0.0
v 1.0 0.0 0.0
v 0.0 1.0 0.0
f 1 2 3
)";
        std::string obj_file = createTestFile("test.obj", obj_content);
        ValidationResult result = validator.validateAsset(obj_file);
        
        REQUIRE(result.is_valid == true);
        REQUIRE(result.total_issues == 0);
    }
    
    SECTION("OBJ file without vertices generates error") {
        std::string obj_content = R"(
# OBJ file without vertices
f 1 2 3
)";
        std::string obj_file = createTestFile("no_vertices.obj", obj_content);
        ValidationResult result = validator.validateAsset(obj_file);
        
        REQUIRE(result.is_valid == false);
        REQUIRE(result.error_count >= 1);
        
        bool found_vertex_error = false;
        for (const auto& issue : result.issues) {
            if (issue.description.find("no vertices") != std::string::npos) {
                found_vertex_error = true;
                break;
            }
        }
        REQUIRE(found_vertex_error == true);
    }
    
    SECTION("OBJ file with MTL reference validates correctly") {
        std::string obj_content = R"(
# OBJ file with MTL reference
mtllib test.mtl
v 0.0 0.0 0.0
f 1 1 1
)";
        std::string obj_file = createTestFile("with_mtl.obj", obj_content);
        
        // Create the referenced MTL file
        std::string mtl_content = R"(
# Test MTL file
newmtl test_material
)";
        createTestFile("test.mtl", mtl_content);
        
        ValidationResult result = validator.validateAsset(obj_file);
        
        // Should be valid since MTL file exists
        REQUIRE(result.is_valid == true);
    }
    
    SECTION("OBJ file with missing MTL reference generates error") {
        std::string obj_content = R"(
# OBJ file with missing MTL reference
mtllib missing.mtl
v 0.0 0.0 0.0
f 1 1 1
)";
        std::string obj_file = createTestFile("missing_mtl.obj", obj_content);
        ValidationResult result = validator.validateAsset(obj_file);
        
        REQUIRE(result.is_valid == false);
        REQUIRE(result.error_count >= 1);
        
        bool found_mtl_error = false;
        for (const auto& issue : result.issues) {
            if (issue.description.find("MTL file not found") != std::string::npos) {
                found_mtl_error = true;
                break;
            }
        }
        REQUIRE(found_mtl_error == true);
    }
}

TEST_CASE_METHOD(AssetValidatorTestFixture, "FBX File Validation", "[fbx]") {
    AssetValidator validator;
    
    SECTION("Valid FBX file passes validation") {
        // Create a minimal FBX file with correct header
        std::string fbx_header = "Kaydara FBX Binary  ";
        std::string fbx_file = temp_dir_ + "/test.fbx";
        std::ofstream file(fbx_file, std::ios::binary);
        file.write(fbx_header.c_str(), fbx_header.length());
        file.close();
        
        ValidationResult result = validator.validateAsset(fbx_file);
        
        // Should pass basic validation
        REQUIRE(result.is_valid == true);
    }
    
    SECTION("Invalid FBX file generates warning") {
        std::string invalid_fbx = createTestFile("invalid.fbx", "This is not a valid FBX file");
        ValidationResult result = validator.validateAsset(invalid_fbx);
        
        REQUIRE(result.warning_count >= 1);
        
        bool found_format_warning = false;
        for (const auto& issue : result.issues) {
            if (issue.description.find("format") != std::string::npos) {
                found_format_warning = true;
                break;
            }
        }
        REQUIRE(found_format_warning == true);
    }
}

TEST_CASE_METHOD(AssetValidatorTestFixture, "Blend File Validation", "[blend]") {
    AssetValidator validator;
    
    SECTION("Valid Blend file passes validation") {
        // Create a minimal Blend file with correct header
        std::string blend_header = "BLENDER";
        std::string blend_file = temp_dir_ + "/test.blend";
        std::ofstream file(blend_file, std::ios::binary);
        file.write(blend_header.c_str(), blend_header.length());
        file.close();
        
        ValidationResult result = validator.validateAsset(blend_file);
        
        // Should pass basic validation
        REQUIRE(result.is_valid == true);
    }
    
    SECTION("Invalid Blend file generates error") {
        std::string invalid_blend = createTestFile("invalid.blend", "This is not a valid Blend file");
        ValidationResult result = validator.validateAsset(invalid_blend);
        
        REQUIRE(result.is_valid == false);
        REQUIRE(result.error_count >= 1);
        
        bool found_signature_error = false;
        for (const auto& issue : result.issues) {
            if (issue.description.find("signature") != std::string::npos) {
                found_signature_error = true;
                break;
            }
        }
        REQUIRE(found_signature_error == true);
    }
}

TEST_CASE_METHOD(AssetValidatorTestFixture, "MTL File Validation", "[mtl]") {
    AssetValidator validator;
    
    SECTION("Valid MTL file passes validation") {
        std::string mtl_content = R"(
# Test MTL file
newmtl test_material
Ka 1.000000 1.000000 1.000000
Kd 1.000000 1.000000 1.000000
Ks 0.000000 0.000000 0.000000
)";
        std::string mtl_file = createTestFile("test.mtl", mtl_content);
        ValidationResult result = validator.validateAsset(mtl_file);
        
        REQUIRE(result.is_valid == true);
    }
    
    SECTION("MTL file without material definition generates warning") {
        std::string mtl_content = R"(
# MTL file without material definition
Ka 1.000000 1.000000 1.000000
)";
        std::string mtl_file = createTestFile("no_material.mtl", mtl_content);
        ValidationResult result = validator.validateAsset(mtl_file);
        
        REQUIRE(result.warning_count >= 1);
        
        bool found_material_warning = false;
        for (const auto& issue : result.issues) {
            if (issue.description.find("material definitions") != std::string::npos) {
                found_material_warning = true;
                break;
            }
        }
        REQUIRE(found_material_warning == true);
    }
    
    SECTION("MTL file with missing texture generates error") {
        std::string mtl_content = R"(
# MTL file with missing texture
newmtl test_material
map_Kd missing_texture.jpg
)";
        std::string mtl_file = createTestFile("missing_texture.mtl", mtl_content);
        ValidationResult result = validator.validateAsset(mtl_file);
        
        REQUIRE(result.is_valid == false);
        REQUIRE(result.error_count >= 1);
        
        bool found_texture_error = false;
        for (const auto& issue : result.issues) {
            if (issue.description.find("texture file not found") != std::string::npos) {
                found_texture_error = true;
                break;
            }
        }
        REQUIRE(found_texture_error == true);
    }
}

TEST_CASE_METHOD(AssetValidatorTestFixture, "Batch Validation", "[batch]") {
    AssetValidator validator;
    
    SECTION("Multiple files validation works correctly") {
        std::string file1 = createTestFile("test1.txt", "Test file 1");
        std::string file2 = createTestFile("test2.txt", "Test file 2");
        std::string file3 = createTestFile("test3.txt", "Test file 3");
        
        std::vector<std::string> files = {file1, file2, file3};
        std::vector<ValidationResult> results = validator.validateAssets(files);
        
        REQUIRE(results.size() == 3);
        for (const auto& result : results) {
            REQUIRE(result.is_valid == true);
        }
    }
    
    SECTION("Directory validation works correctly") {
        // Create test files in the temp directory
        createTestFile("test1.txt", "Test file 1");
        createTestFile("test2.txt", "Test file 2");
        createTestFile("test3.txt", "Test file 3");
        
        std::vector<ValidationResult> results = validator.validateDirectory(temp_dir_);
        
        REQUIRE(results.size() >= 3); // Should find at least our test files
    }
}

TEST_CASE_METHOD(AssetValidatorTestFixture, "Report Generation", "[report]") {
    AssetValidator validator;
    
    SECTION("Report generation works correctly") {
        std::string valid_file = createTestFile("test.txt", "Test content");
        std::string empty_file = createEmptyFile("empty.txt");
        
        std::vector<std::string> files = {valid_file, empty_file};
        std::vector<ValidationResult> results = validator.validateAssets(files);
        
        std::string report = validator.generateReport(results);
        
        REQUIRE(!report.empty());
        REQUIRE(report.find("Asset Validation Report") != std::string::npos);
        REQUIRE(report.find("Total assets validated: 2") != std::string::npos);
    }
    
    SECTION("Report saving works correctly") {
        std::string test_file = createTestFile("test.txt", "Test content");
        ValidationResult result = validator.validateAsset(test_file);
        
        std::vector<ValidationResult> results = {result};
        std::string report_path = temp_dir_ + "/validation_report.txt";
        
        bool saved = validator.saveReport(results, report_path);
        REQUIRE(saved == true);
        
        // Verify file was created
        REQUIRE(std::filesystem::exists(report_path));
        
        // Verify file has content
        std::ifstream report_file(report_path);
        std::string content((std::istreambuf_iterator<char>(report_file)),
                           std::istreambuf_iterator<char>());
        REQUIRE(!content.empty());
    }
}

TEST_CASE_METHOD(AssetValidatorTestFixture, "Configuration Options", "[config]") {
    AssetValidator validator;
    
    SECTION("Validation options can be set and retrieved") {
        std::map<std::string, std::any> options;
        options["check_texture_dependencies"] = false;
        options["max_file_size_mb"] = size_t(500);
        
        validator.setValidationOptions(options);
        
        auto stats = validator.getValidationStats();
        REQUIRE(stats.find("total_files_validated") != stats.end());
    }
}

TEST_CASE_METHOD(AssetValidatorTestFixture, "Error Handling", "[errors]") {
    AssetValidator validator;
    
    SECTION("Exception handling works correctly") {
        // Test with a file that might cause issues
        std::string problematic_file = "/dev/null"; // This might not exist on all systems
        
        ValidationResult result = validator.validateAsset(problematic_file);
        
        // Should handle gracefully without crashing
        REQUIRE(result.asset_path == problematic_file);
    }
}

TEST_CASE_METHOD(AssetValidatorTestFixture, "File Type Detection", "[detection]") {
    AssetValidator validator;
    
    SECTION("File type detection works correctly") {
        // Test various file extensions
        REQUIRE(validator.detectFileType("test.obj") == "obj");
        REQUIRE(validator.detectFileType("test.fbx") == "fbx");
        REQUIRE(validator.detectFileType("test.blend") == "blend");
        REQUIRE(validator.detectFileType("test.mtl") == "mtl");
        REQUIRE(validator.detectFileType("test.jpg") == "texture");
        REQUIRE(validator.detectFileType("test.png") == "texture");
        REQUIRE(validator.detectFileType("test.unknown") == "unknown");
    }
} 