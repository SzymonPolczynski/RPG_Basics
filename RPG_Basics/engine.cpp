#include <iostream>
#include <limits>
#include <algorithm>
#include <utility>
#include <string>
#include "engine.hpp"
#include "input.hpp"
#include "render.hpp"
#include "map.hpp"
#include "fov.hpp"


static void greet(Player& p) {
    std::cout << "Welcome to C++ RPG!\n";
    std::cout << "Enter your hero name: ";
    std::getline(std::cin, p.name);
    if (p.name.empty()) p.name = "Hero";
    std::cout << "Greetings, " << p.name << "! Your journey begins...\n";
}

static std::pair<int, int> deltaFor(Command cmd) {
    switch (cmd) {
	    case Command::Up: return { 0, -1 };
	    case Command::Down: return { 0, 1 };
	    case Command::Left: return { -1, 0 };
	    case Command::Right: return { 1, 0 };
	    default: return { 0, 0 };
    }
}

static bool tryMove(Player& p, Command cmd) {
	auto [dx, dy] = deltaFor(cmd);
	int nx = p.x + dx;
	int ny = p.y + dy;

    if (!isWalkable(nx, ny, g_map)) {
        std::cout << "Bump! You can't go there.\n";
        return false;
    }

    Tile& cell = g_map[ny][nx];

    p.x = nx;
    p.y = ny;

    applyTileEffect(p, cell);

    if (cell == Tile::Trap) {
        cell == Tile::Floor;
    }
    return true;
}

GameState runMenu(Player& p) {
    while (true) {
        std::cout << "====================\n";
        std::cout << "   ASCII RPG (C++)  \n";
        std::cout << "====================\n";
        std::cout << "1) Start Game\n";
        std::cout << "2) Quit\n";
        std::cout << "Choose: ";

        char choice{};
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (choice == '1') {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            p = Player{};
            greet(p);
            return GameState::Play;
        }
        else if (choice == '2' || choice == 'q' || choice == 'Q') {
            return GameState::Exit;
        }
        else {
            std::cout << "Unknown option. \n\n";
        }
    }
}

GameState runPlay(Player& p) {
    bool visible[MAP_HEIGHT][MAP_WIDTH] = {};
    bool explored[MAP_HEIGHT][MAP_WIDTH] = {};
    std::string lastMessage = "Ready.";

    while (p.hp > 0) {
        computeVisibility(p, g_map, visible);
        for (int r = 0; r < MAP_HEIGHT; ++r)
            for (int c = 0; c < MAP_WIDTH; ++c)
                if (visible[r][c]) explored[r][c] = true;

        drawMap(p, visible, explored, lastMessage);

        Command cmd = readCommand();
        if (cmd == Command::Quit) {
            lastMessage = "Back to menu...";
            return GameState::Menu;
        }
        if (cmd == Command::None) {
            lastMessage = "Unknown command.";
            continue;
        }

        bool moved = tryMove(p, cmd);
        if (!moved) {
            lastMessage = "Bump! You can't go there.";
        }
        else if (g_map[p.y][p.x] == Tile::Floor) {
            lastMessage = "Moved.";
        }
        else {
            lastMessage = "Action resolved.";
        }
    }
    std::cout << "You died! Game over.\n";
    return GameState::Exit;
}
