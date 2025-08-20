#pragma once
#include <iostream>
#include "config.hpp"
#include "types.hpp"


// Global map accesed by many files (definition in map.cpp)
extern Tile g_map[MAP_HEIGHT][MAP_WIDTH];

bool isWalkable(int nx, int ny, const Tile map[MAP_HEIGHT][MAP_WIDTH]);
bool applyTileEffect(Player& p, Tile t);
char glyphFor(Tile t, bool isVisible, bool isExplored);
