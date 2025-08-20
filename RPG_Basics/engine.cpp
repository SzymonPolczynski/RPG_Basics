#include <iostream>
#include <limits>
#include <algorithm>
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

static bool tryMove(Player& p, Command cmd) {
    int nx = p.x, ny = p.y;
    if (cmd == Command::Up) ny -= 1;
    if (cmd == Command::Down) ny += 1;
    if (cmd == Command::Left) nx -= 1;
    if (cmd == Command::Right) nx += 1;

    if (!isWalkable(nx, ny, g_map)) {
        std::cout << "Bump! You can't go there.\n";
        return false;
    }

    p.x = nx;
    p.y = ny;

    applyTileEffect(p, g_map[p.y][p.x]);
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

    while (p.hp > 0) {
        computeVisibility(p, g_map, visible);
        for (int r = 0; r < MAP_HEIGHT; ++r)
            for (int c = 0; c < MAP_WIDTH; ++c)
                if (visible[r][c]) explored[r][c] = true;

        drawMap(p, visible, explored);

        Command cmd = readCommand();
        if (cmd == Command::Quit) {
            std::cout << "Back to menu...\n\n";
            return GameState::Menu;
        }
        if (cmd == Command::None) {
            std::cout << "Unknown command.\n\n";
            continue;
        }

        (void)tryMove(p, cmd);
        std::cout << "\n";
    }
    std::cout << "You died! Game over.\n";
    return GameState::Exit;
}
