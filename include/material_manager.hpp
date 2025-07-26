/**
 * @file material_manager.hpp
 * @author KleaSCM
 * @email KleaSCM@gmail.com
 * @brief Header for MaterialManager class providing comprehensive material handling capabilities
 * 
 * This module provides advanced material creation, management, and assignment functionality
 * for 3D assets. Supports PBR materials, texture loading, and automated material workflows.
 * 
 * Architecture:
 * - Modular material system with extensible material types and workflows
 * - Integration with AssetManager and AssetIndexer for texture discovery
 * - Configurable material creation with preset support
 * - Texture loading and assignment with automatic format detection
 * - PBR material creation with multiple map support (albedo, normal, roughness, etc.)
 * - Material validation and optimization
 * - Thread-safe operations for concurrent material processing
 * 
 * Key Features:
 * - Create PBR materials with full parameter control
 * - Load and assign textures with automatic format detection
 * - Quick material setup with presets (metal, plastic, fabric, etc.)
 * - Auto-assign materials based on asset type and naming conventions
 * - Material validation and optimization
 * - Integration with Blender material system
 * - Support for multiple texture formats (PNG, JPG, TGA, EXR, etc.)
 * - Material library management and reuse
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <tuple>
#include <any>
#include <filesystem>

namespace AssetManager {

/**
 * @brief Material creation options and parameters
 */
struct MaterialOptions {
    std::string name;
    std::string preset_type;  // "metal", "plastic", "fabric", "glass", "custom"
    
    // PBR parameters
    float metallic = 0.0f;
    float roughness = 0.5f;
    float subsurface = 0.0f;
    float subsurface_radius = 1.0f;
    float subsurface_color[3] = {1.0f, 1.0f, 1.0f};
    float specular = 0.5f;
    float clearcoat = 0.0f;
    float clearcoat_roughness = 0.0f;
    float ior = 1.45f;
    float transmission = 0.0f;
    float transmission_roughness = 0.0f;
    float emission_strength = 0.0f;
    float emission_color[3] = {1.0f, 1.0f, 1.0f};
    float alpha = 1.0f;
    
    // Texture paths
    std::string albedo_texture;
    std::string normal_texture;
    std::string roughness_texture;
    std::string metallic_texture;
    std::string ao_texture;
    std::string emission_texture;
    std::string displacement_texture;
    
    // Material settings
    bool use_nodes = true;
    bool auto_smooth = true;
    bool backface_culling = false;
    bool blend_method = false;  // true for alpha blend
    
    // Advanced options
    std::map<std::string, std::any> custom_properties;
};

/**
 * @brief Result of material creation or assignment operations
 */
struct MaterialResult {
    std::string material_name;
    bool success;
    std::string message;
    std::vector<std::string> created_materials;
    std::vector<std::string> assigned_textures;
    std::map<std::string, std::any> metadata;
};

/**
 * @brief Texture information and metadata
 */
struct TextureInfo {
    std::string path;
    std::string format;
    int width;
    int height;
    int channels;
    bool is_hdr;
    std::map<std::string, std::any> metadata;
};

/**
 * @brief Material preset definition
 */
struct MaterialPreset {
    std::string name;
    std::string description;
    MaterialOptions options;
    std::vector<std::string> tags;
};

/**
 * @brief MaterialManager class for comprehensive material handling
 */
class MaterialManager {
public:
    MaterialManager();
    ~MaterialManager();

    // Material creation
    MaterialResult createMaterial(const MaterialOptions& options);
    MaterialResult createPBRMaterial(const std::string& name, const MaterialOptions& options = {});
    MaterialResult createQuickMaterial(const std::string& name, const std::string& preset_type);

    // Texture handling
    TextureInfo loadTexture(const std::string& texture_path);
    MaterialResult assignTexture(const std::string& material_name, const std::string& texture_path, const std::string& texture_type);
    std::vector<TextureInfo> discoverTextures(const std::string& directory_path);

    // Material presets
    std::vector<MaterialPreset> getAvailablePresets() const;
    MaterialResult applyPreset(const std::string& material_name, const std::string& preset_name);
    void addCustomPreset(const MaterialPreset& preset);

    // Material management
    MaterialResult autoAssignMaterials(const std::string& asset_path);
    MaterialResult validateMaterial(const std::string& material_name);
    MaterialResult optimizeMaterial(const std::string& material_name);

    // Utility functions
    std::vector<std::string> getSupportedTextureFormats() const;
    bool isTextureFormatSupported(const std::string& format) const;
    std::string detectTextureType(const std::string& texture_path) const;

    // Integration points
    void setAssetManager(std::shared_ptr<class AssetManager> manager);

private:
    std::shared_ptr<class AssetManager> asset_manager_;
    std::map<std::string, MaterialPreset> material_presets_;
    
    // Internal helpers
    void initializeDefaultPresets();
    MaterialOptions getPresetOptions(const std::string& preset_name) const;
    std::string generateMaterialName(const std::string& base_name) const;
    bool validateTexturePath(const std::string& texture_path) const;
    std::string getTextureFormat(const std::string& texture_path) const;
};

} // namespace AssetManager 