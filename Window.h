#ifndef WINDOW_H
#define WINDOW_H

#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <iostream>

struct WindowDimensions {
    int width;
    int height;
};

namespace gps {

    class Window {

    public:
        void Create(int width=800, int height=600, const char *title="OpenGL Project");
        void Delete();

        GLFWwindow* getWindow();
        WindowDimensions getWindowDimensions();
        void setWindowDimensions(WindowDimensions dimensions);

    private:
        WindowDimensions dimensions;
        GLFWwindow *window;
    };
}

#endif //WINDOW_H
