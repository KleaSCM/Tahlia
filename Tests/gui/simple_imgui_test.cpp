/*
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: simple_imgui_test.cpp
Description: Simple ImGui test to show basic GUI functionality.
*/

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    std::cout << "🎨 Starting Simple ImGui Test..." << std::endl;
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }

    // Create window
    GLFWwindow* window = glfwCreateWindow(1200, 800, "Tahlia Asset Library - ImGui Test", nullptr, nullptr);
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

    // Setup style
    ImGui::StyleColorsDark();

    std::cout << "✅ ImGui initialized! Window should appear..." << std::endl;
    std::cout << "💡 Close the window to exit" << std::endl;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create the main window
        ImGui::Begin("Tahlia Asset Library");
        ImGui::Text("🌸 Welcome to Tahlia Asset Library!");
        ImGui::Text("✨ Universal Asset Management System");
        ImGui::Text("💕 Built with Dear ImGui by KleaSCM");
        ImGui::Separator();
        
        // Asset browser panel
        ImGui::BeginChild("Asset Browser", ImVec2(300, 200), true);
        ImGui::Text("📁 Asset Browser");
        ImGui::Separator();
        ImGui::Text("• Character_01.fbx");
        ImGui::Text("• Environment_01.blend");
        ImGui::Text("• Texture_01.png");
        ImGui::Text("• Material_01.mat");
        ImGui::Text("• Audio_01.wav");
        ImGui::EndChild();
        
        ImGui::SameLine();
        
        // Asset preview panel
        ImGui::BeginChild("Asset Preview", ImVec2(300, 200), true);
        ImGui::Text("🎨 Asset Preview");
        ImGui::Separator();
        ImGui::Text("3D Preview Area");
        ImGui::Text("(Coming soon)");
        ImGui::EndChild();
        
        ImGui::SameLine();
        
        // Asset details panel
        ImGui::BeginChild("Asset Details", ImVec2(300, 200), true);
        ImGui::Text("📋 Asset Details");
        ImGui::Separator();
        ImGui::Text("Name: Character_01.fbx");
        ImGui::Text("Type: Model");
        ImGui::Text("Size: 2.0 MB");
        ImGui::Text("Created: 2024-01-15");
        ImGui::EndChild();
        
        ImGui::Separator();
        
        // Controls
        static int counter = 0;
        if (ImGui::Button("Click me!")) {
            counter++;
        }
        ImGui::SameLine();
        ImGui::Text("Counter: %d", counter);
        
        ImGui::Separator();
        ImGui::Text("🎨 This is a working ImGui interface!");
        ImGui::Text("🚀 The full asset library is coming soon!");
        
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
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

    std::cout << "👋 ImGui test completed!" << std::endl;
    return 0;
} 