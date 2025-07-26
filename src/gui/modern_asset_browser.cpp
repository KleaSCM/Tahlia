/*
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: modern_asset_browser.cpp
Description: Modern asset browser with beautiful styling and professional layout.
*/

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>

// Asset item structure
struct AssetItem {
    std::string name;
    std::string type;
    std::string category;
    std::string path;
    std::string preview_path;
    bool is_selected;
    bool is_favorite;
    size_t file_size;
    std::string last_modified;
    std::vector<std::string> tags;
    
    AssetItem(const std::string& n, const std::string& t, const std::string& c, 
              const std::string& p, size_t size, const std::string& modified)
        : name(n), type(t), category(c), path(p), file_size(size), 
          last_modified(modified), is_selected(false), is_favorite(false) {}
};

// Modern color scheme
namespace Colors {
    const ImVec4 Primary = ImVec4(0.2f, 0.3f, 0.8f, 1.0f);      // Blue
    const ImVec4 Secondary = ImVec4(0.8f, 0.2f, 0.6f, 1.0f);    // Pink
    const ImVec4 Success = ImVec4(0.2f, 0.8f, 0.3f, 1.0f);      // Green
    const ImVec4 Warning = ImVec4(0.8f, 0.6f, 0.2f, 1.0f);      // Orange
    const ImVec4 Danger = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);       // Red
    const ImVec4 Background = ImVec4(0.15f, 0.15f, 0.18f, 1.0f); // Dark gray
    const ImVec4 Card = ImVec4(0.22f, 0.22f, 0.25f, 1.0f);      // Card background
    const ImVec4 Text = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);         // Light text
    const ImVec4 TextMuted = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);    // Muted text
}

// Sample assets
std::vector<AssetItem> sample_assets = {
    AssetItem("Character_01.fbx", "Model", "Characters", "/assets/characters/Character_01.fbx", 2048576, "2024-01-15 10:30:00"),
    AssetItem("Environment_01.blend", "Model", "Environment", "/assets/environments/Environment_01.blend", 5120000, "2024-01-14 15:45:00"),
    AssetItem("Texture_01.png", "Texture", "Textures", "/assets/textures/Texture_01.png", 1048576, "2024-01-13 09:20:00"),
    AssetItem("Material_01.mat", "Material", "Materials", "/assets/materials/Material_01.mat", 51200, "2024-01-12 14:10:00"),
    AssetItem("Audio_01.wav", "Audio", "Audio", "/assets/audio/Audio_01.wav", 8192000, "2024-01-11 11:30:00"),
    AssetItem("Video_01.mp4", "Video", "Video", "/assets/video/Video_01.mp4", 25600000, "2024-01-10 16:20:00"),
    AssetItem("Prop_01.obj", "Model", "Props", "/assets/props/Prop_01.obj", 1024000, "2024-01-09 13:15:00"),
    AssetItem("UI_01.png", "Texture", "UI", "/assets/ui/UI_01.png", 256000, "2024-01-08 10:45:00"),
    AssetItem("Effect_01.fx", "Effect", "Effects", "/assets/effects/Effect_01.fx", 128000, "2024-01-07 12:30:00"),
    AssetItem("Animation_01.fbx", "Animation", "Animations", "/assets/animations/Animation_01.fbx", 4096000, "2024-01-06 08:55:00")
};

// Global state
static int selected_view_mode = 0; // 0=Grid, 1=List, 2=Details
static char search_buffer[256] = "";
static int selected_category = 0;
static int selected_type = 0;
static bool show_favorites_only = false;
static bool show_recent_only = false;
static float thumbnail_size = 120.0f;
static AssetItem* selected_asset = nullptr;

