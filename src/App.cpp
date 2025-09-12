#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "App.h"
#include "Game.h"
#include <iostream>

static void glfwErrorCallback(int code, const char* desc){ std::cerr << "[GLFW ERROR] ("<<code<<"): "<<desc<<std::endl; }
static void framebufferSizeCallback(GLFWwindow* window, int w, int h){ glViewport(0,0,w,h); }

App::App(int width, int height, const char* title)
    : window(nullptr), winW(width), winH(height), winTitle(title) {}

App::~App(){
    delete game;
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}

bool App::init(){
    if (!initGLFW()) return false;
    if (!initGLAD()) return false;
    logGLInfo();
    configureBaseGLState();

    // crea juego: 30x20 celdas
    game = new Game(30, 20);

    // tiempo inicial
    lastTime = glfwGetTime();
    acc = 0.0;
    return true;
}

bool App::initGLFW(){
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()){ std::cerr<<"No se pudo inicializar GLFW\n"; return false; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,GL_TRUE);

    window = glfwCreateWindow(winW, winH, winTitle, nullptr, nullptr);
    if(!window){ std::cerr<<"No se pudo crear la ventana GLFW\n"; glfwTerminate(); return false; }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // user pointer para callbacks
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, App::keyCallback);
    return true;
}

bool App::initGLAD(){
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr<<"No se pudo inicializar GLAD\n"; return false;
    }
    return true;
}

void App::logGLInfo(){
    std::cout<<"GL_VENDOR:   "<<glGetString(GL_VENDOR)<<"\n";
    std::cout<<"GL_RENDERER: "<<glGetString(GL_RENDERER)<<"\n";
    std::cout<<"GL_VERSION:  "<<glGetString(GL_VERSION)<<"\n";
    std::cout<<"GLSL:        "<<glGetString(GL_SHADING_LANGUAGE_VERSION)<<std::endl;
}

void App::configureBaseGLState(){
    int fbW, fbH; glfwGetFramebufferSize(window,&fbW,&fbH);
    glViewport(0,0,fbW,fbH);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.05f,0.1f,0.2f,1.0f);
}

void App::keyCallback(GLFWwindow* w, int key, int scancode, int action, int mods){
    auto* app = reinterpret_cast<App*>(glfwGetWindowUserPointer(w));
    if (app) app->handleKey(key, action);
}

void App::handleKey(int key, int action){
    if (!game) return;
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
    switch(key){
        case GLFW_KEY_UP:    game->setPendingDir(Dir::Up); break;
        case GLFW_KEY_DOWN:  game->setPendingDir(Dir::Down); break;
        case GLFW_KEY_LEFT:  game->setPendingDir(Dir::Left); break;
        case GLFW_KEY_RIGHT: game->setPendingDir(Dir::Right); break;
        case GLFW_KEY_R:     game->reset(); break; // reinicio rápido
        default: break;
    }
}

void App::mainLoop(){
    while(!glfwWindowShouldClose(window)){
        // tiempo y ticks
        double now = glfwGetTime();
        double dt = now - lastTime;
        lastTime = now;
        acc += dt;
        while(acc >= TICK){
            game->tick();
            acc -= TICK;
            // debug: imprime cabeza 2D una vez por tick
            const auto& s = game->snake();
            const Cell& h = s.back();
            std::cout << "Head: ("<<h.x<<","<<h.y<<")\n";
        }

        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
    }
}

void App::run(){ mainLoop(); }
