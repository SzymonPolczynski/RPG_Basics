#pragma once
#include "config.hpp"
#include "types.hpp"


bool blocksSight(Tile t);
bool hasLineOfSight(const Tile map[MAP_HEIGHT][MAP_WIDTH], int x0, int y0, int x1, int y1);
void computeVisibility(const Player& p,
					   const Tile map[MAP_HEIGHT][MAP_WIDTH],
					   bool visible[MAP_HEIGHT][MAP_WIDTH]);
