#include "Game.h"
#include <cstdlib> // rand

Game::Game(int cols, int rows) : C(cols), R(rows) {
    reset();
}

void Game::reset() {
    body.clear();
    const int cx = C / 2, cy = R / 2;
    body.push_back({cx - 2, cy});
    body.push_back({cx - 1, cy});
    body.push_back({cx,     cy});
    curDir = pendingDir = Dir::Right;
    over = false;
    points = 0;
    spawnFood();
}

void Game::setPendingDir(Dir d) noexcept {
    if (!isOpposite(d, curDir)) pendingDir = d;
}

bool Game::isOpposite(Dir a, Dir b) noexcept {
    return  (a == Dir::Up    && b == Dir::Down) ||
            (a == Dir::Down  && b == Dir::Up)   ||
            (a == Dir::Left  && b == Dir::Right)||
            (a == Dir::Right && b == Dir::Left);
}

Cell Game::dirDelta(Dir d) const noexcept {
    switch (d) {
        case Dir::Up:    return { 0,-1};
        case Dir::Down:  return { 0, 1};
        case Dir::Left:  return {-1, 0};
        case Dir::Right: return { 1, 0};
    }
    return {0,0};
}

void Game::wrap(Cell& c) const noexcept {
    if (c.x < 0)   c.x = C - 1;
    if (c.x >= C)  c.x = 0;
    if (c.y < 0)   c.y = R - 1;
    if (c.y >= R)  c.y = 0;
}

bool Game::outOfBounds(const Cell& c) const noexcept {
    return c.x < 0 || c.x >= C || c.y < 0 || c.y >= R;
}

bool Game::occupies(const Cell& c) const noexcept {
    for (const auto& s : body) if (s == c) return true;
    return false;
}

Cell Game::nextHead() const noexcept {
    Cell h = body.back();
    const Cell d = dirDelta(pendingDir);
    h.x += d.x; h.y += d.y;
    if (borderMode == Border::Wrap) wrap(h);
    return h;
}

void Game::spawnFood() {
    // Simple y suficiente para grid mediana.
    for (;;) {
        Cell f{ std::rand() % C, std::rand() % R };
        if (!occupies(f)) { food = f; return; }
    }
}

void Game::tick() {
    if (over) return;

    const Cell h = nextHead();
    if (borderMode == Border::Walls && outOfBounds(h)) { over = true; return; }

    const bool grow = (h == food);
    const std::size_t limit = body.size() - (grow ? 0 : 1); // moverte a la antigua cola es legal si no creces
    for (std::size_t i = 0; i < limit; ++i) {
        if (body[i] == h) { over = true; return; }
    }

    curDir = pendingDir;
    body.push_back(h);
    if (!grow) body.pop_front();
    else { ++points; spawnFood(); }
}
