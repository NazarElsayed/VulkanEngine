#include "vt_window.h"

//std
#include <stdexcept>

namespace vt {

    VtWindow::VtWindow(int _width, int _height, std::string _name) : width{ _width }, height{ _height }, windowName{ _name } {
        initWindow();
    }

    VtWindow::~VtWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void VtWindow::framebufferResizeCallback(GLFWwindow* _window, int _width, int _height) {
        auto vtWindow = reinterpret_cast<VtWindow*>(glfwGetWindowUserPointer(_window));
        vtWindow->framebufferResized = true;
        vtWindow->width = _width;
        vtWindow->height = _height;
    }

    void VtWindow::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    void VtWindow::createWindowSurface(VkInstance _instance, VkSurfaceKHR* _surface) {

        if (glfwCreateWindowSurface(_instance, window, nullptr, _surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface");
        }
    }
}