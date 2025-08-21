#pragma once
#include <string>
#include "config.hpp"


enum class Command { Up, Down, Left, Right, Quit, None };
enum class Tile { Floor = 0, Wall = 1, Trap = 2 };
enum class GameState { Menu, Play, Exit };

struct Player {
    std::string name{};
    int x{ MAP_WIDTH / 2 };
    int y{ MAP_HEIGHT / 2 };
    int hp{ 10 };
};
