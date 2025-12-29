#pragma once
#include "GameLogic.hpp"
#include "GameView.hpp"
#include "ProgressManager.hpp"

class GameApp {
private:
    bool is_game_won = false;
    sf::RenderWindow window;
    ProgressManager progress;
    GameLogic logic;
    GameView view;
    sf::Font font;
public:
    explicit GameApp();
    bool show_win_dialog() const;
    void process_events();
    void run();
};