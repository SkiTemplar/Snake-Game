#pragma once
/**
 * @brief Tipos básicos del juego en coordenadas de grilla discreta.
 */

/// @brief Celda lógica (columna x, fila y) en el tablero.
struct Cell {
    int x;
    int y;
};

/// @brief Igualdad estructural de celdas (O(1)).
inline bool operator==(const Cell& a, const Cell& b) noexcept {
    return a.x == b.x && a.y == b.y;
}
