#include <iostream>
#include "engine.hpp"
#include "types.hpp"


int main() {
    Player player{};
    GameState state = GameState::Menu;

    while (state != GameState::Exit) {
        switch (state) {
            case GameState::Menu: state = runMenu(player); break;
            case GameState::Play: state = runPlay(player); break;
            default:              state = GameState::Exit; break;
        }
    }

    std::cout << "Thanks for playing!\n";
    return 0;
}
