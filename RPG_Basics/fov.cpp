#include <algorithm>
#include <cstdlib>
#include "fov.hpp"


bool blocksSight(Tile t) { return t == Tile::Wall; }

bool hasLineOfSight(const Tile map[MAP_HEIGHT][MAP_WIDTH], int x0, int y0, int x1, int y1) {
	if (!inBounds(x0, y0) || !inBounds(x1, y1)) return false;

    int dx = std::abs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = -std::abs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    int x = x0;
    int y = y0;

    while (true) {
        if (!(x == x0 && y == y0)) {
            if (blocksSight(map[y][x])) {
                if (!(x == x1 && y == y1)) return false;
            }
        }
        if (x == x1 && y == y1) break;

        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x += sx; }
        if (e2 <= dx) { err += dx; y += sy; }
    }
    return true;
}

void computeVisibility(const Player& p,
                       const Tile map[MAP_HEIGHT][MAP_WIDTH],
                       bool visible[MAP_HEIGHT][MAP_WIDTH]) {
    for (int r = 0; r < MAP_HEIGHT; ++r)
        for (int c = 0; c < MAP_WIDTH; ++c)
            visible[r][c] = false;

    int xmin = std::max(0, p.x - VISION_RADIUS);
    int xmax = std::min(MAP_WIDTH - 1, p.x + VISION_RADIUS);
    int ymin = std::max(0, p.y - VISION_RADIUS);
    int ymax = std::min(MAP_HEIGHT - 1, p.y + VISION_RADIUS);

    int r2 = VISION_RADIUS * VISION_RADIUS;

    for (int y = ymin; y <= ymax; ++y) {
        for (int x = xmin; x <= xmax; ++x) {
            int dx = x - p.x;
            int dy = y - p.y;
            if (dx * dx + dy * dy <= r2) {
                if (hasLineOfSight(map, p.x, p.y, x, y)) {
                    visible[y][x] = true;
                }
            }
        }
    }
}
