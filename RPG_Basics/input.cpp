#include <iostream>
#include <limits>
#include "input.hpp"


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
