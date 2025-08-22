#pragma once
#include <string>
#include "config.hpp"
#include "types.hpp"

struct Monsters;

void renderInit();
void renderShutdown();

void drawMap(const Player& p,
			 const bool visible[MAP_HEIGHT][MAP_WIDTH],
			 const bool explored[MAP_HEIGHT][MAP_WIDTH],
			 const std::string& status,
			 const Monsters& mons);

void drawInventory(const Player& p,
				   int cursor,
				   const std::string& status);
