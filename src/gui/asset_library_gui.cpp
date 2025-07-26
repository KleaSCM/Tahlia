/*
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: asset_library_gui.cpp
Description: Implementation of the Tahlia Asset Library GUI with docking system.
*/

#include "gui/asset_library_gui.hpp"
#include <iostream>

namespace TahliaGUI {

AssetLibraryGUI::AssetLibraryGUI() : window(nullptr), initialized(false) {
}

AssetLibraryGUI::~AssetLibraryGUI() {
    cleanup();
}

bool AssetLibraryGUI::initialize(const GUIConfig& config) {
    this->config = config;
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    if (config.enable_viewports) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    initialized = true;
    return true;
}

bool AssetLibraryGUI::createWindow() {
    if (!initialized) {
        std::cerr << "GUI not initialized" << std::endl;
        return false;
    }

    // Create window
    window = glfwCreateWindow(config.window_width, config.window_height, 
                             config.window_title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    // Configure ImGui
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    if (config.enable_docking) {
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }
    if (config.enable_viewports) {
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    }

    // Setup platform/renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Setup style
    if (config.theme == Theme::Dark) {
        ImGui::StyleColorsDark();
    } else {
        ImGui::StyleColorsLight();
    }

    // Configure style for docking
    if (config.enable_viewports) {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    return true;
}

int AssetLibraryGUI::run() {
    if (!window) {
        std::cerr << "Window not created" << std::endl;
        return 1;
    }

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render the main window
        renderMainWindow();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (config.enable_viewports) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        glfwSwapBuffers(window);
    }

    return 0;
}

void AssetLibraryGUI::cleanup() {
    if (initialized) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        
        if (window) {
            glfwDestroyWindow(window);
            window = nullptr;
        }
        
        glfwTerminate();
        initialized = false;
    }
}

void AssetLibraryGUI::renderMainWindow() {
    // Create the main window
    ImGui::SetNextWindowSize(ImVec2(1400, 900), ImGuiCond_FirstUseEver);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (config.enable_viewports) {
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("Tahlia Asset Library", nullptr, window_flags);
    ImGui::PopStyleVar(2);

    // Create docking space
    if (config.enable_docking) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    }

    // Render menu bar
    renderMenuBar();

    // Render main panels
    renderAssetBrowserPanel();
    renderAssetPreviewPanel();
    renderAssetDetailsPanel();
    renderSearchPanel();
    renderImportPanel();
    renderHistoryPanel();
    renderSettingsPanel();

    // Render status bar
    renderStatusBar();

    ImGui::End();
}

void AssetLibraryGUI::renderMenuBar() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open Asset Library", "Ctrl+O")) {
                // TODO: Implement open asset library
            }
            if (ImGui::MenuItem("Import Assets", "Ctrl+I")) {
                // TODO: Implement import assets
            }
            if (ImGui::MenuItem("Export Assets", "Ctrl+E")) {
                // TODO: Implement export assets
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Select All", "Ctrl+A")) {
                // TODO: Implement select all
            }
            if (ImGui::MenuItem("Clear Selection", "Ctrl+D")) {
                // TODO: Implement clear selection
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Copy", "Ctrl+C")) {
                // TODO: Implement copy
            }
            if (ImGui::MenuItem("Paste", "Ctrl+V")) {
                // TODO: Implement paste
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Grid View", "F1")) {
                // TODO: Switch to grid view
            }
            if (ImGui::MenuItem("List View", "F2")) {
                // TODO: Switch to list view
            }
            if (ImGui::MenuItem("Details View", "F3")) {
                // TODO: Switch to details view
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                // TODO: Show about dialog
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMenuBar();
    }
}

void AssetLibraryGUI::renderAssetBrowserPanel() {
    ImGui::Begin("Asset Browser");
    
    ImGui::Text("🌸 Asset Browser");
    ImGui::Separator();
    
    // View mode selector
    static int view_mode = 0;
    ImGui::RadioButton("Grid", &view_mode, 0); ImGui::SameLine();
    ImGui::RadioButton("List", &view_mode, 1); ImGui::SameLine();
    ImGui::RadioButton("Details", &view_mode, 2);
    
    ImGui::Separator();
    
    // Sample assets (placeholder)
    ImGui::Text("📁 Sample Assets:");
    ImGui::Text("  • Character_01.fbx");
    ImGui::Text("  • Environment_01.blend");
    ImGui::Text("  • Texture_01.png");
    ImGui::Text("  • Material_01.mat");
    ImGui::Text("  • Audio_01.wav");
    
    ImGui::End();
}

