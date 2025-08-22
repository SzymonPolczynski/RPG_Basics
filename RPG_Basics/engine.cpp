#include <iostream>
#include <limits>
#include <algorithm>
#include <utility>
#include <string>
#include <conio.h>
#include "engine.hpp"
#include "input.hpp"
#include "render.hpp"
#include "map.hpp"
#include "fov.hpp"
#include "monsters.hpp"
#include "monsters_db.hpp"

static void greet(Player& p) {
    std::cout << "Welcome to C++ RPG!\n";
    std::cout << "Enter your hero name: ";
    std::getline(std::cin, p.name);
    if (p.name.empty()) p.name = "Hero";
    std::cout << "Greetings, " << p.name << "! Your journey begins...\n";

    // SAMPLE ITEMS ---> TODO: DELETE LATER
    p.inv.backpack.push_back(Item{ "Rusty Sword", Slot::Weapon, 1, 0 });
    p.inv.backpack.push_back(Item{ "Wooden Shield", Slot::Shield, 0, 1 });
    p.inv.backpack.push_back(Item{ "Cloth Hat", Slot::Head, 0, 1 });

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
	const int nx = p.x + dx;
	const int ny = p.y + dy;

    if (!isWalkable(nx, ny, g_map)) {
        std::cout << "Bump! You can't go there.\n";
        return false;
    }

    Tile& cell = g_map[ny][nx];

    p.x = nx;
    p.y = ny;

    const bool wasTrap = (cell == Tile::Trap);

    applyTileEffect(p, cell);

    if (wasTrap) {
        cell = Tile::Floor;
    }
    return true;
}

static size_t toIndex(Slot s) { return static_cast<size_t>(s); }

static std::string equipFromBackpack(Player& p, int backpackIndex) {
    if (backpackIndex < 0 || backpackIndex >= static_cast<int>(p.inv.backpack.size()))
        return "No item at that index.";

    Item it = p.inv.backpack[static_cast<size_t>(backpackIndex)];
    size_t idx = toIndex(it.slot);

    if (p.inv.equipped[idx]) {
        p.inv.backpack.push_back(*p.inv.equipped[idx]);
    }

    p.inv.equipped[idx] = it;

    p.inv.backpack.erase(p.inv.backpack.begin() + backpackIndex);

    return "Equipped: " + it.name;
}

static std::string uneqipSlot(Player& p, int slotNumber) {
    if (slotNumber < 0 || slotNumber >= static_cast<int>(Slot::Count))
        return "No such slot.";
    size_t idx = static_cast<size_t>(slotNumber);
    if (!p.inv.equipped[idx]) return "Slot already empty.";

    Item it = *p.inv.equipped[idx];
    p.inv.equipped[idx].reset();
    p.inv.backpack.push_back(it);
    return "Unequipped: " + it.name;
}

static void inventoryLoop(Player& p) {
    int cursor = 0;
    std::string status = "Inventory open.";

    auto clampCursor = [&]() {
        if (p.inv.backpack.empty()) { cursor = 0; return; }
        if (cursor < 0) cursor = 0;
        if (cursor >= static_cast<int>(p.inv.backpack.size()))
            cursor = static_cast<int>(p.inv.backpack.size()) - 1;
        };
    clampCursor();

    for (;;) {
        drawInventory(p, cursor, status);
        int ch = _getch();
        if (ch == 224) {
            int k = _getch();
            if (k == 72 || k == 75) { cursor--; clampCursor(); }
            else if (k == 80 || k == 77) { cursor++; clampCursor(); }
            continue;
        }
        switch (ch) {
            case 'w': case 'W': case 'a': case 'A': cursor--; clampCursor(); break;
            case 's': case 'S': case 'd': case 'D': cursor++; clampCursor(); break;

            case 'e': case 'E':
                status = equipFromBackpack(p, cursor);
                clampCursor();
                break;

            case 'u': case 'U': {
                status = "Press slot number (0-6) to unequip: 0=Head,1=Chest,2=Weapon,3=Shield,4=Boots,5=Ring,6=Amulet";
                drawInventory(p, cursor, status);
                int k = _getch();
                if (k >= '0' && k <= '6') {
                    status = uneqipSlot(p, k - '0');
                }
                else {
                    status = "Cancelled.";
                }
                break;
            }
            case 'i': case 'I':
                return;
            case 'q': case 'Q':
                return;
            default:
                status = "Use arrows/WASD to move, E to equip, U to unequip by slot, I to return.";
                break;
        }
    }
}

GameState runMenu(Player& p) {
    for (;;) {
        std::cout << "====================\n";
        std::cout << "   ASCII RPG (C++)  \n";
        std::cout << "====================\n";
        std::cout << "1) Start Game\n";
        std::cout << "2) Quit\n";
        std::cout << "Choose: ";

        int ch = _getch();

        if (ch == '1') {
            p = Player{};
            greet(p);
            return GameState::Play;
        }
        else if (ch == '2' || ch == 'q' || ch == 'Q') {
            return GameState::Exit;
        }
        else {
            std::cout << "\nUnknown option.";
            (void)_getch();
        }
    }
}

GameState runPlay(Player& p) {
    renderInit();

    Monsters mons;
    spawnMonster(mons, MAX_MONSTERS, g_map);

    bool visible[MAP_HEIGHT][MAP_WIDTH] = {};
    bool explored[MAP_HEIGHT][MAP_WIDTH] = {};
    std::string lastMessage = "Ready.";

    for (;;) {
        computeVisibility(p, g_map, visible);
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            for (int x = 0; x < MAP_WIDTH; ++x) {
                if (visible[y][x]) explored[y][x] = true;
            }
        }

        drawMap(p, visible, explored, lastMessage, mons);

        if (p.hp <= 0) {
            lastMessage = "You died!";
            drawMap(p, visible, explored, lastMessage, mons);
            renderShutdown();
            return GameState::Menu;
        }

        Command cmd = readCommand();
        if (cmd == Command::Quit) {
            lastMessage = "Back to menu...";
            drawMap(p, visible, explored, lastMessage, mons);
            renderShutdown();
            return GameState::Menu;
        }

        if (cmd == Command::Inventory) {
            inventoryLoop(p);
            lastMessage = "Inventory closed";
            continue;
        }

        if (cmd == Command::None) {
            lastMessage = "Unknown command.";
            continue;
        }

        bool consumed = false;

        if (cmd == Command::Up || cmd == Command::Down || cmd == Command::Left || cmd == Command::Right) {
            consumed = playerBumpAttackOrMove(p, mons, cmd, lastMessage, g_map);
        }
        else if (cmd == Command::None) {
            lastMessage = "Unknown command.";
        }
        else {

        }
        if (consumed) {
            monstersTurn(mons, p, g_map);
            if (p.hp <= 0) {
                lastMessage = "You died!";
                drawMap(p, visible, explored, lastMessage, mons);
                renderShutdown();
                return GameState::Menu;
            }
        }
    }
}
