#pragma once
#include <GLFW/glfw3.h>
class Game;

class App {
public:
    App(int width, int height, const char* title);
    ~App();

    bool init();
    void run();

private:
    GLFWwindow* window;
    int winW, winH;
    const char* winTitle;

    // tiempo/tick
    double lastTime = 0.0;
    double acc = 0.0;
    const double TICK = 1.0/12.0;

    // juego
    Game* game = nullptr;

    bool initGLFW();
    bool initGLAD();
    void logGLInfo();
    void configureBaseGLState();
    void mainLoop();

    // input
    static void keyCallback(GLFWwindow* w, int key, int scancode, int action, int mods);
    void handleKey(int key, int action);
};
