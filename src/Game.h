#pragma once
#include <deque>
#include "Types.h"

/**
 * @brief Direcciones cardinales de movimiento.
 */
enum class Dir { Up, Down, Left, Right };

/**
 * @brief Lógica pura de Snake sobre una grilla CxR, sin dependencias de OpenGL.
 *
 * Responsabilidades:
 *  - Avance con paso fijo (tick).
 *  - Gestión de crecimiento, comida y colisiones.
 *  - Modos de borde (wrap / walls).
 */
class Game {
public:
    /// @brief Modo de borde del tablero.
    enum class Border { Wrap, Walls };

    /**
     * @brief Construye el juego para una grilla de cols x rows.
     * @param cols Columnas (C > 0)
     * @param rows Filas (R > 0)
     */
    Game(int cols, int rows);

    /// @brief Estado inicial: serpiente de 3, dirección derecha, puntuación 0 y comida nueva.
    void reset();

    /// @brief Solicita cambio de dirección (se aplica al inicio del próximo tick si no es 180º).
    void setPendingDir(Dir d) noexcept;

    /// @brief Avanza un paso lógico: aplica dirección, mueve, crece y evalúa colisiones.
    void tick();

    // --- Consultas (O(1)) ---
    /// @brief Cuerpo completo (cabeza = back()).
    const std::deque<Cell>& snake() const noexcept { return body; }
    /// @brief Dirección actual aplicada.
    Dir dir() const noexcept { return curDir; }
    /// @brief Indicador de fin de juego.
    bool gameOver() const noexcept { return over; }
    /// @brief Tamaño en columnas.
    int cols() const noexcept { return C; }
    /// @brief Tamaño en filas.
    int rows() const noexcept { return R; }
    /// @brief Celda de la comida actual.
    const Cell& foodCell() const noexcept { return food; }
    /// @brief Puntuación actual (nº de comidas).
    int score() const noexcept { return points; }

    /// @brief Fija el modo de borde (wrap/walls).
    void setBorderMode(Border m) noexcept { borderMode = m; }
    /// @brief Recupera el modo de borde.
    Border borderModeMode() const noexcept { return borderMode; }

private:
    // --- Estado invariante de tablero ---
    int C;                  ///< @brief Columnas.
    int R;                  ///< @brief Filas.

    // --- Estado dinámico de juego ---
    std::deque<Cell> body;  ///< @brief Cuerpo: cola=front(), cabeza=back().
    Cell food{};            ///< @brief Posición de la comida.
    Dir curDir{};           ///< @brief Dirección aplicada.
    Dir pendingDir{};       ///< @brief Dirección solicitada (se valida por tick).
    bool over = false;      ///< @brief Fin de juego.
    int points = 0;         ///< @brief Puntuación.
    Border borderMode = Border::Wrap; ///< @brief Modo de borde.

    // --- Utilidades internas ---
    /// @brief ¿Son opuestas? (bloquea giro 180º).
    static bool isOpposite(Dir a, Dir b) noexcept;

    /// @brief Delta unitario para una dirección.
    Cell dirDelta(Dir d) const noexcept;

    /// @brief Aplica wrap-around sobre una celda.
    void wrap(Cell& c) const noexcept;

    /// @brief ¿Está fuera de límites?
    bool outOfBounds(const Cell& c) const noexcept;

    /// @brief ¿La serpiente ocupa la celda?
    bool occupies(const Cell& c) const noexcept;

    /// @brief Nueva cabeza según pendingDir y modo de borde.
    Cell nextHead() const noexcept;

    /// @brief Genera comida en celda libre.
    void spawnFood();
};
