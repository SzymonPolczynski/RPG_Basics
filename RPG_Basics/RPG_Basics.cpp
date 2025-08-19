
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

enum class Command { Up, Down, Left, Right, Quit, None };
enum class Tile    { Floor = 0, Wall = 1, Trap = 2};
enum GameState { Menu, Play, Exit };

// ====== MAP DATA ======
// 0 = floor, 1 = wall, 2 = trap

Tile g_map[MAP_HEIGHT][MAP_WIDTH] = {
    {Tile::Floor, Tile::Floor, Tile::Wall, Tile::Floor, Tile::Floor},
    {Tile::Floor, Tile::Floor, Tile::Wall, Tile::Floor, Tile::Trap},
    {Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor},
    {Tile::Wall, Tile::Wall, Tile::Floor, Tile::Wall, Tile::Floor },
    {Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor}
};

// ====== DECLARATIONS ======

void greet(Player& p);
void drawMap(const Player& p, const Tile map[MAP_HEIGHT][MAP_WIDTH]);
Command readCommand();

bool isWalkable(int nx, int ny, const Tile map[MAP_HEIGHT][MAP_WIDTH]);
bool applyTileEffect(Player& p, Tile t);
bool tryMove(Player& p, Command cmd, const Tile map[MAP_HEIGHT][MAP_WIDTH]);

GameState runMenu(Player& p);
GameState runPlay(Player& p, Tile map[MAP_HEIGHT][MAP_WIDTH]);

// ====== DEFINITIONS ======

void greet(Player& p) {
    std::cout << "Welcome to C++! Let's build an RPG.\n";
    std::cout << "Enter your hero name: ";
    std::getline(std::cin, p.name);
    if (p.name.empty()) p.name = "Hero";
    std::cout << "Greetings, " << p.name << "! Your journey begins...\n";
}

void drawMap(const Player& p, const Tile map[MAP_HEIGHT][MAP_WIDTH]) {
    for (int row = 0; row < MAP_HEIGHT; ++row) {
        for (int col = 0; col < MAP_WIDTH; ++col) {
            if (row == p.y && col == p.x) std::cout << '@';
            else {
                switch (map[row][col]) {
                    case Tile::Floor: std::cout << '.'; break;
                    case Tile::Wall:  std::cout << '#'; break;
                    case Tile::Trap:  std::cout << 'T'; break;
                }
            }
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

bool isWalkable(int nx, int ny, const Tile map[MAP_HEIGHT][MAP_WIDTH]) {
    if (nx < 0 || nx >= MAP_WIDTH) return false;
    if (ny < 0 || ny >= MAP_HEIGHT) return false;
    return map[ny][nx] != Tile::Wall;
}

bool applyTileEffect(Player& p, Tile t) {
    if (t == Tile::Trap) {
        p.hp -= 1;
        std::cout << "Ouch! You stepped on a trap. HP now: " << p.hp << "\n";
        return true;
    }
    return false;
}

bool tryMove(Player& p, Command cmd, const Tile map[MAP_HEIGHT][MAP_WIDTH]) {
    int nx = p.x, ny = p.y;
    if (cmd == Command::Up) ny -= 1;
    if (cmd == Command::Down) ny += 1;
    if (cmd == Command::Left) nx -= 1;
    if (cmd == Command::Right) nx += 1;

    // MOVEMENT RESTRICTIONS
    if (!isWalkable(nx, ny, map)) {
        std::cout << "Bump! You can't go there.\n";
        return false;
    }

    // ALLOW MOVEMENT
    p.x = nx;
    p.y = ny;

    // TRAP CHECK
    applyTileEffect(p, map[p.y][p.x]);
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

GameState runPlay(Player& p, Tile map[MAP_HEIGHT][MAP_WIDTH]) {
    while (p.hp > 0) {
        drawMap(p, map);
        Command cmd = readCommand();

        if (cmd == Command::Quit) {
            std::cout << "Back to menu...\n\n";
            return GameState::Menu;
        }
        if (cmd == Command::None) {
            std::cout << "Unknown command.\n\n";
            continue;
        }

        (void)tryMove(p, cmd, map);
        std::cout << "\n";
    }
    std::cout << "You died! Game over.\n";
    return GameState::Exit;
}

int main() {
    Player player{};
    GameState state = GameState::Menu;

    while (state != GameState::Exit) {
        switch (state) {
            case GameState::Menu: state = runMenu(player); break;
            case GameState::Play: state = runPlay(player, g_map); break;
            default:              state = GameState::Exit; break;
        }
    }

    std::cout << "Thanks for playing!\n";
    return 0;
}
