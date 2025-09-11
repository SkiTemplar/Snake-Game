//
// Created by Rodrigo on 11/09/2025.
//

#ifndef SNAKE_APP_H
#define SNAKE_APP_H
#pragma once
#include "config.h"
#include <iostream>


class App {
public:
    App(int width, int height, const char *title);

    ~App();

    bool init(); //Inicializar GLFW y GLAD
    void run(); //Bucle principal
private:
    GLFWwindow* window;
    int winW, winH;
    const char* winTitle;

    bool initGLFW();
    bool initGLAD();
    void logGLInfo();
    void configureBaseGLState();
    void mainLoop();
};


#endif //SNAKE_APP_H