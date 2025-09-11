//
// Created by Rodrigo on 11/09/2025.
//

#include "App.h"

//Callbacks
static void glfwErrorCallback(int error, const char* description) {
    std::cerr << "GLFW error: " << description << std::endl;
}

static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

//Constructor

App::App(int width, int height, const char* title) : window(nullptr), winW(width), winH(height), winTitle(title) {}

App::~App() {
    if (window) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}
bool App::init() {
    if (!initGLFW()) {
        return false;
    }
    if (!initGLAD()) {
        return false;
    }
    logGLInfo();
    configureBaseGLState();
    return true;
}
bool App::initGLFW() {
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#endif
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    window = glfwCreateWindow(winW,winH,winTitle, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    return true;
}

bool App::initGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    return true;
}

void App::logGLInfo() {
    std::cout << "GL VERSION: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL VERSION: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "GL VENDOR: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "GL RENDERER: " << glGetString(GL_RENDERER) << std::endl;
}

void App::configureBaseGLState() {
    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    glViewport(0, 0, fbW, fbH);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.05f, 0.1f, 0.2f, 1.0f);
}

void App::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
    }
}
void App::run() {
    mainLoop();
}