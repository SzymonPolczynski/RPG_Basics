#pragma once


constexpr int MAP_WIDTH = 10;
constexpr int MAP_HEIGHT = 6;
constexpr int VISION_RADIUS = 4;

inline constexpr int MONSTER_SIGHT = 8;
inline constexpr int MAX_MONSTERS = 6;

inline bool inBounds(int x, int y) noexcept {
	return (x >= 0 && x < MAP_WIDTH) && (y >= 0 && y < MAP_HEIGHT);
}
