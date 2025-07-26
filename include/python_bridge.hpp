/*
 * Author: KleaSCM
 * Email: KleaSCM@gmail.com
 * Name: python_bridge.hpp
 * Description: Header file for the PythonBridge class providing elegant FFI/Python integration.
 *              Exposes C++ core functionality to Python with Blender context management.
 *              Designed for seamless integration with existing modules and flexible Python workflows.
 *
 * Architecture:
 * - Modular FFI system for C++/Python binding
 * - Blender context management with state preservation
 * - Elegant Python APIs that feel natural and intuitive
 * - Thread-safe operations for concurrent Python/C++ interactions
 * - Extensible design for new Python bindings and features
 *
 * Key Features:
 * - Expose AssetManager, ImportManager, MaterialManager to Python
 * - Blender context management (selection, viewport, mode preservation)
 * - Python-friendly APIs with proper error handling and exceptions
 * - Integration with Blender's Python API for seamless workflows
 * - Comprehensive Python module structure and documentation
 * - Extensible design for new Python bindings and features
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <any>

// Forward declarations
namespace AssetManager {
    class AssetManager;
    class ImportManager;
    class MaterialManager;
    class ImportHistory;
    struct ImportOptions;
    struct ImportResult;
    struct MaterialOptions;
    struct MaterialResult;
    struct ImportHistoryEntry;
    struct HistoryStats;
    struct UndoResult;
}

namespace PythonBridge {

/**
 * @brief Blender context state for preservation during operations
 */
struct BlenderContext {
    std::vector<std::string> selected_objects;
    std::string active_object;
    std::string view_layer;
    std::string mode;
    std::map<std::string, std::string> viewport_settings;
    std::vector<std::string> visible_collections;
    bool is_rendering;
    std::map<std::string, std::string> custom_state;
};

/**
 * @brief Python module configuration and settings
 */
struct PythonModuleConfig {
    std::string module_name;
    std::string version;
    std::string description;
    std::vector<std::string> dependencies;
    std::map<std::string, std::string> metadata;
    bool enable_debug_mode;
    bool enable_context_preservation;
    size_t max_context_stack_size;
};

/**
 * @brief Simple result structure for Python operations
 */
struct PythonResult {
    bool success;
    std::string message;
    std::map<std::string, std::string> data;
    std::vector<std::string> list_data;
};

class PythonBridge {
public:
    PythonBridge();
    ~PythonBridge();

    // Core initialization and setup
    bool initialize(const PythonModuleConfig& config = {});
    bool isInitialized() const;
    bool isPythonAvailable() const;
    void cleanup();

    // Blender context management
    BlenderContext captureContext() const;
    bool restoreContext(const BlenderContext& context);
    bool preserveContext(std::function<void()> operation);
    void pushContext();
    void popContext();
    size_t getContextStackSize() const;
    void clearContextStack();

    // C++ core integration
    void setAssetManager(std::shared_ptr<AssetManager::AssetManager> manager);
    void setImportManager(std::shared_ptr<AssetManager::ImportManager> manager);
    void setMaterialManager(std::shared_ptr<AssetManager::MaterialManager> manager);
    void setImportHistory(std::shared_ptr<AssetManager::ImportHistory> history);

    // Python-friendly wrapper methods
    PythonResult importAssetPython(const std::string& asset_path, const std::map<std::string, std::string>& options = {});
    std::vector<PythonResult> importAssetsGridPython(const std::vector<std::string>& asset_paths, const std::map<std::string, std::string>& options = {}, int rows = 1, int cols = 1, float spacing = 5.0f);
    std::vector<PythonResult> importAssetsCirclePython(const std::vector<std::string>& asset_paths, const std::map<std::string, std::string>& options = {}, float radius = 10.0f);
    std::vector<PythonResult> importAssetsLinePython(const std::vector<std::string>& asset_paths, const std::map<std::string, std::string>& options = {}, float spacing = 5.0f);
    std::vector<PythonResult> importAssetsRandomPython(const std::vector<std::string>& asset_paths, const std::map<std::string, std::string>& options = {}, int count = 10, float area_size = 20.0f);
    
    PythonResult createMaterialPython(const std::string& name, const std::map<std::string, std::string>& options = {});
    PythonResult createPBRMaterialPython(const std::string& name, const std::map<std::string, std::string>& options = {});
    PythonResult createQuickMaterialPython(const std::string& name, const std::string& preset_type = "");
    
    PythonResult undoLastImportPython();
    PythonResult undoImportPython(const std::string& entry_id);
    std::vector<PythonResult> getHistoryPython();
    PythonResult getHistoryStatsPython();

    // Utility methods for Python integration
    std::map<std::string, std::string> convertImportOptionsToMap(const AssetManager::ImportOptions& options);
    AssetManager::ImportOptions convertMapToImportOptions(const std::map<std::string, std::string>& options);
    PythonResult convertImportResultToMap(const AssetManager::ImportResult& result);
    std::map<std::string, std::string> convertMaterialOptionsToMap(const AssetManager::MaterialOptions& options);
    AssetManager::MaterialOptions convertMapToMaterialOptions(const std::map<std::string, std::string>& options);
    PythonResult convertMaterialResultToMap(const AssetManager::MaterialResult& result);
    PythonResult convertHistoryEntryToMap(const AssetManager::ImportHistoryEntry& entry);
    PythonResult convertHistoryStatsToMap(const AssetManager::HistoryStats& stats);
    PythonResult convertUndoResultToMap(const AssetManager::UndoResult& result);

    // Error handling and reporting
    void setPythonExceptionHandler(std::function<void(const std::string&)> handler);
    std::string getLastError() const;
    void clearLastError();
    bool hasError() const;

    // Configuration and settings
    void setDebugMode(bool enable);
    void setContextPreservation(bool enable);
    void setMaxContextStackSize(size_t max_size);
    PythonModuleConfig getConfig() const;

private:
    std::shared_ptr<AssetManager::AssetManager> asset_manager_;
    std::shared_ptr<AssetManager::ImportManager> import_manager_;
    std::shared_ptr<AssetManager::MaterialManager> material_manager_;
    std::shared_ptr<AssetManager::ImportHistory> import_history_;
    
    PythonModuleConfig config_;
    std::vector<BlenderContext> context_stack_;
    std::function<void(const std::string&)> exception_handler_;
    std::string last_error_;
    bool initialized_;
    bool python_available_;
    
    // Internal helpers
    void handlePythonException(const std::exception& e) const;
    bool checkPythonAvailability();
    void captureBlenderState(BlenderContext& context) const;
    void restoreBlenderState(const BlenderContext& context);
    
public:
    // Helper methods for testing
    PythonResult createErrorResult(const std::string& error_message);
    PythonResult createSuccessResult(const std::string& message = "Success");
};

} // namespace PythonBridge 