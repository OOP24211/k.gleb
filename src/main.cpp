#include <iostream>
#include <array>
#include <SFML/Graphics.hpp>
#include <algorithm>

class PositionSwitcher {
public:
    bool switch_zero_pos(std::vector<int>& vec,size_t index){
        int new_value = vec[index];
        auto it = std::find(vec.begin(),vec.end(),0);
        size_t zero_index = std::distance(vec.begin(), it);
        if ((index == zero_index-1 or index == zero_index+1 or index == zero_index-2 or index == zero_index+2) and index < vec.size() ) {
            vec[zero_index] = new_value;
            vec[index] = 0;
            return true;
        }
        return false;

    };

};

class GameLogic {
private:
    PositionSwitcher switcher;
    const std::vector<int> FINAL_TABLE = {2,2,2,0,1,1,1};
public:
    std::vector<int> game_table;
    size_t current_selection_index;
    GameLogic() {
        reset();
    }
    void reset() {
        game_table = {1, 1, 1, 0, 2, 2, 2}; // Это правильное начальное состояние для вашей логики
        current_selection_index = (game_table.size()) / 2;
    }
    bool is_win() {
        return std::equal(game_table.begin(),game_table.end(),FINAL_TABLE.begin());
    }
    void select_left() {
        if (current_selection_index >0) {
            current_selection_index--;
        }
    }

    void select_right() {
        if (current_selection_index < game_table.size()-1) {
            current_selection_index++;
        }
    }
    bool make_move() {
        if (switcher.switch_zero_pos(game_table,current_selection_index)) {
            auto it = std::find(game_table.begin(),game_table.end(),0);
            size_t new_zero_index = std::distance(game_table.begin(),it);
            current_selection_index = new_zero_index;
            return true;
        }
        return false;

    }
};

class GameView {
private:

    sf::Shader glossyShader;
    sf::CircleShape ballShape;
    sf::Sprite backgroundSprite;
    sf::RenderWindow& window;
    sf::Texture backgroundTextureMember;

    const float RADIUS = 30.0f;
    const float SPACING = 10.0f;
    const float OUTLINE_THICKNESS = 5.0f;
    const sf::Color HIGHLIGHT_COLOR = sf::Color::White;
    const sf::Color color1 = sf::Color(255, 55, 55, 255); // Красный (1)
    const sf::Color color2 = sf::Color(40, 40, 55, 255);  // Пустое место (0)
    const sf::Color color3 = sf::Color(55, 255, 55, 255); // Зеленый (2)
public:
    GameView(sf::RenderWindow& win) :window(win) {
        if (!loadAssets()) {
            std::cerr<<"Error"<<std::endl;
        }
        ballShape.setRadius(RADIUS);
        ballShape.setOrigin(RADIUS,RADIUS);
    }
    bool loadAssets() {
        if (!glossyShader.loadFromFile("glossy_sphere.frag",sf::Shader::Fragment)) {
            std::cerr<<"Shader Error! Using flat color"<<std::endl;
            return false;
        }
        if (backgroundTextureMember.loadFromFile("background.png")) {
            float window_width = static_cast<float>(window.getSize().x);
            float window_height = static_cast<float>(window.getSize().y);

            backgroundSprite.setTexture(backgroundTextureMember);
            float scaleX = window_width / backgroundTextureMember.getSize().x;
            float scaleY = window_height / backgroundTextureMember.getSize().y;
            backgroundSprite.setScale(scaleX,scaleY);
        }
        return true;
    }
    void draw(const std::vector<int> &game_table, size_t current_selection_index) {

        window.draw(backgroundSprite);

        float window_width = static_cast<float>(window.getSize().x);
        float window_height = static_cast<float>(window.getSize().y);
        float block_width = (RADIUS * 2.0f) + SPACING;
        float centerX = window_width / 2.0f;
        float centerY = window_height / 2.0f;
        size_t count_shapes = game_table.size();
        float center_index = count_shapes / 2.0f;

        for (size_t i=0; i< count_shapes; ++i) {
            float offset_index = static_cast<float>(i) - center_index;
            float new_x = centerX + offset_index * block_width;

            sf::Color currentColor;
            if (game_table[i] == 1) {
                currentColor = color1;
            } else if (game_table[i] == 0) {
                currentColor = color2;
            } else {
                currentColor = color3;
            }

            ballShape.setPosition(new_x, centerY);
            if (game_table[i] != 0) {
                sf::CircleShape dropShadow(RADIUS * 1.0f);
                dropShadow.setOrigin(RADIUS * 1.0f, RADIUS * 1.0f);
                dropShadow.setPosition(new_x + 8.0f, centerY + 12.0f);
                dropShadow.setFillColor(sf::Color(0, 0, 0, 40));
                window.draw(dropShadow);
            }
            if (game_table[i] == 0) {
                // Пустая ячейка (плоский цвет)
                ballShape.setFillColor(currentColor);
                window.draw(ballShape);
            } else {
                // Объёмный шарик (с шейдером)
                glossyShader.setParameter("u_base_color", currentColor);
                glossyShader.setParameter("u_center", new_x, centerY);
                glossyShader.setParameter("u_radius", RADIUS);
                window.draw(ballShape, &glossyShader);
            }
            if (i == current_selection_index) {
                sf::CircleShape outline(RADIUS + OUTLINE_THICKNESS / 2.0f);
                outline.setOrigin(RADIUS + OUTLINE_THICKNESS / 2.0f, RADIUS + OUTLINE_THICKNESS / 2.0f);
                outline.setPosition(new_x, centerY);
                outline.setFillColor(sf::Color::Transparent);
                outline.setOutlineThickness(OUTLINE_THICKNESS);
                outline.setOutlineColor(HIGHLIGHT_COLOR);
                window.draw(outline);
            }
        }
    }

};

class GameApp {
private:
    sf::RenderWindow window;
    GameLogic logic;
    GameView view;
    sf::Font font;
public:
    GameApp():
    window(sf::VideoMode(800, 600), "Balls Game", sf::Style::Default, sf::ContextSettings(8)),
    logic(),
    view(window) {
        window.setFramerateLimit(60);
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Font Error!." << std::endl;
        }
    }
   bool showWinDialog() {
        sf::RenderWindow dialogWindow(sf::VideoMode(600, 400), "WIN!", sf::Style::Close);
        dialogWindow.setFramerateLimit(30);

        sf::Text message;
        message.setFont(font);
        message.setString("NicE Job! Press Esc to restart game");
        message.setCharacterSize(30);
        message.setFillColor(sf::Color::Black);

        sf::FloatRect textRect = message.getLocalBounds();
        message.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        message.setPosition(dialogWindow.getSize().x / 2.0f, dialogWindow.getSize().y / 2.0f);

        while (dialogWindow.isOpen()) {
            sf::Event event;
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

    void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left) {
                    logic.select_left();
                } else if (event.key.code == sf::Keyboard::Right) {
                    logic.select_right();
                } else if (event.key.code == sf::Keyboard::Space) {
                    logic.make_move(); // Выполнение хода
                }
            }
        }
    }

    void run() {
        while (window.isOpen()) {

            // 1. Проверка победы
            if (logic.is_win()) {
                if (showWinDialog()) {
                    logic.reset();
                } else {
                    window.close();
                }
            }

            // 2. Обработка событий
            processEvents();

            // 3. Отрисовка
            window.clear(sf::Color::Black);
            view.draw(logic.game_table, logic.current_selection_index);
            window.display();
        }
    }
};



int main() {
    GameApp app;
    app.run();
    return 0;
}