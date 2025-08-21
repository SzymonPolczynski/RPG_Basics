#include "map.hpp"


// 0 = floor, 1 = wall, 2 = trap
Tile g_map[MAP_HEIGHT][MAP_WIDTH] = {
    {Tile::Floor, Tile::Floor, Tile::Wall, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Wall, Tile::Floor, Tile::Floor},
    {Tile::Floor, Tile::Floor, Tile::Wall, Tile::Floor, Tile::Trap, Tile::Floor, Tile::Floor, Tile::Wall, Tile::Floor, Tile::Trap},
    {Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor},
    {Tile::Wall, Tile::Wall, Tile::Floor, Tile::Wall, Tile::Floor, Tile::Floor, Tile::Wall, Tile::Floor, Tile::Wall, Tile::Floor},
    {Tile::Floor, Tile::Floor, Tile::Floor, Tile::Trap, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Trap, Tile::Floor},
    (Tile::Floor, Tile::Floor, Tile::Wall, Tile::Floor, Tile::Trap, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Trap)
};

bool isWalkable(int nx, int ny, const Tile map[MAP_HEIGHT][MAP_WIDTH]) {
    if (!inBounds(nx, ny)) return false;
    return map[ny][nx] != Tile::Wall;
}

bool applyTileEffect(Player& p, Tile t) {
    if (t == Tile::Trap) {
        p.hp -= 1;
        std::cout << "Ouch! You stepped on a trap. HP now: " << p.hp << "\n";
        return true;
    }
    return false;
}

char glyphFor(Tile t, bool isVisible, bool isExplored) {
    if (!isExplored) return ' ';
    if (isVisible) {
        switch (t) {
            case Tile::Floor: return '.';
            case Tile::Wall:  return '#';
            case Tile::Trap:  return 'T';
        }
    }
    else {
        switch (t) {
            case Tile::Floor: return ',';
            case Tile::Wall:  return '+';
            case Tile::Trap:  return '^';
        }
    }
    return ' ';
}
