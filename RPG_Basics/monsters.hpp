#pragma once
#include "types.hpp"
#include "map.hpp"


void spawnMonster(Monsters& mons, int count, const Tile map[MAP_HEIGHT][MAP_WIDTH]);
bool playerBumpAttackOrMove(Player& p, Monsters& mons, Command cmd, std::string& outStatus, Tile map[MAP_HEIGHT][MAP_WIDTH]);
void monstersTurn(Monsters& mons, Player& p, const Tile map[MAP_HEIGHT][MAP_WIDTH]);
bool isMonsterAt(const Monsters& mons, int x, int y, const Monster** outPtr = nullptr);
char monsterGlyphAt(const Monsters& mons, int x, int y, bool isVisible, bool isExplored);
