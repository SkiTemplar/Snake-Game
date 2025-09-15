#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "App.h"

// ---------------- Helpers locales (no contaminan interfaz) ----------------
namespace {
    void glfwErrorCallback(int code, const char* desc) {
        std::cerr << "[GLFW ERROR] (" << code << "): " << desc << std::endl;
    }

    void framebufferSizeCallback(GLFWwindow* window, int w, int h) {
        glViewport(0, 0, w, h);
    }

    unsigned compile(GLenum type, const char* src) {
        const unsigned s = glCreateShader(type);
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);
        int ok = 0; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[1024]; glGetShaderInfoLog(s, 1024, nullptr, log);
            std::cerr << "[GL COMPILE] " << log << "\n";
        }
        return s;
    }

    unsigned linkProg(const char* vs, const char* fs) {
        const unsigned v = compile(GL_VERTEX_SHADER,   vs);
        const unsigned f = compile(GL_FRAGMENT_SHADER, fs);
        const unsigned p = glCreateProgram();
        glAttachShader(p, v); glAttachShader(p, f);
        glLinkProgram(p);
        glDeleteShader(v); glDeleteShader(f);
        int ok = 0; glGetProgramiv(p, GL_LINK_STATUS, &ok);
        if (!ok) {
            char log[1024]; glGetProgramInfoLog(p, 1024, nullptr, log);
            std::cerr << "[GL LINK] " << log << "\n";
        }
        return p;
    }

    // @brief Matriz ortográfica column-major para viewport 2D.
    void makeOrtho(float l, float r, float b, float t, float out[16]) {
        for (int i = 0; i < 16; ++i) out[i] = 0.0f;
        out[0]  =  2.0f / (r - l);
        out[5]  =  2.0f / (t - b);
        out[10] = -1.0f;
        out[12] = -(r + l) / (r - l);
        out[13] = -(t + b) / (t - b);
        out[15] =  1.0f;
    }
} // namespace

// ---------------- App ----------------

App::App(int width, int height, const char* title) noexcept
    : winW(width), winH(height), winTitle(title) {}

App::~App() {
    // destruir en orden inverso
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

bool App::initGLFW() {
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) { std::cerr << "GLFW init falló\n"; return false; }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if defined(__APPLE__)
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    window = glfwCreateWindow(winW, winH, winTitle, nullptr, nullptr);
    if (!window) { std::cerr << "No se creó la ventana\n"; glfwTerminate(); return false; }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, App::keyCallback);
    return true;
}

bool App::initGLAD() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD init falló\n"; return false;
    }
    return true;
}

void App::logGLInfo() const {
    std::cout << "GL_VENDOR:   " << glGetString(GL_VENDOR)   << "\n";
    std::cout << "GL_RENDERER: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "GL_VERSION:  " << glGetString(GL_VERSION)  << "\n";
    std::cout << "GLSL:        " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void App::configureBaseGLState() {
    int fbW = 0, fbH = 0;
    glfwGetFramebufferSize(window, &fbW, &fbH);
    glViewport(0, 0, fbW, fbH);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.05f, 0.10f, 0.20f, 1.0f);
}

[[nodiscard]] bool App::init() {
    if (!initGLFW()) return false;
    if (!initGLAD()) return false;
    logGLInfo();
    configureBaseGLState();

    game = std::make_unique<Game>(30, 20);
    initRenderer2D(game->cols(), game->rows());

    lastTime = glfwGetTime();
    acc = 0.0;
    updateWindowTitle();
    return true;
}

void App::initRenderer2D(int gridW, int gridH) {
    // Quad 1x1 en [0,1]x[0,1]
    const float vertices[] = {
        0,0, 1,0, 1,1,
        0,0, 1,1, 0,1
    };
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // Shaders mínimos
    static const char* VS = R"(#version 330 core
    layout(location=0) in vec2 aPos;
    uniform mat4 uProj;
    uniform vec2 uCell; // traslación en coords de grilla
    void main(){
        vec2 world = aPos + uCell;
        gl_Position = uProj * vec4(world, 0.0, 1.0);
    })";

    static const char* FS = R"(#version 330 core
    out vec4 FragColor;
    uniform vec4 uColor;
    void main(){ FragColor = uColor; })";

    prog = linkProg(VS, FS);
    uProjLoc  = glGetUniformLocation(prog, "uProj");
    uCellLoc  = glGetUniformLocation(prog, "uCell");
    uColorLoc = glGetUniformLocation(prog, "uColor");

    // Proyección: [0..C]x[0..R] a NDC. Y crece hacia abajo (conveniente para tu lógica).
    makeOrtho(0.0f, (float)gridW, (float)gridH, 0.0f, proj);
}

