#!/bin/bash

echo "🎨 Building Blender Asset Manager C++ Core with Zig..."
echo "Author: KleaSCM"
echo "Email: KleaSCM@gmail.com"
echo "====================================="

# Check if nlohmann/json is available
if ! pkg-config --exists nlohmann_json; then
    echo "⚠️  nlohmann/json not found via pkg-config, using header-only version"
    JSON_INCLUDE=""
else
    JSON_INCLUDE="$(pkg-config --cflags nlohmann_json)"
fi

# Build flags
CXXFLAGS="-std=c++17 -Wall -Wextra -O2 -I include"
LDFLAGS=""

# Source files
SOURCES=(
    "src/main.cpp"
    "src/core/asset_manager.cpp"
    "src/core/asset_indexer.cpp"
)

# Build command
echo "🔧 Compiling with Zig C++..."
zig c++ $CXXFLAGS $JSON_INCLUDE "${SOURCES[@]}" $LDFLAGS -o blender_asset_manager

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    echo "🚀 Running the asset manager..."
    echo ""
    ./blender_asset_manager
else
    echo "❌ Build failed!"
    exit 1
fi 