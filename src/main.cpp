#include <iostream>
#include <array>
#include <SFML/Graphics.hpp>
#include <algorithm>

class PositionSwitcher {
    int new_value = 0;
public:
    bool switch_zero_pos(std::vector<int>& vec,size_t index){
        new_value = vec[index];
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


void resetGame(std::vector<int>& game_table,
               size_t& current_selection_index)
{
    // Начальное состояние игры
    game_table = {1, 1, 1, 0, 2, 2, 2};
    current_selection_index = (game_table.size()) / 2;

    std::cout << "--- Game Restarted ---" << std::endl;
}


bool showWinDialog() {
    sf::RenderWindow dialogWindow(sf::VideoMode(600, 400), "WIN!", sf::Style::Close);
    dialogWindow.setFramerateLimit(30);

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Ошибка загрузки шрифта! Текст может не отобразиться." << std::endl;
    }

    sf::Text message;
    message.setFont(font);
    message.setString("NicE Job! Press Esc to restart game");
    message.setCharacterSize(30);
    message.setFillColor(sf::Color::Black);

    sf::FloatRect textRect = message.getLocalBounds();
    message.setOrigin(textRect.left + textRect.width / 2.0f,
                      textRect.top + textRect.height / 2.0f);
    message.setPosition(dialogWindow.getSize().x / 2.0f, dialogWindow.getSize().y / 2.0f);

    while (dialogWindow.isOpen()) {
        sf::Event event;
        while (dialogWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                dialogWindow.close();
                return false;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    dialogWindow.close();
                    return true;
                }
            }
        }

        dialogWindow.clear(sf::Color::White);
        dialogWindow.draw(message);
        dialogWindow.display();
    }

    return false;
}



int main() {
    std::vector<int> game_table = {1,1,1,0,2,2,2};
    std::array final_table = {2,2,2,0,1,1,1};
    const size_t count_shapes= 7;
    sf::ContextSettings settings;
    sf::Shader glossyShader;
    if (!glossyShader.loadFromFile("glossy_sphere.frag", sf::Shader::Fragment)) {
        std::cerr << "Ошибка загрузки шейдера! Будет использоваться плоский цвет." << std::endl;
        return -1;
    }

    settings.antialiasingLevel = 8;
    PositionSwitcher switcher;
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Works!", sf::Style::Default, settings);    window.setFramerateLimit(60);
    //std::array<sf::CircleShape, count_shapes> shape_array;
    float radius = 30.0f;
    float spacing = 10.0f;
    sf::CircleShape ballShape(radius);
    ballShape.setOrigin(radius, radius);
    float center_index = count_shapes/2;
    size_t current_selection_index = 0;
    const float OUTLINE_THICKNESS = 5.0f;
    const sf::Color HIGHLIGHT_COLOR = sf::Color::White;
    const sf::Color color1 = sf::Color(255, 55, 55, 255);
    const sf::Color color2 = sf::Color(40, 40, 55, 255);
    const sf::Color color3 = sf::Color(55, 255, 55, 255);
    sf::Texture backgroundTexture;

    backgroundTexture.loadFromFile("background.png");
    sf::Sprite backgroundSprite(backgroundTexture);
    float window_width = static_cast<float>(window.getSize().x);
    float window_height = static_cast<float>(window.getSize().y);
    if (backgroundTexture.getSize().x > 0 && backgroundTexture.getSize().y > 0) {
        float scaleX = window_width / backgroundTexture.getSize().x;
        float scaleY = window_height / backgroundTexture.getSize().y;
        backgroundSprite.setScale(scaleX, scaleY);
    }
    float block_width = (radius * 2.0f)+spacing;
    float centerX = window_width/2.0f;
    float centerY = window_height/2.0f;




    while (window.isOpen()) {
        if (std::equal(game_table.begin(), game_table.end(), final_table.begin())) {
            if (showWinDialog()) {
                resetGame(game_table, current_selection_index);
            } else {
                window.close();
            }
        }
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type==sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Left) {
                    if (current_selection_index > 0) {
                        current_selection_index--;
                    }
                }
                else if (event.key.code == sf::Keyboard::Right) {
                    if (current_selection_index < count_shapes-1) {
                        current_selection_index++;
                    }
                }
                else if (event.key.code == sf::Keyboard::Space) {
                    size_t pressed_index = current_selection_index;
                    // Пытаемся поменять местами шарик и пустую ячейку
                    if (switcher.switch_zero_pos(game_table, pressed_index)) {
                        // После успешного перемещения, выделение перемещается на пустую ячейку (теперь заполненную)
                        // Находим новый индекс пустого места (0)
                        auto it = std::find(game_table.begin(), game_table.end(), 0);
                        size_t new_zero_index = std::distance(game_table.begin(), it);

                        // Выделение переходит на новую пустую позицию
                        current_selection_index = new_zero_index;
                    }

                }
            }
        }

        //   shape_array[current_selection_index].setOutlineThickness(OUTLINE_THICKNESS);
        // shape_array[current_selection_index].setOutlineColor(HIGHLIGHT_COLOR);

        window.clear(sf::Color::Black);

        if (backgroundTexture.getSize().x > 0) { // Отрисовываем спрайт, только если текстура загружена
            window.draw(backgroundSprite);
        }
        //   for (const auto & shape:shape_array) {
        //   window.draw(shape);
        //   }
        // ...
        float offset_index;
        float new_x;
        sf::Color currentColor;

