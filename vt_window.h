#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace vt {

    class VtWindow {

    public:
        VtWindow(int _width, int _height, std::string _name);
        ~VtWindow();

        VtWindow(const VtWindow&) = delete;
        VtWindow& operator = (const VtWindow&) = delete;

        bool shouldClose() { return glfwWindowShouldClose(window); }
        VkExtent2D getExtent() { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
        bool wasWindowResized() { return framebufferResized; }
        void resetWindowResizedFlag() { framebufferResized = false; }

        void createWindowSurface(VkInstance _instance, VkSurfaceKHR* _surface);

    private:
        static void framebufferResizeCallback(GLFWwindow* _window, int _width, int _height);
        void initWindow();

        int width;
        int height;
        bool framebufferResized;

        const std::string windowName;

        GLFWwindow* window;
    };
}