// Helper functions
void SetupModernStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Modern colors
    style.Colors[ImGuiCol_WindowBg] = Colors::Background;
    style.Colors[ImGuiCol_ChildBg] = Colors::Card;
    style.Colors[ImGuiCol_Text] = Colors::Text;
    style.Colors[ImGuiCol_TextDisabled] = Colors::TextMuted;
    style.Colors[ImGuiCol_Header] = Colors::Primary;
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(Colors::Primary.x + 0.1f, Colors::Primary.y + 0.1f, Colors::Primary.z + 0.1f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(Colors::Primary.x + 0.2f, Colors::Primary.y + 0.2f, Colors::Primary.z + 0.2f, 1.0f);
    style.Colors[ImGuiCol_Button] = Colors::Primary;
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(Colors::Primary.x + 0.1f, Colors::Primary.y + 0.1f, Colors::Primary.z + 0.1f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(Colors::Primary.x + 0.2f, Colors::Primary.y + 0.2f, Colors::Primary.z + 0.2f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = Colors::Card;
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(Colors::Card.x + 0.05f, Colors::Card.y + 0.05f, Colors::Card.z + 0.05f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(Colors::Card.x + 0.1f, Colors::Card.y + 0.1f, Colors::Card.z + 0.1f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = Colors::Primary;
    style.Colors[ImGuiCol_TitleBgActive] = Colors::Primary;
    style.Colors[ImGuiCol_ScrollbarBg] = Colors::Background;
    style.Colors[ImGuiCol_ScrollbarGrab] = Colors::Primary;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(Colors::Primary.x + 0.1f, Colors::Primary.y + 0.1f, Colors::Primary.z + 0.1f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(Colors::Primary.x + 0.2f, Colors::Primary.y + 0.2f, Colors::Primary.z + 0.2f, 1.0f);
    
    // Modern spacing and sizing
    style.WindowPadding = ImVec2(15, 15);
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(10, 8);
    style.ItemInnerSpacing = ImVec2(6, 4);
    style.ScrollbarSize = 12.0f;
    style.GrabMinSize = 8.0f;
    
    // Modern rounding
    style.WindowRounding = 8.0f;
    style.ChildRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.ScrollbarRounding = 6.0f;
    style.TabRounding = 4.0f;
}

void RenderAssetThumbnail(const AssetItem& asset, float size) {
    ImGui::BeginGroup();
    
    // Thumbnail background
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 thumbnail_size(size, size);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    // Draw thumbnail background
    draw_list->AddRectFilled(
        pos, 
        ImVec2(pos.x + thumbnail_size.x, pos.y + thumbnail_size.y),
        IM_COL32(40, 40, 45, 255)
    );
    
    // Draw asset type icon
    ImVec2 icon_pos = ImVec2(pos.x + thumbnail_size.x * 0.5f - 20, pos.y + thumbnail_size.y * 0.5f - 20);
    ImVec4 icon_color = Colors::Primary;
    
    if (asset.type == "Model") {
        draw_list->AddRectFilled(icon_pos, ImVec2(icon_pos.x + 40, icon_pos.y + 40), IM_COL32(100, 150, 255, 255));
        draw_list->AddText(ImVec2(icon_pos.x + 8, icon_pos.y + 12), IM_COL32(255, 255, 255, 255), "3D");
    } else if (asset.type == "Texture") {
        draw_list->AddRectFilled(icon_pos, ImVec2(icon_pos.x + 40, icon_pos.y + 40), IM_COL32(255, 100, 150, 255));
        draw_list->AddText(ImVec2(icon_pos.x + 8, icon_pos.y + 12), IM_COL32(255, 255, 255, 255), "TEX");
    } else if (asset.type == "Material") {
        draw_list->AddRectFilled(icon_pos, ImVec2(icon_pos.x + 40, icon_pos.y + 40), IM_COL32(255, 200, 100, 255));
        draw_list->AddText(ImVec2(icon_pos.x + 8, icon_pos.y + 12), IM_COL32(255, 255, 255, 255), "MAT");
    } else if (asset.type == "Audio") {
        draw_list->AddRectFilled(icon_pos, ImVec2(icon_pos.x + 40, icon_pos.y + 40), IM_COL32(100, 255, 150, 255));
        draw_list->AddText(ImVec2(icon_pos.x + 8, icon_pos.y + 12), IM_COL32(255, 255, 255, 255), "AUD");
    } else {
        draw_list->AddRectFilled(icon_pos, ImVec2(icon_pos.x + 40, icon_pos.y + 40), IM_COL32(150, 150, 150, 255));
        draw_list->AddText(ImVec2(icon_pos.x + 8, icon_pos.y + 12), IM_COL32(255, 255, 255, 255), "FILE");
    }
    
    // Selection overlay
    if (asset.is_selected) {
        draw_list->AddRect(
            pos, 
            ImVec2(pos.x + thumbnail_size.x, pos.y + thumbnail_size.y),
            IM_COL32(100, 150, 255, 255), 0.0f, 0, 3.0f
        );
    }
    
    ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + thumbnail_size.y + 5));
    
    // Asset name
    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + size);
    ImGui::TextColored(Colors::Text, "%s", asset.name.c_str());
    ImGui::PopTextWrapPos();
    
    // Asset type
    ImGui::TextColored(Colors::TextMuted, "%s", asset.type.c_str());
    
    ImGui::EndGroup();
}

void RenderAssetGrid() {
    float window_width = ImGui::GetWindowWidth();
    int columns = static_cast<int>(window_width / (thumbnail_size + 20));
    if (columns < 1) columns = 1;
    
    ImGui::Columns(columns, nullptr, false);
    
    for (auto& asset : sample_assets) {
        if (ImGui::IsItemClicked()) {
            // Clear previous selection
            for (auto& a : sample_assets) a.is_selected = false;
            asset.is_selected = true;
            selected_asset = &asset;
        }
        
        RenderAssetThumbnail(asset, thumbnail_size);
        ImGui::NextColumn();
    }
    
    ImGui::Columns(1);
}

void RenderAssetList() {
    ImGui::BeginTable("AssetList", 5, ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | 
                                     ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | 
                                     ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter);
    
    ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort);
    ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100.0f);
    ImGui::TableSetupColumn("Category", ImGuiTableColumnFlags_WidthFixed, 120.0f);
    ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 80.0f);
    ImGui::TableSetupColumn("Modified", ImGuiTableColumnFlags_WidthFixed, 120.0f);
    ImGui::TableHeadersRow();
    
    for (auto& asset : sample_assets) {
        ImGui::TableNextRow();
        
        // Name column
        ImGui::TableSetColumnIndex(0);
        if (ImGui::Selectable(asset.name.c_str(), asset.is_selected, ImGuiSelectableFlags_SpanAllColumns)) {
            for (auto& a : sample_assets) a.is_selected = false;
            asset.is_selected = true;
            selected_asset = &asset;
        }
        
        // Type column
        ImGui::TableSetColumnIndex(1);
        ImGui::Text("%s", asset.type.c_str());
        
        // Category column
        ImGui::TableSetColumnIndex(2);
        ImGui::Text("%s", asset.category.c_str());
        
        // Size column
        ImGui::TableSetColumnIndex(3);
        if (asset.file_size < 1024) {
            ImGui::Text("%zu B", asset.file_size);
        } else if (asset.file_size < 1024 * 1024) {
            ImGui::Text("%.1f KB", asset.file_size / 1024.0f);
        } else {
            ImGui::Text("%.1f MB", asset.file_size / (1024.0f * 1024.0f));
        }
        
        // Modified column
        ImGui::TableSetColumnIndex(4);
        ImGui::Text("%s", asset.last_modified.c_str());
    }
    
    ImGui::EndTable();
}