for (size_t i = 0; i < count_shapes; ++i) {

            offset_index = static_cast<float>(i) - center_index;
            new_x = centerX + offset_index * block_width;

            // --- 0. ОТРИСОВКА ОТБРАСЫВАЕМОЙ ТЕНИ (Drop Shadow) ---
            if (game_table[i] != 0) {
                sf::CircleShape dropShadow(radius * 1.0f); // Ещё больше радиус (1.25x)
                dropShadow.setOrigin(radius * 1.0f, radius * 1.0f);

                // Смещение: немного вправо и вниз (для лучшего разделения с шариком)
                dropShadow.setPosition(new_x + 8.0f, centerY + 12.0f);

                // Цвет: Сделаем еще более прозрачным (Alpha 70)
                dropShadow.setFillColor(sf::Color(0, 0, 0, 40));
                window.draw(dropShadow);
            }

            // Определяем цвет и позицию
            if (game_table[i] == 1) {
                currentColor = color1;
            } else if (game_table[i] == 0) {
                currentColor=color2;
            } else {
                currentColor = color3;
            }

            ballShape.setPosition(new_x, centerY);

            // --- 1. Отрисовка шарика с шейдером ---
            if (game_table[i] == 0) {
                // Пустая ячейка (плоский желтый)
                ballShape.setFillColor(currentColor);
                window.draw(ballShape);
            } else {
                // Объёмный шарик
                glossyShader.setParameter("u_base_color", currentColor);
                glossyShader.setParameter("u_center", new_x, centerY);
                glossyShader.setParameter("u_radius", radius);
                window.draw(ballShape, &glossyShader);
            }

            // --- 2. ОТРИСОВКА ЭФФЕКТА НЕОНОВОГО КОНТУРА ---
            if (i == current_selection_index) {


                // b) Основное белое, яркое кольцо (сам ободок)
                sf::CircleShape outline(radius + OUTLINE_THICKNESS / 2.0f);
                outline.setOrigin(radius + OUTLINE_THICKNESS / 2.0f, radius + OUTLINE_THICKNESS / 2.0f);
                outline.setPosition(new_x, centerY);
                outline.setFillColor(sf::Color::Transparent);
                outline.setOutlineThickness(OUTLINE_THICKNESS);
                outline.setOutlineColor(HIGHLIGHT_COLOR);
                window.draw(outline);
            }
        }

        window.display();
    }

    return 0;
}