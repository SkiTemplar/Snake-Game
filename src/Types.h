#pragma once
struct Cell { int x, y; };
inline bool operator==(const Cell& a, const Cell& b){ return a.x==b.x && a.y==b.y; }