void RenderToolbar() {
    ImGui::BeginChild("Toolbar", ImVec2(0, 60), true);
    
    // View mode buttons
    ImGui::PushStyleColor(ImGuiCol_Button, selected_view_mode == 0 ? Colors::Primary : Colors::Card);
    if (ImGui::Button("Grid", ImVec2(60, 30))) selected_view_mode = 0;
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, selected_view_mode == 1 ? Colors::Primary : Colors::Card);
    if (ImGui::Button("List", ImVec2(60, 30))) selected_view_mode = 1;
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, selected_view_mode == 2 ? Colors::Primary : Colors::Card);
    if (ImGui::Button("Details", ImVec2(60, 30))) selected_view_mode = 2;
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    ImGui::Separator();
    ImGui::SameLine();
    
    // Import button
    ImGui::PushStyleColor(ImGuiCol_Button, Colors::Success);
    if (ImGui::Button("Import Assets", ImVec2(120, 30))) {
        // TODO: Open import dialog
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    
    // Export button
    ImGui::PushStyleColor(ImGuiCol_Button, Colors::Warning);
    if (ImGui::Button("Export", ImVec2(80, 30))) {
        // TODO: Open export dialog
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    
    // Delete button
    ImGui::PushStyleColor(ImGuiCol_Button, Colors::Danger);
    if (ImGui::Button("Delete", ImVec2(80, 30))) {
        // TODO: Delete selected assets
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    ImGui::Separator();
    ImGui::SameLine();
    
    // Thumbnail size slider
    ImGui::Text("Thumbnail Size:");
    ImGui::SameLine();
    ImGui::SliderFloat("##ThumbnailSize", &thumbnail_size, 80.0f, 200.0f, "%.0f");
    
    ImGui::EndChild();
}

void RenderSidebar() {
    ImGui::BeginChild("Sidebar", ImVec2(250, 0), true);
    
    ImGui::Text("🔍 Search & Filter");
    ImGui::Separator();
    
    // Search box
    ImGui::InputText("Search", search_buffer, sizeof(search_buffer));
    
    ImGui::Separator();
    ImGui::Text("Categories:");
    
    const char* categories[] = {"All", "Characters", "Props", "Environment", "UI", "Effects"};
    ImGui::Combo("Category", &selected_category, categories, IM_ARRAYSIZE(categories));
    
    ImGui::Separator();
    ImGui::Text("Types:");
    
    const char* types[] = {"All", "Model", "Texture", "Material", "Audio", "Video"};
    ImGui::Combo("Type", &selected_type, types, IM_ARRAYSIZE(types));
    
    ImGui::Separator();
    ImGui::Checkbox("Favorites Only", &show_favorites_only);
    ImGui::Checkbox("Recent Only", &show_recent_only);
    
    ImGui::Separator();
    ImGui::Text("Quick Actions:");
    
    if (ImGui::Button("Refresh Library", ImVec2(-1, 30))) {
        // TODO: Refresh asset library
    }
    
    if (ImGui::Button("Generate Thumbnails", ImVec2(-1, 30))) {
        // TODO: Generate thumbnails
    }
    
    if (ImGui::Button("Validate Assets", ImVec2(-1, 30))) {
        // TODO: Validate assets
    }
    
    ImGui::EndChild();
}

void RenderAssetPreview() {
    ImGui::BeginChild("Asset Preview", ImVec2(0, 300), true);
    
    if (selected_asset) {
        ImGui::Text("🎨 Asset Preview: %s", selected_asset->name.c_str());
        ImGui::Separator();
        
        // Preview area
        ImVec2 preview_size = ImGui::GetContentRegionAvail();
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        // Draw preview background
        draw_list->AddRectFilled(
            pos, 
            ImVec2(pos.x + preview_size.x - 20, pos.y + preview_size.y - 100),
            IM_COL32(30, 30, 35, 255)
        );
        
        // Draw preview content (placeholder)
        ImVec2 center = ImVec2(pos.x + (preview_size.x - 20) * 0.5f, pos.y + (preview_size.y - 100) * 0.5f);
        draw_list->AddText(center, IM_COL32(150, 150, 150, 255), "3D Preview Area");
        draw_list->AddText(ImVec2(center.x - 30, center.y + 20), IM_COL32(100, 100, 100, 255), "(Coming Soon)");
        
        ImGui::SetCursorScreenPos(ImVec2(pos.x, pos.y + preview_size.y - 80));
        
        // Asset info
        ImGui::Text("Name: %s", selected_asset->name.c_str());
        ImGui::Text("Type: %s", selected_asset->type.c_str());
        ImGui::Text("Category: %s", selected_asset->category.c_str());
        ImGui::Text("Size: %.1f MB", selected_asset->file_size / (1024.0f * 1024.0f));
        ImGui::Text("Modified: %s", selected_asset->last_modified.c_str());
        
    } else {
        ImGui::Text("🎨 Asset Preview");
        ImGui::Separator();
        ImGui::Text("Select an asset to preview");
    }
    
    ImGui::EndChild();
}

int main() {
    std::cout << "🎨 Starting Modern Asset Browser..." << std::endl;
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }

    // Create window
    GLFWwindow* window = glfwCreateWindow(1600, 1000, "Tahlia Asset Library - Modern Browser", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup platform/renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Setup modern style
    SetupModernStyle();

    std::cout << "✅ Modern Asset Browser initialized!" << std::endl;
    std::cout << "💡 Close the window to exit" << std::endl;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Main window
        ImGui::SetNextWindowSize(ImVec2(1600, 1000), ImGuiCond_FirstUseEver);
        ImGui::Begin("Tahlia Asset Library - Modern Browser", nullptr, ImGuiWindowFlags_MenuBar);
        
        // Menu bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Import Assets", "Ctrl+I")) {}
                if (ImGui::MenuItem("Export Assets", "Ctrl+E")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "Alt+F4")) {
                    glfwSetWindowShouldClose(window, true);
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit")) {
                if (ImGui::MenuItem("Select All", "Ctrl+A")) {}
                if (ImGui::MenuItem("Clear Selection", "Ctrl+D")) {}
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Grid View", "F1")) selected_view_mode = 0;
                if (ImGui::MenuItem("List View", "F2")) selected_view_mode = 1;
                if (ImGui::MenuItem("Details View", "F3")) selected_view_mode = 2;
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        
        // Toolbar
        RenderToolbar();
        
        // Main content area
        ImGui::BeginChild("MainContent", ImVec2(0, 0), false);
        
        // Sidebar
        RenderSidebar();
        ImGui::SameLine();
        
        // Main asset area
        ImGui::BeginChild("AssetArea", ImVec2(0, 0), true);
        
        // Asset preview at top
        RenderAssetPreview();
        
        // Asset browser
        ImGui::BeginChild("AssetBrowser", ImVec2(0, 0), true);
        
        if (selected_view_mode == 0) {
            RenderAssetGrid();
        } else if (selected_view_mode == 1) {
            RenderAssetList();
        } else {
            // Details view (same as list for now)
            RenderAssetList();
        }
        
        ImGui::EndChild(); // AssetBrowser
        ImGui::EndChild(); // AssetArea
        ImGui::EndChild(); // MainContent

        ImGui::End(); // Main window

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.15f, 0.15f, 0.18f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "👋 Modern Asset Browser completed!" << std::endl;
    return 0;
} 