#include <iostream>
#include <limits>
#include <conio.h>
#include "input.hpp"


Command readCommand() {
    int ch = _getch();

    if (ch == 224) {
        int k = _getch();
        switch (k) {
            case 72: return Command::Up;
            case 80: return Command::Down;
            case 75: return Command::Left;
            case 77: return Command::Right;
            default: return Command::None;
        }
    }

    switch (ch) {
        case 'w': case 'W': return Command::Up;
        case 's': case 'S': return Command::Down;
        case 'a': case 'A': return Command::Left;
        case 'd': case 'D': return Command::Right;
        case 'q': case 'Q': return Command::Quit;
        default:            return Command::None;
    }
}
