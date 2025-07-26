/*
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: test_audit.cpp
Description: Production-level unit tests for the AssetAuditor class. Verifies directory scanning, statistics, and report generation for Blender Asset Manager.
*/

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include "audit.hpp"
#include <filesystem>
#include <fstream>

TEST_CASE("AssetAuditor basic audit runs without error", "[audit]") {
    // Setup: Use the project root as the base path
    std::filesystem::path projectRoot = std::filesystem::current_path();
    AssetAuditor auditor(projectRoot);
    REQUIRE_NOTHROW(auditor.runAudit());
}

TEST_CASE("AssetAuditor generates JSON report", "[audit]") {
    std::filesystem::path projectRoot = std::filesystem::current_path();
    AssetAuditor auditor(projectRoot);
    auditor.runAudit();
    std::ifstream report("asset_audit_report.json");
    REQUIRE(report.good());
    // Optionally, check for expected JSON keys
    std::string content((std::istreambuf_iterator<char>(report)), std::istreambuf_iterator<char>());
    REQUIRE(content.find("statistics") != std::string::npos);
    REQUIRE(content.find("assets_path") != std::string::npos);
} 