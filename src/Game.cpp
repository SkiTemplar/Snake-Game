#include "Game.h"

Game::Game(int cols, int rows) : C(cols), R(rows) { reset(); }

void Game::reset(){
    body.clear();
    // cola de 3 celdas centrada y mirando a la derecha
    int cx = C/2, cy = R/2;
    body.push_back({cx-2, cy});
    body.push_back({cx-1, cy});
    body.push_back({cx,   cy});
    curDir = Dir::Right;
    pendingDir = Dir::Right;
    over = false;
}

void Game::setPendingDir(Dir d){
    // bloquea giro 180º en el mismo tick
    if (!isOpposite(d, curDir)) pendingDir = d;
}

bool Game::isOpposite(Dir a, Dir b){
    return (a==Dir::Up && b==Dir::Down) || (a==Dir::Down && b==Dir::Up) ||
           (a==Dir::Left && b==Dir::Right) || (a==Dir::Right && b==Dir::Left);
}

Cell Game::dirDelta(Dir d) const {
    switch(d){
        case Dir::Up:    return {0,-1};
        case Dir::Down:  return {0, 1};
        case Dir::Left:  return {-1,0};
        case Dir::Right: return {1, 0};
    }
    return {0,0};
}

void Game::wrap(Cell& c) const {
    if (c.x < 0)     c.x = C-1;
    if (c.x >= C)    c.x = 0;
    if (c.y < 0)     c.y = R-1;
    if (c.y >= R)    c.y = 0;
}

void Game::step(){
    // aplica dirección pendiente y avanza cabeza
    curDir = pendingDir;
    Cell head = body.back();
    Cell d = dirDelta(curDir);
    head.x += d.x; head.y += d.y;
    wrap(head);                 // por ahora sin morir en pared

    body.push_back(head);       // avanza
    body.pop_front();           // sin crecimiento aún
}

void Game::tick(){
    if (over) return;
    step();
    // TODO: colisión consigo mismo y comida en módulos siguientes
}
