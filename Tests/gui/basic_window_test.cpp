/*
Author: KleaSCM
Email: KleaSCM@gmail.com
Name: basic_window_test.cpp
Description: Basic window test using GLFW to show GUI capability.
*/

#include <GLFW/glfw3.h>
#include <iostream>

int main() {
    std::cout << "🎨 Starting Basic Window Test..." << std::endl;
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Tahlia Asset Library - Basic Window Test", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    std::cout << "✅ Window created! You should see a window appear..." << std::endl;
    std::cout << "💡 Close the window to exit" << std::endl;
    std::cout << "🎨 This proves we can create GUI windows!" << std::endl;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();

    std::cout << "👋 Basic window test completed!" << std::endl;
    std::cout << "✨ We can create GUI windows! ImGui integration coming soon!" << std::endl;
    return 0;
} 