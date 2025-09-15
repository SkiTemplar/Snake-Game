#pragma once
#include <memory>
#include <cstdio>     // snprintf
#include "Game.h"

// Forward-declare evita incluir GLFW en el header.
struct GLFWwindow;

/**
 * @brief Capa de aplicación: ventana, contexto OpenGL, bucle principal y render 2D.
 *
 * Responsabilidades:
 *  - Inicializar GLFW y GLAD.
 *  - Configurar estado base de OpenGL (2D).
 *  - Gestionar input y temporización con timestep fijo.
 *  - Renderizar rejilla, comida y serpiente.
 */
class App {
public:
    /// @brief Construye la app con tamaño de ventana y título.
    App(int width, int height, const char* title) noexcept;
    /// @brief Libera recursos de ventana/contexto.
    ~App();

    /// @brief Inicializa ventana, contexto y renderer.
    /// @return true si todo OK.
    [[nodiscard]] bool init();

    /// @brief Entra en el bucle principal. Retorna al cerrar la ventana.
    void run();

private:
    // --- Ventana y configuración ---
    GLFWwindow* window = nullptr; ///< @brief Puntero a ventana GLFW.
    int  winW;
    int  winH;
    const char* winTitle;

    // --- Timestep fijo ---
    double lastTime = 0.0;  ///< @brief Marca anterior.
    double acc      = 0.0;  ///< @brief Acumulador.
    static constexpr double TICK = 1.0 / 12.0; ///< @brief 12 Hz lógicos.

    // --- Juego ---
    std::unique_ptr<Game> game;              ///< @brief Lógica de Snake.
    Game::Border currentBorder = Game::Border::Wrap; ///< @brief Modo actual.

    // --- Render (programa y geometría de celda 1x1) ---
    unsigned int prog = 0, vao = 0, vbo = 0;
    int uProjLoc = -1, uCellLoc = -1, uColorLoc = -1;
    float proj[16]{}; ///< @brief Matriz ortográfica column-major.

    // --- Arranque OpenGL/GLFW ---
    bool initGLFW();
    bool initGLAD();
    void logGLInfo() const;
    void configureBaseGLState();

    // --- Bucle y dibujo ---
    void mainLoop();
    void initRenderer2D(int gridW, int gridH);
    void drawCell(float cx, float cy, float r, float g, float b, float a) const;
    void drawGrid() const;
    void drawFrame() const;
    void updateWindowTitle() const;

    // --- Input ---
    static void keyCallback(GLFWwindow* w, int key, int scancode, int action, int mods);
    void handleKey(int key, int action) noexcept;
};
