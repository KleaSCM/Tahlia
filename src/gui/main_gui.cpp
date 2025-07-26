/*
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: main_gui.cpp
Description: Main GUI application entry point for Tahlia asset library interface.
             Initializes the GUI system and runs the main application loop.
*/

#include "gui/asset_library_gui.hpp"
#include "asset_manager.hpp"
#include "import_manager.hpp"
#include "material_manager.hpp"
#include "import_history.hpp"
#include "python_bridge.hpp"
#include <iostream>
#include <memory>
#include <stdexcept>

int main(int argc, char* argv[]) {
    try {
        std::cout << "🎨 Tahlia Asset Library GUI Starting..." << std::endl;
        std::cout << "🌸 Universal Asset Management System" << std::endl;
        std::cout << "✨ Built with Dear ImGui" << std::endl;
        std::cout << "💕 By KleaSCM" << std::endl;
        std::cout << std::endl;

        // Create GUI instance
        TahliaGUI::AssetLibraryGUI gui;

        // Configure GUI
        TahliaGUI::GUIConfig config;
        config.dark_theme = true;
        config.show_demo_window = false;
        config.show_metrics_window = false;
        config.font_scale = 1.0f;
        config.thumbnail_size = 128;
        config.default_view_mode = TahliaGUI::AssetViewMode::GRID;
        config.enable_docking = true;
        config.enable_multi_viewport = true;
        config.font_path = "src/gui/misc/fonts/Roboto-Medium.ttf";
        config.font_size = 16.0f;

        // Initialize GUI
        if (!gui.initialize(config)) {
            std::cerr << "❌ Failed to initialize GUI!" << std::endl;
            return 1;
        }

        std::cout << "✅ GUI initialized successfully!" << std::endl;

        // Set up callbacks
        gui.setAssetDoubleClickCallback([](const TahliaGUI::AssetItem& asset) {
            std::cout << "🖱️ Double-clicked asset: " << asset.name << std::endl;
            // TODO: Implement asset preview/import
        });

        gui.setAssetRightClickCallback([](const TahliaGUI::AssetItem& asset) {
            std::cout << "🖱️ Right-clicked asset: " << asset.name << std::endl;
            // TODO: Implement context menu
        });

        gui.setImportCallback([](const std::vector<std::string>& asset_paths, 
                                const TahliaGUI::ImportOptions& options) {
            std::cout << "📥 Importing " << asset_paths.size() << " assets..." << std::endl;
            for (const auto& path : asset_paths) {
                std::cout << "  📁 " << path << std::endl;
            }
            // TODO: Implement actual import
        });

        // Load sample assets for demonstration
        std::vector<TahliaGUI::AssetItem> sample_assets = {
            {"Character_01.fbx", "/assets/characters/Character_01.fbx", "Model", "Characters", "", false, true, 2048576, "2024-01-15 10:30:00", {"character", "human", "male"}},
            {"Environment_01.blend", "/assets/environments/Environment_01.blend", "Model", "Environment", "", false, true, 5120000, "2024-01-14 15:45:00", {"environment", "forest", "nature"}},
            {"Texture_01.png", "/assets/textures/Texture_01.png", "Texture", "Textures", "", false, true, 1048576, "2024-01-13 09:20:00", {"texture", "wood", "material"}},
            {"Material_01.mat", "/assets/materials/Material_01.mat", "Material", "Materials", "", false, true, 51200, "2024-01-12 14:10:00", {"material", "pbr", "metal"}},
            {"Audio_01.wav", "/assets/audio/Audio_01.wav", "Audio", "Audio", "", false, true, 8192000, "2024-01-11 11:30:00", {"audio", "music", "background"}},
            {"Video_01.mp4", "/assets/video/Video_01.mp4", "Video", "Video", "", false, true, 25600000, "2024-01-10 16:20:00", {"video", "cutscene", "cinematic"}},
            {"Prop_01.obj", "/assets/props/Prop_01.obj", "Model", "Props", "", false, true, 1024000, "2024-01-09 13:15:00", {"prop", "furniture", "table"}},
            {"UI_01.png", "/assets/ui/UI_01.png", "Texture", "UI", "", false, true, 256000, "2024-01-08 10:45:00", {"ui", "button", "interface"}},
            {"Effect_01.fx", "/assets/effects/Effect_01.fx", "Effect", "Effects", "", false, true, 128000, "2024-01-07 12:30:00", {"effect", "particle", "fire"}},
            {"Animation_01.fbx", "/assets/animations/Animation_01.fbx", "Animation", "Animations", "", false, true, 4096000, "2024-01-06 08:55:00", {"animation", "walk", "character"}}
        };

        // Add sample assets to GUI (this would normally come from the asset manager)
        // For now, we'll just print them
        std::cout << "📚 Sample assets loaded:" << std::endl;
        for (const auto& asset : sample_assets) {
            std::cout << "  📁 " << asset.name << " (" << asset.type << ")" << std::endl;
        }
        std::cout << std::endl;

        std::cout << "🚀 Starting GUI main loop..." << std::endl;
        std::cout << "💡 Tips:" << std::endl;
        std::cout << "   • Use Ctrl+O to open asset library" << std::endl;
        std::cout << "   • Use Ctrl+I to import assets" << std::endl;
        std::cout << "   • Use Ctrl+A to select all assets" << std::endl;
        std::cout << "   • Use Ctrl+D to clear selection" << std::endl;
        std::cout << "   • Use F5 to refresh library" << std::endl;
        std::cout << "   • Use Alt+F4 to exit" << std::endl;
        std::cout << std::endl;

        // Run the GUI
        gui.run();

        std::cout << "👋 Tahlia Asset Library GUI closed successfully!" << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "❌ Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Unknown fatal error occurred!" << std::endl;
        return 1;
    }
} 