void AssetLibraryGUI::renderAssetPreviewPanel() {
    ImGui::Begin("Asset Preview");
    
    ImGui::Text("🎨 Asset Preview");
    ImGui::Separator();
    
    ImGui::Text("3D Preview Area");
    ImGui::Text("(Coming soon with OpenGL rendering)");
    
    // Placeholder for 3D preview
    ImVec2 preview_size = ImGui::GetContentRegionAvail();
    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetCursorScreenPos(),
        ImVec2(ImGui::GetCursorScreenPos().x + preview_size.x, ImGui::GetCursorScreenPos().y + preview_size.y),
        IM_COL32(50, 50, 50, 255)
    );
    
    ImGui::End();
}

void AssetLibraryGUI::renderAssetDetailsPanel() {
    ImGui::Begin("Asset Details");
    
    ImGui::Text("📋 Asset Details");
    ImGui::Separator();
    
    ImGui::Text("Name: Character_01.fbx");
    ImGui::Text("Type: Model");
    ImGui::Text("Category: Characters");
    ImGui::Text("Size: 2.0 MB");
    ImGui::Text("Created: 2024-01-15 10:30:00");
    ImGui::Text("Modified: 2024-01-15 10:30:00");
    
    ImGui::Separator();
    ImGui::Text("Tags:");
    ImGui::Text("  • character");
    ImGui::Text("  • human");
    ImGui::Text("  • male");
    
    ImGui::End();
}

void AssetLibraryGUI::renderSearchPanel() {
    ImGui::Begin("Search & Filter");
    
    ImGui::Text("🔍 Search & Filter");
    ImGui::Separator();
    
    static char search_buffer[256] = "";
    ImGui::InputText("Search", search_buffer, sizeof(search_buffer));
    
    ImGui::Separator();
    ImGui::Text("Filters:");
    
    static bool filter_models = true;
    static bool filter_textures = true;
    static bool filter_materials = true;
    static bool filter_audio = true;
    
    ImGui::Checkbox("Models", &filter_models);
    ImGui::Checkbox("Textures", &filter_textures);
    ImGui::Checkbox("Materials", &filter_materials);
    ImGui::Checkbox("Audio", &filter_audio);
    
    ImGui::End();
}

void AssetLibraryGUI::renderImportPanel() {
    ImGui::Begin("Import");
    
    ImGui::Text("📥 Import");
    ImGui::Separator();
    
    if (ImGui::Button("Select Files")) {
        // TODO: Implement file selection
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Select Folder")) {
        // TODO: Implement folder selection
    }
    
    ImGui::Separator();
    ImGui::Text("Import Options:");
    
    static bool create_thumbnails = true;
    static bool extract_metadata = true;
    static bool validate_assets = true;
    
    ImGui::Checkbox("Create Thumbnails", &create_thumbnails);
    ImGui::Checkbox("Extract Metadata", &extract_metadata);
    ImGui::Checkbox("Validate Assets", &validate_assets);
    
    ImGui::End();
}

void AssetLibraryGUI::renderHistoryPanel() {
    ImGui::Begin("History");
    
    ImGui::Text("📚 History");
    ImGui::Separator();
    
    ImGui::Text("Recent Actions:");
    ImGui::Text("  • Imported Character_01.fbx");
    ImGui::Text("  • Created Material_01.mat");
    ImGui::Text("  • Exported Texture_01.png");
    ImGui::Text("  • Deleted old_asset.obj");
    
    ImGui::Separator();
    
    if (ImGui::Button("Clear History")) {
        // TODO: Clear history
    }
    
    ImGui::End();
}

void AssetLibraryGUI::renderSettingsPanel() {
    ImGui::Begin("Settings");
    
    ImGui::Text("⚙️ Settings");
    ImGui::Separator();
    
    ImGui::Text("General:");
    static bool auto_refresh = true;
    static bool show_hidden = false;
    ImGui::Checkbox("Auto Refresh", &auto_refresh);
    ImGui::Checkbox("Show Hidden Files", &show_hidden);
    
    ImGui::Separator();
    ImGui::Text("Display:");
    static float thumbnail_size = 128.0f;
    ImGui::SliderFloat("Thumbnail Size", &thumbnail_size, 64.0f, 256.0f);
    
    ImGui::End();
}

void AssetLibraryGUI::renderStatusBar() {
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiCond_Always, ImVec2(1.0f, 1.0f));
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 20.0f), ImGuiCond_Always);
    
    ImGuiWindowFlags status_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    
    ImGui::Begin("Status Bar", nullptr, status_flags);
    
    ImGui::Text("Ready | 5 assets loaded | 2.0 MB total");
    
    ImGui::End();
}

} // namespace TahliaGUI 