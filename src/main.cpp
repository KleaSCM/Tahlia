/*
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: main.cpp
Description: Main entry point for the Blender Asset Manager C++ core application.
             Demonstrates the asset management system capabilities with fast scanning.
             Also includes asset auditing functionality for comprehensive library analysis.
*/

#include "../include/asset_manager.hpp"
#include "../include/audit.hpp"
#include <iostream>
#include <chrono>
#include <string>

int main(int argc, char* argv[]) {
    // Check if audit mode is requested
    if (argc > 1 && std::string(argv[1]) == "--audit") {
        std::cout << "\U0001F3A8 Blender Asset Audit Tool" << std::endl;
        std::cout << "Author: KleaSCM" << std::endl;
        std::cout << "Email: KleaSCM@gmail.com" << std::endl;
        std::cout << "=====================================" << std::endl;
        try {
            std::filesystem::path projectRoot = std::filesystem::current_path();
            AssetAuditor auditor(projectRoot);
            auditor.runAudit();
        } catch (const std::exception& e) {
            std::cerr << "❌ Exception occurred: " << e.what() << std::endl;
            return 1;
        } catch (...) {
            std::cerr << "❌ Unknown exception occurred!" << std::endl;
            return 1;
        }
        return 0;
    }

    // Default asset manager mode
    std::cout << "🎨 Blender Asset Manager C++ Core" << std::endl;
    std::cout << "Author: KleaSCM" << std::endl;
    std::cout << "Email: KleaSCM@gmail.com" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    try {
        // Create asset manager instance
        AssetManager::AssetManager manager;
        
        // Initialize with current directory
        std::cout << "🔧 Initializing asset manager..." << std::endl;
        if (!manager.initialize()) {
            std::cerr << "❌ Failed to initialize asset manager!" << std::endl;
            return 1;
        }
        
        std::cout << "✅ Asset manager initialized successfully!" << std::endl;
        std::cout << "📁 Assets root: " << manager.get_assets_root() << std::endl;
        
        // Scan assets
        std::cout << "\n🔍 Scanning assets..." << std::endl;
        auto scan_start = std::chrono::high_resolution_clock::now();
        if (!manager.scan_assets()) {
            std::cerr << "❌ Failed to scan assets!" << std::endl;
            return 1;
        }
        auto scan_end = std::chrono::high_resolution_clock::now();
        auto scan_duration = std::chrono::duration_cast<std::chrono::milliseconds>(scan_end - scan_start);
        std::cout << "✅ Asset scan completed in " << scan_duration.count() << "ms" << std::endl;
        
        // Get asset statistics
        std::cout << "\n📊 Asset Statistics:" << std::endl;
        std::cout << manager.get_asset_stats() << std::endl;
        
        // Get all assets
        auto all_assets = manager.get_all_assets();
        std::cout << "\n📋 Total assets found: " << all_assets.size() << std::endl;
        
        // Display first few assets
        std::cout << "\n📁 Sample Assets:" << std::endl;
        for (size_t i = 0; i < std::min(size_t(5), all_assets.size()); ++i) {
            const auto& asset = all_assets[i];
            std::cout << "  • " << asset.name << " (" << asset.type << ") - " << asset.path << std::endl;
        }
        
        // Test search functionality
        std::cout << "\n🔎 Testing search functionality..." << std::endl;
        AssetManager::SearchFilters filters;
        filters.search_term = "building";
        auto search_results = manager.search_assets(filters);
        std::cout << "Found " << search_results.size() << " assets matching 'building'" << std::endl;
        
        // Test material presets
        std::cout << "\n🎨 Testing material system..." << std::endl;
        std::cout << "Available material presets:" << std::endl;
        std::cout << manager.get_material_presets() << std::endl;
        
        // Test supported formats
        std::cout << "\n📄 Supported file formats:" << std::endl;
        std::cout << manager.get_supported_formats() << std::endl;
        
        // Performance test
        std::cout << "\n⚡ Performance Test:" << std::endl;
        auto perf_start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 100; ++i) {
            manager.get_all_assets();
        }
        auto perf_end = std::chrono::high_resolution_clock::now();
        auto perf_duration = std::chrono::duration_cast<std::chrono::microseconds>(perf_end - perf_start);
        std::cout << "100 asset queries completed in " << perf_duration.count() << "μs" << std::endl;
        std::cout << "Average query time: " << (perf_duration.count() / 100.0) << "μs" << std::endl;
        
        std::cout << "\n✅ All tests completed successfully!" << std::endl;
        std::cout << "🚀 C++ Asset Manager Core is ready for production!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception occurred: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Unknown exception occurred!" << std::endl;
        return 1;
    }
    
    return 0;
} 