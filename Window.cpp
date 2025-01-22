#include "Window.h"

namespace gps {

    void Window::Create(int width, int height, const char *title) {
        if (!glfwInit()) {
            throw std::runtime_error("Could not start GLFW3!");
        }

        //window hints
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        //window scaling for HiDPI displays
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

        //for sRBG framebuffer
        glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

        //for antialising
        glfwWindowHint(GLFW_SAMPLES, 4);

        this->window = glfwCreateWindow(width, height, title, NULL, NULL);
        if (!this->window) {
            throw std::runtime_error("Could not create GLFW3 window!");
        }

        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();

        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

        glfwSetWindowMonitor(this->window, primaryMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwMakeContextCurrent(window);

        glfwSwapInterval(1);

#if not defined (__APPLE__)
        // start GLEW extension handler
        glewExperimental = GL_TRUE;
        glewInit();
#endif
        // get version info
        const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
        const GLubyte* version = glGetString(GL_VERSION); // version as a string
        std::cout << "Renderer: " << renderer << std::endl;
        std::cout << "OpenGL version: " << version << std::endl;

        //for RETINA display
        glfwGetFramebufferSize(window, &this->dimensions.width, &this->dimensions.height);
    }

    void Window::Delete() {
        if (window)
            glfwDestroyWindow(window);
        //close GL context and any other GLFW resources
        glfwTerminate();
    }

    GLFWwindow* Window::getWindow() {
        return this->window;
    }

    WindowDimensions Window::getWindowDimensions() {
        return this->dimensions;
    }

    void Window::setWindowDimensions(WindowDimensions dimensions) {
        this->dimensions = dimensions;
    }
}
