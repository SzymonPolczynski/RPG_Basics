#pragma once
#include <string>
#include <array>
#include <vector>
#include <optional>
#include "config.hpp"


enum class Command { Up, Down, Left, Right, Quit, Inventory, None };

enum class Tile { Floor = 0, Wall = 1, Trap = 2 };

enum class GameState { Menu, Play, Exit };

enum class Slot { Head, Chest, Weapon, Shield, Boots, Ring, Amulet, Count };

struct Item {
    std::string name;
    Slot slot;
    int dmg = 0;
    int armor = 0;
};

struct Inventory {
    std::array<std::optional<Item>, static_cast<size_t>(Slot::Count)> equipped;
    std::vector<Item> backpack;
};

struct Player {
    std::string name{};
    int x{ MAP_WIDTH / 2 };
    int y{ MAP_HEIGHT / 2 };
    int hp{ 10 };
    Inventory inv;
};
