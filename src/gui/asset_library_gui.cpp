/*
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: asset_library_gui.cpp
Description: Main GUI implementation for Tahlia asset library interface using Dear ImGui.
             Provides the interface for browsing, importing, and managing assets.
*/

#include "gui/asset_library_gui.hpp"
#include "asset_manager.hpp"
#include "import_manager.hpp"
#include "material_manager.hpp"
#include "import_history.hpp"
#include "python_bridge.hpp"
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace TahliaGUI {

    // Constructor
    AssetLibraryGUI::AssetLibraryGUI() 
        : window_(nullptr), initialized_(false), view_mode_(AssetViewMode::GRID) {
    }

    // Destructor
    AssetLibraryGUI::~AssetLibraryGUI() {
        cleanup();
    }

    // Initialize the GUI system
    bool AssetLibraryGUI::initialize(const GUIConfig& config) {
        config_ = config;
        
        // Initialize GLFW
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return false;
        }

        // Create window
        if (!createWindow("Tahlia Asset Library", 1280, 720)) {
            return false;
        }

        // Setup Dear ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        // Note: Docking and Viewports are optional features
        // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        // Setup platform/renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window_, true);
        ImGui_ImplOpenGL3_Init("#version 130");

        // Setup theme and fonts
        setupTheme();
        setupFonts();

        initialized_ = true;
        std::cout << "Tahlia Asset Library GUI initialized successfully!" << std::endl;
        return true;
    }

    // Cleanup resources
    void AssetLibraryGUI::cleanup() {
        if (initialized_) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
            
            if (window_) {
                glfwDestroyWindow(window_);
                window_ = nullptr;
            }
            glfwTerminate();
            initialized_ = false;
        }
    }

    // Create GLFW window
    bool AssetLibraryGUI::createWindow(const std::string& title, int width, int height) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!window_) {
            std::cerr << "Failed to create GLFW window" << std::endl;
            return false;
        }

        glfwMakeContextCurrent(window_);
        glfwSwapInterval(1); // Enable vsync

        return true;
    }

    // Main run loop
    void AssetLibraryGUI::run() {
        if (!initialized_) {
            std::cerr << "GUI not initialized!" << std::endl;
            return;
        }

        while (!glfwWindowShouldClose(window_)) {
            glfwPollEvents();

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Render main window
            renderMainWindow();

            // Render demo window if enabled
            if (config_.show_demo_window) {
                ImGui::ShowDemoWindow(&config_.show_demo_window);
            }

            // Render metrics window if enabled
            if (config_.show_metrics_window) {
                ImGui::ShowMetricsWindow(&config_.show_metrics_window);
            }

            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window_, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                    // Update and Render additional Platform Windows
        // Note: Viewports are optional features
        // if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        //     GLFWwindow* backup_current_context = glfwGetCurrentContext();
        //     ImGui::UpdatePlatformWindows();
        //     ImGui::RenderPlatformWindowsDefault();
        //     glfwMakeContextCurrent(backup_current_context);
        // }

            glfwSwapBuffers(window_);
        }
    }

    // Render main window
    void AssetLibraryGUI::renderMainWindow() {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::SetNextWindowBgAlpha(0.0f);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | 
                                       ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | 
                                       ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
                                       ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("Tahlia Asset Library", nullptr, window_flags);

        // Render menu bar
        renderMenuBar();

        // Setup docking
        // Note: Docking is an optional feature
        // if (config_.enable_docking) {
        //     setupDocking();
        // }

        // Render docking layout
        renderDockingLayout();

        // Render status bar
        renderStatusBar();

        ImGui::End();
    }

    // Render menu bar
    void AssetLibraryGUI::renderMenuBar() {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open Asset Library", "Ctrl+O")) {
                    // TODO: Implement open asset library
                }
                if (ImGui::MenuItem("Save Asset Library", "Ctrl+S")) {
                    // TODO: Implement save asset library
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Import Assets", "Ctrl+I")) {
                    show_import_panel_ = true;
                }
                if (ImGui::MenuItem("Export Assets", "Ctrl+E")) {
                    // TODO: Implement export assets
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4")) {
                    glfwSetWindowShouldClose(window_, true);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Select All", "Ctrl+A")) {
                    // TODO: Implement select all
                }
                if (ImGui::MenuItem("Clear Selection", "Ctrl+D")) {
                    clearSelection();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Delete Selected", "Del")) {
                    // TODO: Implement delete selected
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Asset Browser", nullptr, &show_asset_browser_)) {}
                if (ImGui::MenuItem("Asset Preview", nullptr, &show_asset_preview_)) {}
                if (ImGui::MenuItem("Asset Details", nullptr, &show_asset_details_)) {}
                if (ImGui::MenuItem("Search & Filter", nullptr, &show_search_filter_)) {}
                if (ImGui::MenuItem("Import Panel", nullptr, &show_import_panel_)) {}
                if (ImGui::MenuItem("Material Editor", nullptr, &show_material_editor_)) {}
                if (ImGui::MenuItem("History", nullptr, &show_history_panel_)) {}
                if (ImGui::MenuItem("Settings", nullptr, &show_settings_panel_)) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Grid View", nullptr, view_mode_ == AssetViewMode::GRID)) {
                    setViewMode(AssetViewMode::GRID);
                }
                if (ImGui::MenuItem("List View", nullptr, view_mode_ == AssetViewMode::LIST)) {
                    setViewMode(AssetViewMode::LIST);
                }
                if (ImGui::MenuItem("Details View", nullptr, view_mode_ == AssetViewMode::DETAILS)) {
                    setViewMode(AssetViewMode::DETAILS);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Tools")) {
                if (ImGui::MenuItem("Refresh Library", "F5")) {
                    refreshAssetLibrary();
                }
                if (ImGui::MenuItem("Generate Thumbnails")) {
                    // TODO: Implement generate thumbnails
                }
                if (ImGui::MenuItem("Validate Assets")) {
                    // TODO: Implement validate assets
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Demo Window", nullptr, &config_.show_demo_window)) {}
                if (ImGui::MenuItem("Metrics Window", nullptr, &config_.show_metrics_window)) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help")) {
                if (ImGui::MenuItem("About Tahlia")) {
                    // TODO: Implement about dialog
                }
                if (ImGui::MenuItem("Documentation")) {
                    // TODO: Open documentation
                }
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

    // Render status bar
    void AssetLibraryGUI::renderStatusBar() {
        ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - 20));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, 20));
        ImGui::SetNextWindowBgAlpha(0.8f);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                                       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus;

        ImGui::Begin("Status Bar", nullptr, window_flags);

        // Display status information
        ImGui::Text("Assets: %zu | Selected: %zu | Filtered: %zu", 
                   assets_.size(), selected_assets_.size(), filtered_assets_.size());

        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 200);
        ImGui::Text("View: %s", 
                   view_mode_ == AssetViewMode::GRID ? "Grid" : 
                   view_mode_ == AssetViewMode::LIST ? "List" : "Details");

        ImGui::End();
    }

    // Setup docking
    void AssetLibraryGUI::setupDocking() {
        // ImGui::DockSpace(ImGui::GetID("MyDockSpace"), ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
    }

    // Render docking layout
    void AssetLibraryGUI::renderDockingLayout() {
        // Asset Browser Panel
        if (show_asset_browser_) {
            ImGui::Begin("Asset Browser", &show_asset_browser_);
            renderAssetBrowserPanel();
            ImGui::End();
        }

        // Asset Preview Panel
        if (show_asset_preview_) {
            ImGui::Begin("Asset Preview", &show_asset_preview_);
            renderAssetPreviewPanel();
            ImGui::End();
        }

        // Asset Details Panel
        if (show_asset_details_) {
            ImGui::Begin("Asset Details", &show_asset_details_);
            renderAssetDetailsPanel();
            ImGui::End();
        }

        // Search & Filter Panel
        if (show_search_filter_) {
            ImGui::Begin("Search & Filter", &show_search_filter_);
            renderSearchFilterPanel();
            ImGui::End();
        }

        // Import Panel
        if (show_import_panel_) {
            ImGui::Begin("Import Assets", &show_import_panel_);
            renderImportPanel();
            ImGui::End();
        }

        // Material Editor Panel
        if (show_material_editor_) {
            ImGui::Begin("Material Editor", &show_material_editor_);
            renderMaterialEditorPanel();
            ImGui::End();
        }

        // History Panel
        if (show_history_panel_) {
            ImGui::Begin("Import History", &show_history_panel_);
            renderHistoryPanel();
            ImGui::End();
        }

        // Settings Panel
        if (show_settings_panel_) {
            ImGui::Begin("Settings", &show_settings_panel_);
            renderSettingsPanel();
            ImGui::End();
        }
    }

    // Render asset browser panel
    void AssetLibraryGUI::renderAssetBrowserPanel() {
        // Toolbar
        if (ImGui::Button("Refresh")) {
            refreshAssetLibrary();
        }
        ImGui::SameLine();
        if (ImGui::Button("Import")) {
            show_import_panel_ = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete")) {
            // TODO: Implement delete selected
        }

        ImGui::Separator();

        // View mode buttons
        if (ImGui::Button("Grid")) {
            setViewMode(AssetViewMode::GRID);
        }
        ImGui::SameLine();
        if (ImGui::Button("List")) {
            setViewMode(AssetViewMode::LIST);
        }
        ImGui::SameLine();
        if (ImGui::Button("Details")) {
            setViewMode(AssetViewMode::DETAILS);
        }

        ImGui::Separator();

        // Asset display area
        ImGui::BeginChild("AssetDisplay", ImVec2(0, 0), true);

        switch (view_mode_) {
            case AssetViewMode::GRID:
                renderAssetGrid();
                break;
            case AssetViewMode::LIST:
                renderAssetList();
                break;
            case AssetViewMode::DETAILS:
                renderAssetDetails();
                break;
        }

        ImGui::EndChild();
    }

    // Render asset grid view
    void AssetLibraryGUI::renderAssetGrid() {
        float thumbnail_size = static_cast<float>(config_.thumbnail_size);
        float padding = 10.0f;
        float item_width = thumbnail_size + padding * 2;
        
        int columns = static_cast<int>(ImGui::GetWindowWidth() / item_width);
        if (columns < 1) columns = 1;

        ImGui::Columns(columns, nullptr, false);

        for (const auto& asset : filtered_assets_) {
            if (!asset.visible) continue;

            ImGui::BeginGroup();
            
            // Thumbnail (placeholder for now)
            ImGui::Button(asset.name.c_str(), ImVec2(thumbnail_size, thumbnail_size));
            
            // Asset name
            ImGui::TextWrapped("%s", asset.name.c_str());
            
            // Asset type
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", asset.type.c_str());

            ImGui::EndGroup();

            // Handle selection
            if (ImGui::IsItemClicked()) {
                if (ImGui::GetIO().KeyCtrl) {
                    // Multi-select
                    auto it = std::find(selected_assets_.begin(), selected_assets_.end(), asset.name);
                    if (it != selected_assets_.end()) {
                        selected_assets_.erase(it);
                    } else {
                        selected_assets_.push_back(asset.name);
                    }
                } else {
                    // Single select
                    selected_assets_.clear();
                    selected_assets_.push_back(asset.name);
                }
            }

            // Handle double-click (placeholder for now)
            // if (ImGui::IsItemDoubleClicked()) {
            //     if (asset_double_click_callback_) {
            //         asset_double_click_callback_(asset);
            //     }
            // }

            // Context menu
            if (ImGui::BeginPopupContextItem(asset.name.c_str())) {
                renderContextMenu(asset);
                ImGui::EndPopup();
            }

            ImGui::NextColumn();
        }

        ImGui::Columns(1);
    }

    // Render asset list view
    void AssetLibraryGUI::renderAssetList() {
        ImGui::BeginTable("AssetList", 4, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | 
                                         ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable);

        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Modified", ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableHeadersRow();

        for (const auto& asset : filtered_assets_) {
            if (!asset.visible) continue;

            ImGui::TableNextRow();
            
            // Name column
            ImGui::TableSetColumnIndex(0);
            bool selected = std::find(selected_assets_.begin(), selected_assets_.end(), asset.name) != selected_assets_.end();
            if (ImGui::Selectable(asset.name.c_str(), selected, ImGuiSelectableFlags_SpanAllColumns)) {
                if (ImGui::GetIO().KeyCtrl) {
                    auto it = std::find(selected_assets_.begin(), selected_assets_.end(), asset.name);
                    if (it != selected_assets_.end()) {
                        selected_assets_.erase(it);
                    } else {
                        selected_assets_.push_back(asset.name);
                    }
                } else {
                    selected_assets_.clear();
                    selected_assets_.push_back(asset.name);
                }
            }

            // Type column
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", asset.type.c_str());

            // Size column
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%zu KB", asset.file_size / 1024);

            // Modified column
            ImGui::TableSetColumnIndex(3);
            ImGui::Text("%s", asset.last_modified.c_str());
        }

        ImGui::EndTable();
    }

    // Render asset details view
    void AssetLibraryGUI::renderAssetDetails() {
        // This will be implemented to show detailed information about selected assets
        ImGui::Text("Asset Details View - Coming Soon!");
    }

    // Render asset preview panel
    void AssetLibraryGUI::renderAssetPreviewPanel() {
        if (selected_assets_.empty()) {
            ImGui::Text("No asset selected");
            return;
        }

        // Show preview of first selected asset
        const std::string& selected_asset_name = selected_assets_[0];
        
        // Find the asset
        auto it = std::find_if(assets_.begin(), assets_.end(), 
                              [&](const AssetItem& asset) { return asset.name == selected_asset_name; });
        
        if (it != assets_.end()) {
            const AssetItem& asset = *it;
            
            ImGui::Text("Preview: %s", asset.name.c_str());
            ImGui::Separator();
            
            // TODO: Implement actual asset preview rendering
            ImGui::Text("Preview rendering not yet implemented");
            ImGui::Text("Asset: %s", asset.name.c_str());
            ImGui::Text("Type: %s", asset.type.c_str());
            ImGui::Text("Path: %s", asset.path.c_str());
            ImGui::Text("Size: %zu bytes", asset.file_size);
        }
    }

    // Render asset details panel
    void AssetLibraryGUI::renderAssetDetailsPanel() {
        if (selected_assets_.empty()) {
            ImGui::Text("No asset selected");
            return;
        }

        // Show details of first selected asset
        const std::string& selected_asset_name = selected_assets_[0];
        
        auto it = std::find_if(assets_.begin(), assets_.end(), 
                              [&](const AssetItem& asset) { return asset.name == selected_asset_name; });
        
        if (it != assets_.end()) {
            const AssetItem& asset = *it;
            
            ImGui::Text("Asset Details");
            ImGui::Separator();
            
            ImGui::Text("Name: %s", asset.name.c_str());
            ImGui::Text("Type: %s", asset.type.c_str());
            ImGui::Text("Category: %s", asset.category.c_str());
            ImGui::Text("Path: %s", asset.path.c_str());
            ImGui::Text("Size: %zu bytes", asset.file_size);
            ImGui::Text("Modified: %s", asset.last_modified.c_str());
            
            if (!asset.tags.empty()) {
                ImGui::Text("Tags:");
                for (const auto& tag : asset.tags) {
                    ImGui::BulletText("%s", tag.c_str());
                }
            }
        }
    }

    // Render search filter panel
    void AssetLibraryGUI::renderSearchFilterPanel() {
        ImGui::Text("Search & Filter");
        ImGui::Separator();

        // Search text
        static char search_buffer[256] = "";
        if (ImGui::InputText("Search", search_buffer, sizeof(search_buffer))) {
            current_filter_.search_text = search_buffer;
            updateFilteredAssets();
        }

        // File type filter
        static const char* file_types[] = {"All", "Models", "Textures", "Materials", "Audio", "Video"};
        static int selected_file_type = 0;
        if (ImGui::Combo("File Type", &selected_file_type, file_types, IM_ARRAYSIZE(file_types))) {
            current_filter_.file_type_filter = file_types[selected_file_type];
            updateFilteredAssets();
        }

        // Category filter
        static const char* categories[] = {"All", "Characters", "Props", "Environment", "UI", "Effects"};
        static int selected_category = 0;
        if (ImGui::Combo("Category", &selected_category, categories, IM_ARRAYSIZE(categories))) {
            current_filter_.category_filter = categories[selected_category];
            updateFilteredAssets();
        }

        // Checkboxes
        ImGui::Checkbox("Favorites Only", &current_filter_.show_only_favorites);
        ImGui::Checkbox("Recent Only", &current_filter_.show_only_recent);

        ImGui::Separator();

        // Clear filters button
        if (ImGui::Button("Clear Filters")) {
            clearSearchFilter();
            memset(search_buffer, 0, sizeof(search_buffer));
            selected_file_type = 0;
            selected_category = 0;
        }
    }

    // Render import panel
    void AssetLibraryGUI::renderImportPanel() {
        ImGui::Text("Import Assets");
        ImGui::Separator();

        static ImportOptions import_options;
        
        // Import options
        renderImportOptions(import_options);

        ImGui::Separator();

        // Import buttons
        if (ImGui::Button("Select Files")) {
            // TODO: Implement file selection dialog
        }
        ImGui::SameLine();
        if (ImGui::Button("Import Selected")) {
            // TODO: Implement import
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            show_import_panel_ = false;
        }
    }

    // Render material editor panel
    void AssetLibraryGUI::renderMaterialEditorPanel() {
        ImGui::Text("Material Editor");
        ImGui::Separator();
        ImGui::Text("Material editor functionality coming soon!");
    }

    // Render history panel
    void AssetLibraryGUI::renderHistoryPanel() {
        ImGui::Text("Import History");
        ImGui::Separator();
        ImGui::Text("History functionality coming soon!");
    }

    // Render settings panel
    void AssetLibraryGUI::renderSettingsPanel() {
        ImGui::Text("Settings");
        ImGui::Separator();

        // Theme settings
        if (ImGui::CollapsingHeader("Theme")) {
            ImGui::Checkbox("Dark Theme", &config_.dark_theme);
            ImGui::SliderFloat("Font Scale", &config_.font_scale, 0.5f, 2.0f);
        }

        // View settings
        if (ImGui::CollapsingHeader("View")) {
            ImGui::SliderInt("Thumbnail Size", &config_.thumbnail_size, 64, 256);
            ImGui::Checkbox("Enable Docking", &config_.enable_docking);
            ImGui::Checkbox("Enable Multi-Viewport", &config_.enable_multi_viewport);
        }

        // Performance settings
        if (ImGui::CollapsingHeader("Performance")) {
            ImGui::Checkbox("Show Demo Window", &config_.show_demo_window);
            ImGui::Checkbox("Show Metrics Window", &config_.show_metrics_window);
        }
    }

    // Setup theme
    void AssetLibraryGUI::setupTheme() {
        if (config_.dark_theme) {
            ImGui::StyleColorsDark();
        } else {
            ImGui::StyleColorsLight();
        }

        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 5.0f;
        style.FrameRounding = 4.0f;
        style.GrabRounding = 3.0f;
        style.ScrollbarRounding = 3.0f;
        style.TabRounding = 3.0f;
    }

    // Setup fonts
    void AssetLibraryGUI::setupFonts() {
        ImGuiIO& io = ImGui::GetIO();
        
        // Load default font
        io.Fonts->AddFontDefault();
        
        // TODO: Load custom font if specified
        if (!config_.font_path.empty()) {
            // io.Fonts->AddFontFromFileTTF(config_.font_path.c_str(), config_.font_size);
        }
    }

    // Asset management methods
    void AssetLibraryGUI::loadAssetLibrary(const std::string& library_path) {
        // TODO: Implement asset library loading
        std::cout << "Loading asset library from: " << library_path << std::endl;
    }

    void AssetLibraryGUI::refreshAssetLibrary() {
        // TODO: Implement asset library refresh
        std::cout << "Refreshing asset library..." << std::endl;
        updateFilteredAssets();
    }

    void AssetLibraryGUI::importAssets(const std::vector<std::string>& asset_paths, const ImportOptions& options) {
        // TODO: Implement asset import
        std::cout << "Importing " << asset_paths.size() << " assets..." << std::endl;
    }

    void AssetLibraryGUI::exportAssets(const std::vector<std::string>& asset_names, const std::string& export_path) {
        // TODO: Implement asset export
        std::cout << "Exporting " << asset_names.size() << " assets to: " << export_path << std::endl;
    }

    void AssetLibraryGUI::deleteAssets(const std::vector<std::string>& asset_names) {
        // TODO: Implement asset deletion
        std::cout << "Deleting " << asset_names.size() << " assets..." << std::endl;
    }

    // Search and filtering methods
    void AssetLibraryGUI::setSearchFilter(const SearchFilter& filter) {
        current_filter_ = filter;
        updateFilteredAssets();
    }

    void AssetLibraryGUI::clearSearchFilter() {
        current_filter_ = SearchFilter{};
        updateFilteredAssets();
    }

    std::vector<AssetItem> AssetLibraryGUI::getFilteredAssets() const {
        return filtered_assets_;
    }

    // Selection management methods
    void AssetLibraryGUI::selectAsset(const std::string& asset_name) {
        selected_assets_.clear();
        selected_assets_.push_back(asset_name);
    }

    void AssetLibraryGUI::selectAssets(const std::vector<std::string>& asset_names) {
        selected_assets_ = asset_names;
    }

    void AssetLibraryGUI::clearSelection() {
        selected_assets_.clear();
    }

    std::vector<std::string> AssetLibraryGUI::getSelectedAssets() const {
        return selected_assets_;
    }

    // View management methods
    void AssetLibraryGUI::setViewMode(AssetViewMode mode) {
        view_mode_ = mode;
    }

    void AssetLibraryGUI::setThumbnailSize(int size) {
        config_.thumbnail_size = size;
    }

    void AssetLibraryGUI::togglePanel(PanelType panel_type, bool show) {
        switch (panel_type) {
            case PanelType::ASSET_BROWSER: show_asset_browser_ = show; break;
            case PanelType::ASSET_PREVIEW: show_asset_preview_ = show; break;
            case PanelType::ASSET_DETAILS: show_asset_details_ = show; break;
            case PanelType::SEARCH_FILTER: show_search_filter_ = show; break;
            case PanelType::IMPORT_PANEL: show_import_panel_ = show; break;
            case PanelType::MATERIAL_EDITOR: show_material_editor_ = show; break;
            case PanelType::HISTORY_PANEL: show_history_panel_ = show; break;
            case PanelType::SETTINGS_PANEL: show_settings_panel_ = show; break;
        }
    }

    // Callback setters
    void AssetLibraryGUI::setAssetDoubleClickCallback(std::function<void(const AssetItem&)> callback) {
        asset_double_click_callback_ = callback;
    }

    void AssetLibraryGUI::setAssetRightClickCallback(std::function<void(const AssetItem&)> callback) {
        asset_right_click_callback_ = callback;
    }

    void AssetLibraryGUI::setImportCallback(std::function<void(const std::vector<std::string>&, const ImportOptions&)> callback) {
        import_callback_ = callback;
    }

    // Private helper methods
    void AssetLibraryGUI::renderContextMenu(const AssetItem& asset) {
        if (ImGui::MenuItem("Open")) {
            // TODO: Implement open asset
        }
        if (ImGui::MenuItem("Import")) {
            // TODO: Implement import asset
        }
        if (ImGui::MenuItem("Export")) {
            // TODO: Implement export asset
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Delete")) {
            // TODO: Implement delete asset
        }
    }

    void AssetLibraryGUI::renderImportOptions(ImportOptions& options) {
        static char target_location[256] = "";
        ImGui::InputText("Target Location", target_location, sizeof(target_location));
        options.target_location = target_location;
        ImGui::SliderFloat("Scale", &options.scale, 0.1f, 10.0f);
        ImGui::DragFloat3("Rotation", options.rotation, 1.0f, -180.0f, 180.0f);
        ImGui::DragFloat3("Position", options.position, 0.1f);
        ImGui::Checkbox("Merge Objects", &options.merge_objects);
        ImGui::Checkbox("Auto Smooth", &options.auto_smooth);
        ImGui::Checkbox("Link Assets", &options.link_assets);
        
        const char* patterns[] = {"Single", "Grid", "Circle", "Line", "Random"};
        static int selected_pattern = 0;
        if (ImGui::Combo("Import Pattern", &selected_pattern, patterns, IM_ARRAYSIZE(patterns))) {
            options.import_pattern = patterns[selected_pattern];
        }
    }

    void AssetLibraryGUI::updateFilteredAssets() {
        filtered_assets_.clear();
        
        for (const auto& asset : assets_) {
            if (isAssetVisible(asset)) {
                filtered_assets_.push_back(asset);
            }
        }
    }

    bool AssetLibraryGUI::isAssetVisible(const AssetItem& asset) const {
        // Search text filter
        if (!current_filter_.search_text.empty()) {
            if (asset.name.find(current_filter_.search_text) == std::string::npos &&
                asset.type.find(current_filter_.search_text) == std::string::npos) {
                return false;
            }
        }

        // File type filter
        if (!current_filter_.file_type_filter.empty() && current_filter_.file_type_filter != "All") {
            if (asset.type != current_filter_.file_type_filter) {
                return false;
            }
        }

        // Category filter
        if (!current_filter_.category_filter.empty() && current_filter_.category_filter != "All") {
            if (asset.category != current_filter_.category_filter) {
                return false;
            }
        }

        // File size filter
        if (asset.file_size < current_filter_.min_file_size || 
            asset.file_size > current_filter_.max_file_size) {
            return false;
        }

        return true;
    }

    // Placeholder implementations for remaining methods
    void AssetLibraryGUI::renderMaterialEditor() {}
    void AssetLibraryGUI::renderHistoryList() {}
    void AssetLibraryGUI::renderSettings() {}
    void AssetLibraryGUI::loadThumbnails() {}
    void AssetLibraryGUI::generateThumbnail(const AssetItem& asset) {}
    void AssetLibraryGUI::handleDragAndDrop() {}
    void AssetLibraryGUI::handleKeyboardShortcuts() {}

} // namespace TahliaGUI 