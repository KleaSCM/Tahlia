/*
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: asset_library_gui.hpp
Description: Main GUI header for Tahlia asset library interface using Dear ImGui.
             Provides the interface for browsing, importing, and managing assets.
*/

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

// Forward declarations
namespace AssetManager {
    class AssetManager;
}
namespace ImportManager {
    class ImportManager;
}
namespace MaterialManager {
    class MaterialManager;
}
namespace ImportHistory {
    class ImportHistory;
}
namespace PythonBridge {
    class PythonBridge;
}

namespace TahliaGUI {

    // Asset view modes
    enum class AssetViewMode {
        GRID,
        LIST,
        DETAILS
    };

    // Panel types
    enum class PanelType {
        ASSET_BROWSER,
        ASSET_PREVIEW,
        ASSET_DETAILS,
        SEARCH_FILTER,
        IMPORT_PANEL,
        MATERIAL_EDITOR,
        HISTORY_PANEL,
        SETTINGS_PANEL
    };

    // Asset item structure
    struct AssetItem {
        std::string name;
        std::string path;
        std::string type;
        std::string category;
        std::string thumbnail_path;
        bool selected = false;
        bool visible = true;
        size_t file_size = 0;
        std::string last_modified;
        std::vector<std::string> tags;
    };

    // Search filter structure
    struct SearchFilter {
        std::string search_text;
        std::string file_type_filter;
        std::string category_filter;
        std::vector<std::string> tag_filters;
        bool show_only_favorites = false;
        bool show_only_recent = false;
        size_t min_file_size = 0;
        size_t max_file_size = SIZE_MAX;
    };

    // Import options structure
    struct ImportOptions {
        std::string target_location;
        float scale = 1.0f;
        float rotation[3] = {0.0f, 0.0f, 0.0f};
        float position[3] = {0.0f, 0.0f, 0.0f};
        bool merge_objects = false;
        bool auto_smooth = true;
        bool link_assets = false;
        std::string import_pattern = "single";
    };

    // GUI configuration
    struct GUIConfig {
        bool dark_theme = true;
        bool show_demo_window = false;
        bool show_metrics_window = false;
        float font_scale = 1.0f;
        int thumbnail_size = 128;
        AssetViewMode default_view_mode = AssetViewMode::GRID;
        bool enable_docking = true;
        bool enable_multi_viewport = true;
        std::string font_path = "misc/fonts/Roboto-Medium.ttf";
        float font_size = 16.0f;
    };

    // Main GUI application class
    class AssetLibraryGUI {
    public:
        AssetLibraryGUI();
        ~AssetLibraryGUI();

        // Initialization and setup
        bool initialize(const GUIConfig& config = GUIConfig{});
        void cleanup();
        bool createWindow(const std::string& title, int width, int height);
        void run();

        // Core GUI rendering
        void renderMainWindow();
        void renderMenuBar();
        void renderStatusBar();
        void renderDockingLayout();

        // Panel rendering
        void renderAssetBrowserPanel();
        void renderAssetPreviewPanel();
        void renderAssetDetailsPanel();
        void renderSearchFilterPanel();
        void renderImportPanel();
        void renderMaterialEditorPanel();
        void renderHistoryPanel();
        void renderSettingsPanel();

        // Asset management
        void loadAssetLibrary(const std::string& library_path);
        void refreshAssetLibrary();
        void importAssets(const std::vector<std::string>& asset_paths, const ImportOptions& options = ImportOptions{});
        void exportAssets(const std::vector<std::string>& asset_names, const std::string& export_path);
        void deleteAssets(const std::vector<std::string>& asset_names);

        // Search and filtering
        void setSearchFilter(const SearchFilter& filter);
        void clearSearchFilter();
        std::vector<AssetItem> getFilteredAssets() const;

        // Selection management
        void selectAsset(const std::string& asset_name);
        void selectAssets(const std::vector<std::string>& asset_names);
        void clearSelection();
        std::vector<std::string> getSelectedAssets() const;

        // View management
        void setViewMode(AssetViewMode mode);
        void setThumbnailSize(int size);
        void togglePanel(PanelType panel_type, bool show);

