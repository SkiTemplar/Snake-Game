#pragma once
#include <deque>
#include "Types.h"

enum class Dir { Up, Down, Left, Right };

class Game {
public:
    Game(int cols, int rows);
    void reset();
    void setPendingDir(Dir d);   // setas la próxima dirección
    void tick();                 // avanza 1 paso lógico

    // getters simples
    const std::deque<Cell>& snake() const { return body; }
    Dir dir() const { return curDir; }
    bool gameOver() const { return over; }
    int cols() const { return C; }
    int rows() const { return R; }

private:
    int C, R;
    std::deque<Cell> body;
    Dir curDir, pendingDir;
    bool over;

    static bool isOpposite(Dir a, Dir b);
    Cell dirDelta(Dir d) const;
    void step();
    void wrap(Cell& c) const;   // por ahora modo wrap para probar
};