void App::drawCell(float cx, float cy, float r, float g, float b, float a) const {
    glUseProgram(prog);
    glUniformMatrix4fv(uProjLoc, 1, GL_FALSE, proj);
    glUniform2f(uCellLoc, cx, cy);
    glUniform4f(uColorLoc, r, g, b, a);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void App::drawGrid() const {
    const int W = game->cols(), H = game->rows();
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            const bool odd = ((x + y) & 1) != 0;
            const float a = odd ? 0.12f : 0.16f;
            const float b = odd ? 0.18f : 0.22f;
            drawCell((float)x, (float)y, 0.10f + a, 0.15f + b, 0.20f + a, 1.0f);
        }
    }
}

void App::drawFrame() const {
    if (game->gameOver()) glClearColor(0.30f, 0.05f, 0.05f, 1.0f);
    else                  glClearColor(0.05f, 0.10f, 0.20f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    drawGrid();

    // Comida
    const Cell& f = game->foodCell();
    drawCell((float)f.x, (float)f.y, 1.0f, 0.3f, 0.3f, 1.0f);

    // Snake
    const auto& s = game->snake();
    for (std::size_t i = 0; i < s.size(); ++i) {
        const Cell& c = s[i];
        const bool head = (i + 1 == s.size());
        if (head) drawCell((float)c.x, (float)c.y, 0.2f, 1.0f, 0.4f, 1.0f);
        else      drawCell((float)c.x, (float)c.y, 0.2f, 0.8f, 1.0f, 1.0f);
    }
}

void App::updateWindowTitle() const {
    if (!game || !window) return;
    const char* mode = (currentBorder == Game::Border::Wrap) ? "WRAP" : "WALLS";
    char buf[160];
    std::snprintf(buf, sizeof(buf),
                  "Snake OpenGL v1.0 | SCORE: %d | %s%s",
                  game->score(), mode, game->gameOver() ? " | GAME OVER (R)" : "");
    glfwSetWindowTitle(window, buf);
}

void App::mainLoop() {
    while (!glfwWindowShouldClose(window)) {
        const double now = glfwGetTime();
        const double dt  = now - lastTime;
        lastTime = now;
        acc += dt;

        while (acc >= TICK) {
            game->tick();
            acc -= TICK;
        }

        updateWindowTitle();
        glfwPollEvents();
        drawFrame();
        glfwSwapBuffers(window);
    }
}

void App::run() {
    mainLoop();
}

// ---------------- Input ----------------

void App::keyCallback(GLFWwindow* w, int key, int /*scancode*/, int action, int /*mods*/) {
    auto* app = reinterpret_cast<App*>(glfwGetWindowUserPointer(w));
    if (app) app->handleKey(key, action);
}

void App::handleKey(int key, int action) noexcept {
    if (!game) return;
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;

    switch (key) {
        case GLFW_KEY_UP:    game->setPendingDir(Dir::Up);    break;
        case GLFW_KEY_DOWN:  game->setPendingDir(Dir::Down);  break;
        case GLFW_KEY_LEFT:  game->setPendingDir(Dir::Left);  break;
        case GLFW_KEY_RIGHT: game->setPendingDir(Dir::Right); break;
        case GLFW_KEY_R:     game->reset();                   break;
        case GLFW_KEY_M:
            currentBorder = (currentBorder == Game::Border::Wrap)
                          ? Game::Border::Walls
                          : Game::Border::Wrap;
            game->setBorderMode(currentBorder);
            updateWindowTitle();
            break;
        default: break;
    }
}