        // Callbacks
        void setAssetDoubleClickCallback(std::function<void(const AssetItem&)> callback);
        void setAssetRightClickCallback(std::function<void(const AssetItem&)> callback);
        void setImportCallback(std::function<void(const std::vector<std::string>&, const ImportOptions&)> callback);

    private:
        // Window and rendering
        GLFWwindow* window_;
        GUIConfig config_;
        bool initialized_;

        // Asset data
        std::vector<AssetItem> assets_;
        std::vector<AssetItem> filtered_assets_;
        std::vector<std::string> selected_assets_;
        SearchFilter current_filter_;
        AssetViewMode view_mode_;

        // Core system pointers
        std::shared_ptr<AssetManager::AssetManager> asset_manager_;
        std::shared_ptr<ImportManager::ImportManager> import_manager_;
        std::shared_ptr<MaterialManager::MaterialManager> material_manager_;
        std::shared_ptr<ImportHistory::ImportHistory> import_history_;
        std::shared_ptr<PythonBridge::PythonBridge> python_bridge_;

        // Callbacks
        std::function<void(const AssetItem&)> asset_double_click_callback_;
        std::function<void(const AssetItem&)> asset_right_click_callback_;
        std::function<void(const std::vector<std::string>&, const ImportOptions&)> import_callback_;

        // UI state
        bool show_asset_browser_ = true;
        bool show_asset_preview_ = true;
        bool show_asset_details_ = true;
        bool show_search_filter_ = true;
        bool show_import_panel_ = false;
        bool show_material_editor_ = false;
        bool show_history_panel_ = false;
        bool show_settings_panel_ = false;

        // Helper methods
        void setupTheme();
        void setupFonts();
        void setupDocking();
        void renderAssetGrid();
        void renderAssetList();
        void renderAssetDetails();
        void renderContextMenu(const AssetItem& asset);
        void renderImportOptions(ImportOptions& options);
        void renderMaterialEditor();
        void renderHistoryList();
        void renderSettings();
        void updateFilteredAssets();
        void loadThumbnails();
        void generateThumbnail(const AssetItem& asset);
        bool isAssetVisible(const AssetItem& asset) const;
        void handleDragAndDrop();
        void handleKeyboardShortcuts();
    };

    // Utility functions
    namespace GUIUtils {
        void renderAssetThumbnail(const AssetItem& asset, float size);
        void renderAssetInfo(const AssetItem& asset);
        void renderProgressBar(const std::string& label, float progress);
        void renderTooltip(const std::string& text);
        void renderConfirmationDialog(const std::string& title, const std::string& message, std::function<void()> on_confirm);
        void renderFileDialog(const std::string& title, std::string& selected_path, bool is_save = false);
        void renderColorPicker(const std::string& label, float color[4]);
        void renderVector3Input(const std::string& label, float values[3]);
        void renderComboBox(const std::string& label, const std::vector<std::string>& items, int& selected_index);
        void renderCheckbox(const std::string& label, bool& value);
        void renderSlider(const std::string& label, float& value, float min_val, float max_val);
        void renderInputText(const std::string& label, std::string& text);
        void renderButton(const std::string& label, std::function<void()> callback, bool enabled = true);
        void renderSeparator();
        void renderSpacing();
        void renderText(const std::string& text, bool disabled = false);
        void renderTextColored(const std::string& text, const ImVec4& color);
        void renderBulletText(const std::string& text);
        void renderHeader(const std::string& text);
        void renderSubHeader(const std::string& text);
        void renderCollapsingHeader(const std::string& label, std::function<void()> content);
        void renderTabBar(const std::string& label, std::function<void()> content);
        void renderGroupBox(const std::string& label, std::function<void()> content);
        void renderChildWindow(const std::string& label, const ImVec2& size, std::function<void()> content);
        void renderPopup(const std::string& label, std::function<void()> content);
        void renderModal(const std::string& label, std::function<void()> content);
        void renderMenu(const std::string& label, std::function<void()> content);
        void renderMenuItem(const std::string& label, std::function<void()> callback, bool enabled = true);
        void renderMenuSeparator();
        void renderContextMenu(std::function<void()> content);
        void renderDragDropSource(const std::string& label, const std::string& payload_type, const std::string& payload_data);
        void renderDragDropTarget(const std::string& label, const std::string& payload_type, std::function<void(const std::string&)> callback);
    };

} // namespace TahliaGUI 