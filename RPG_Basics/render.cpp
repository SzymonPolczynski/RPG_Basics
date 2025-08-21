#include <iostream>
#include <sstream>
#include <string>
#include "render.hpp"
#include "map.hpp"


#ifdef _WIN32
    #include <Windows.h>
    static HANDLE gConsole = nullptr;

    static void clearScreenAndHome() {
        if (!gConsole) return;
        CONSOLE_SCREEN_BUFFER_INFO csbi{};
        if (!GetConsoleScreenBufferInfo(gConsole, &csbi)) return;

        DWORD cellCount = static_cast<DWORD>(csbi.dwSize.X) * static_cast<DWORD>(csbi.dwSize.Y);
        DWORD written = 0;
        COORD home{ 0, 0 };

        FillConsoleOutputCharacterA(gConsole, ' ', cellCount, home, &written);
        FillConsoleOutputAttribute(gConsole, csbi.wAttributes, cellCount, home, &written);
        SetConsoleCursorPosition(gConsole, home);
    }

    static void hideCursor(bool hide) {
        if (!gConsole) return;
        CONSOLE_CURSOR_INFO info{};
        if (!GetConsoleCursorInfo(gConsole, &info)) return;
        info.bVisible = hide ? FALSE : TRUE;
        SetConsoleCursorInfo(gConsole, &info);
    }
#endif // _WIN32

void renderInit() {
#ifdef _WIN32
    gConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    hideCursor(true);
#endif // _WIN32

#ifdef _WIN32
    clearScreenAndHome();
#else
    std::cout << "\x1b[2J\x1b[H"
#endif // _WIN32
    std::cout.flush();

}

void renderShutdown() {
#ifdef _WIN32
    hideCursor(false);
#endif // _WIN32
    std::cout.flush();
}

void drawMap(const Player& p,
             const bool visible[MAP_HEIGHT][MAP_WIDTH],
             const bool explored[MAP_HEIGHT][MAP_WIDTH],
             const std::string& status) {
#ifdef _WIN32
    clearScreenAndHome();
#else
    std::cout << "\x1b[H";
#endif // _WIN32

    std::ostringstream out;

    for (int row = 0; row < MAP_HEIGHT; ++row) {
        for (int col = 0; col < MAP_WIDTH; ++col) {
            if (row == p.y && col == p.x) out << '@';
            else out << glyphFor(g_map[row][col], visible[row][col], explored[row][col]);
        }
        out << '\n';
    }

    out << "HP: " << p.hp << "   (W/A/S/D to move, Q to quit)\n";
    out << "Status: " << status << "\n";

    std::cout << out.str();
    std::cout.flush();
}
