
#include <iostream>
#include <string>
#include <limits>


// ====== CONFIG ======

constexpr int MAP_WIDTH = 10;
constexpr int MAP_HEIGHT = 6;
constexpr int VISION_RADIUS = 4;

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
    {Tile::Floor, Tile::Floor, Tile::Wall, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Wall, Tile::Floor, Tile::Floor},
    {Tile::Floor, Tile::Floor, Tile::Wall, Tile::Floor, Tile::Trap, Tile::Floor, Tile::Floor, Tile::Wall, Tile::Floor, Tile::Trap},
    {Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor},
    {Tile::Wall, Tile::Wall, Tile::Floor, Tile::Wall, Tile::Floor, Tile::Floor, Tile::Wall, Tile::Floor, Tile::Wall, Tile::Floor},
    {Tile::Floor, Tile::Floor, Tile::Floor, Tile::Trap, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Trap, Tile::Floor},
    (Tile::Floor, Tile::Floor, Tile::Wall, Tile::Floor, Tile::Trap, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Floor, Tile::Trap)
};

// ====== DECLARATIONS ======

void greet(Player& p);
Command readCommand();

bool isWalkable(int nx, int ny, const Tile map[MAP_HEIGHT][MAP_WIDTH]);
bool applyTileEffect(Player& p, Tile t);
bool tryMove(Player& p, Command cmd, const Tile map[MAP_HEIGHT][MAP_WIDTH]);

bool blocksSight(Tile t);
bool hasLineOfSight(const Tile map[MAP_HEIGHT][MAP_WIDTH], int x0, int y0, int x1, int y1);
void computeVisibility(const Player& p,
                       const Tile map[MAP_HEIGHT][MAP_WIDTH],
                       bool visible[MAP_HEIGHT][MAP_WIDTH]);

void drawMap(const Player& p,
             const Tile map[MAP_HEIGHT][MAP_WIDTH],
             const bool visible[MAP_HEIGHT][MAP_WIDTH],
             const bool explored[MAP_HEIGHT][MAP_WIDTH]);

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

bool blockSight(Tile t) {
    return t == Tile::Wall;
}

bool hasLineOfSight(const Tile map[MAP_HEIGHT][MAP_WIDTH], int x0, int y0, int x1, int y1) {
    if (x0 < 0 || x0 >= MAP_WIDTH || y0 < 0 || y0 >= MAP_HEIGHT) return false;
    if (x1 < 0 || x1 >= MAP_WIDTH || y1 < 0 || y1 >= MAP_HEIGHT) return false;

    int dx = std::abs(x1 - x0);
    int sx = (x0 < x1) ? 1 : -1;
    int dy = -std::abs(y1 - y0);
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    int x = x0;
    int y = y0;

    while (true) {
        if (!(x == x0 && y == y0)) {
            if (blockSight(map[y][x])) {
                if (!(x == x1 && y == y1)) return false;
            }
        }
        if (x == x1 && y == y1) break;

        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x += sx; }
        if (e2 <= dx) { err += dx; y += sy; }
    }
    return true;
}

void computeVisibility(const Player& p,
                       const Tile map[MAP_HEIGHT][MAP_WIDTH],
                       bool visible[MAP_HEIGHT][MAP_WIDTH]) {
    for (int r = 0; r < MAP_HEIGHT; ++r)
        for (int c = 0; c < MAP_WIDTH; ++c)
            visible[r][c] = false;

    int xmin = std::max(0, p.x - VISION_RADIUS);
    int xmax = std::min(MAP_WIDTH - 1, p.x + VISION_RADIUS);
    int ymin = std::max(0, p.y - VISION_RADIUS);
    int ymax = std::min(MAP_HEIGHT - 1, p.y + VISION_RADIUS);

    int r2 = VISION_RADIUS * VISION_RADIUS;

    for (int y = ymin; y <= ymax; ++y) {
        for (int x = xmin; x <= xmax; ++x) {
            int dx = x - p.x;
            int dy = y - p.y;
            if (dx * dx + dy * dy <= r2) {
                if (hasLineOfSight(map, p.x, p.y, x, y)) {
                    visible[y][x] = true;
                }
            }
        }
    }
}

char glyphFor(Tile t, bool isVisible, bool isExplored) {
    if (!isExplored) return ' ';
    if (isVisible) {
        switch (t) {
            case Tile::Floor: return '.';
            case Tile::Wall: return '#';
            case Tile::Trap: return 'T';
        }
    }
    else {
        // GHOSTED (IN FOG OF WAR)
        switch (t) {
            case Tile::Floor: return ',';
            case Tile::Wall: return '+';
            case Tile::Trap: return '^';
        }
    }
    return ' ';
}

void drawMap(const Player& p,
             const Tile map[MAP_HEIGHT][MAP_WIDTH],
             const bool visible[MAP_HEIGHT][MAP_WIDTH],
             const bool explored[MAP_HEIGHT][MAP_WIDTH]) {
    for (int row = 0; row < MAP_HEIGHT; ++row) {
        for (int col = 0; col < MAP_WIDTH; ++col) {
            if (row == p.y && col == p.x) {
                std::cout << '@';
            }
            else {
                std::wcout << glyphFor(map[row][col], visible[row][col], explored[row][col]);
            }
        }
        std::cout << '\n';
    }
    std::cout << "HP: " << p.hp << "   (W/S/A/D to move, Q to quit)\n";
}

// ====== STATES ======

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
    bool visible[MAP_HEIGHT][MAP_WIDTH] = {};
    bool explored[MAP_HEIGHT][MAP_WIDTH] = {};
    
    while (p.hp > 0) {
        computeVisibility(p, map, visible);
        for (int r = 0; r < MAP_HEIGHT; ++r)
            for (int c = 0; c < MAP_WIDTH; ++c)
                if (visible[r][c]) explored[r][c] = true;

        drawMap(p, map, visible, explored);

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
