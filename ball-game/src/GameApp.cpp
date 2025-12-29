#include "GameApp.hpp"
#include <iostream>
GameApp::GameApp():
    window(sf::VideoMode(800, 600), "Balls Game", sf::Style::Default, sf::ContextSettings(8)),
    logic(progress.get_ball_count()),
    view(window) {
        window.setFramerateLimit(60);
        if (!view.load_assets()) {
            std::cerr<< "Warning: Some assets failed to load." << std::endl;
        }
        if (!font.loadFromFile("obj/fonts/arial.ttf")) {
            std::cerr << "Font Error!." << std::endl;
        }
    }
   bool GameApp::show_win_dialog() const {
        sf::RenderWindow dialogWindow(sf::VideoMode(600, 400), "WIN!", sf::Style::Close);
        dialogWindow.setFramerateLimit(30);

        sf::Text message;
        message.setFont(font);
        message.setString("NicE Job! Press Esc to restart game");
        message.setCharacterSize(30);
        message.setFillColor(sf::Color::Black);

        sf::FloatRect textRect = message.getLocalBounds();
        message.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        message.setPosition(static_cast<float>(dialogWindow.getSize().x) / 2.0f, static_cast<float>(dialogWindow.getSize().y) / 2.0f);

        while (dialogWindow.isOpen()) {
            sf::Event event{};
            while (dialogWindow.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    dialogWindow.close();
                    return false;
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    dialogWindow.close();
                    return true;
                }
            }
            dialogWindow.clear(sf::Color::White);
            dialogWindow.draw(message);
            dialogWindow.display();
        }
        return false;
    }

    void GameApp::process_events() {
    sf::Event event{};
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        if (is_game_won) {
            if (event.type==sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                is_game_won = false;
                progress.next_level();
                const size_t new_n = progress.get_ball_count();
                logic.setup_level(new_n);
                if (progress.get_current_level() >= 2) {
                    view.update_window_size(new_n);
                }

            }
            continue;

        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Left) logic.select_left();
            else if (event.key.code == sf::Keyboard::Right) logic.select_right();
            else if (event.key.code == sf::Keyboard::Space) {
                if (logic.make_move() && logic.is_win()) {
                    is_game_won = true;
                }
            }
        }
    }
}
    void GameApp::run(){
    while (window.isOpen()) {
        process_events();

        window.clear(sf::Color::Black);

        // Рисуем игру
        view.draw(logic.game_table, logic.current_selection_index);

        // Если победа — рисуем оверлей поверх
        if (is_game_won) {
            view.draw_win_overlay(progress.get_current_level(),font);
        }

        window.display();
    }
}