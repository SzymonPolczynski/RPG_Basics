#include <iostream>
#include <sstream>
#include "render.hpp"
#include "map.hpp"


void drawMap(const Player& p,
             const bool visible[MAP_HEIGHT][MAP_WIDTH],
             const bool explored[MAP_HEIGHT][MAP_WIDTH]) {
    std::ostringstream out;
    for (int row = 0; row < MAP_HEIGHT; ++row) {
        for (int col = 0; col < MAP_WIDTH; ++col) {
            if (row == p.y && col == p.x) out << '@';
            else out << glyphFor(g_map[row][col], visible[row][col], explored[row][col]);
        }
        out << '\n';
    }
    out << "HP: " << p.hp << "   (W/A/S/D to move, Q to quit)\n";
    std::cout << out.str();
}
