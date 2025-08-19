
#include <iostream>
#include <string>
#include <limits>


// ====== CONFIG ======

constexpr int MAP_WIDTH = 5;
constexpr int MAP_HEIGHT = 5;

// ====== TYPES ======

struct Player {
    std::string name{};
    int x{ MAP_WIDTH / 2 };
    int y{ MAP_HEIGHT / 2 };
    int hp{ 10 };
};

enum class Command {
    Up, Down, Left, Right, Quit, None
};

// ====== DECLARATIONS ======

void greet(Player& p);
void drawMap(const Player& p);
Command readCommand();
bool tryMove(Player& p, Command cmd);

// ====== DEFINITIONS ======

void greet(Player& p) {
    std::cout << "Welcome to C++! Let's build an RPG.\n";
    std::cout << "Enter your hero name: ";
    std::getline(std::cin, p.name);
    if (p.name.empty()) p.name = "Hero";
    std::cout << "Greetings, " << p.name << "! Your journey begins...\n";
}

void drawMap(const Player& p) {
    for (int row = 0; row < MAP_HEIGHT; ++row) {
        for (int col = 0; col < MAP_WIDTH; ++col) {
            if (row == p.y && col == p.x) std::cout << '@';
            else                          std::cout << '.';
        }
        std::cout << '\n';
    }
    std::cout << "HP: " << p.hp << "   (W/S/A/D to move, Q to quit)\n";
}

Command readCommand() {
    std::cout << "Command: ";
    char c{};
    if (!(std::cin >> c)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return Command::None;
    }
    switch (c) {
    case 'w': case 'W': return Command::Up;
    case 's': case 'S': return Command::Down;
    case 'a': case 'A': return Command::Left;
    case 'd': case 'D': return Command::Right;
    case 'q': case 'Q': return Command::Quit;
    default:            return Command::None;
    }
}

bool tryMove(Player& p, Command cmd) {
    int nx = p.x, ny = p.y;
    if (cmd == Command::Up) ny -= 1;
    if (cmd == Command::Down) ny += 1;
    if (cmd == Command::Left) nx -= 1;
    if (cmd == Command::Right) nx += 1;

    if (nx < 0 || nx >= MAP_WIDTH) return false;
    if (ny < 0 || ny >= MAP_HEIGHT) return false;

    p.x = nx; p.y = ny;
    return true;
}


int main() {
    Player player{};
    greet(player);

    while (true) {
        drawMap(player);
        Command cmd = readCommand();

        if (cmd == Command::Quit) break;
        if (cmd == Command::None) {
            std::cout << "Unknown command. \n\n";
            continue;
        }

        bool moved = tryMove(player, cmd);
        if (!moved) std::cout << "You bump into an invisible wall! \n\n";
        else        std::cout << "\n";
    }

    std::cout << "Bye!\n";
    return 0;
}