/*
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: main_gui.cpp
Description: Main entry point for the Tahlia GUI application with docking system.
*/

#include "gui/asset_library_gui.hpp"
#include <iostream>

int main() {
    std::cout << "🎨 Starting Tahlia Asset Library GUI..." << std::endl;
    std::cout << "🌸 Universal Asset Management System" << std::endl;
    std::cout << "💕 Built with Dear ImGui by KleaSCM" << std::endl;
    
    // Initialize the GUI
    TahliaGUI::AssetLibraryGUI gui;
    
    // Configure the GUI
    TahliaGUI::GUIConfig config;
    config.window_title = "Tahlia Asset Library";
    config.window_width = 1400;
    config.window_height = 900;
    config.enable_docking = true;
    config.enable_viewports = true;
    config.theme = TahliaGUI::Theme::Dark;
    
    // Initialize the GUI system
    if (!gui.initialize(config)) {
        std::cerr << "❌ Failed to initialize GUI" << std::endl;
        return 1;
    }
    
    std::cout << "✅ GUI initialized successfully!" << std::endl;
    std::cout << "🎨 Creating main window..." << std::endl;
    
    // Create the main window
    if (!gui.createWindow()) {
        std::cerr << "❌ Failed to create main window" << std::endl;
        gui.cleanup();
        return 1;
    }
    
    std::cout << "✅ Main window created!" << std::endl;
    std::cout << "🚀 Starting GUI main loop..." << std::endl;
    std::cout << "💡 Close the window to exit" << std::endl;
    
    // Run the main GUI loop
    int result = gui.run();
    
    // Cleanup
    gui.cleanup();
    
    std::cout << "👋 GUI application completed!" << std::endl;
    return result;
} 