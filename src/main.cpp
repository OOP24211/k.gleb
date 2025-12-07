#include <iostream>
#include <array>
#include <SFML/Graphics.hpp>
#include <algorithm>

template <size_t N>
class PositionSwitcher {
    int new_value = 0;
public:
    bool switch_zero_pos(std::array<int, N>& arr,size_t index){
        new_value = arr[index];
        auto it = std::find(arr.begin(),arr.end(),0);
        size_t zero_index = std::distance(arr.begin(), it);
        if ((index == zero_index-1 or index == zero_index+1 or index == zero_index-2 or index == zero_index+2) and index < N ) {
            arr[zero_index] = new_value;
            arr[index] = 0;
            return true;
        }
        else {
            return false;
        }
    };

};


template <size_t N>
void resetGame(std::array<int, N>& game_table,
               std::array<sf::CircleShape, N>& shape_array,
               size_t& current_selection_index,
               const sf::Color color1, const sf::Color color2, const sf::Color color3,
               float centerX, float centerY, float block_width, float radius)
{
    game_table = {1, 1, 1, 0, 2, 2, 2};

    current_selection_index = N / 2;

    const float center_index = N / 2.0f;
    for (size_t i = 0; i < N; ++i) {

        float offset_index = static_cast<float>(i) - center_index;
        float new_x = centerX + offset_index * block_width;
        shape_array[i].setPosition(new_x, centerY);

        if (game_table[i] == 1) {
            shape_array[i].setFillColor(color1);
        } else if (game_table[i] == 0) {
            shape_array[i].setFillColor(color2);
        } else {
            shape_array[i].setFillColor(color3);
        }
        shape_array[i].setOutlineThickness(0.0f);
    }
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
    std::array game_table = {1,1,1,0,2,2,2};
    std::array final_table = {1,1,2,0,1,2,2};
    //std::array final_table = {2,2,2,0,1,1,1};
    const size_t count_shapes= 7;
    PositionSwitcher<count_shapes> switcher;
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML works!");
    window.setFramerateLimit(60);
    std::array<sf::CircleShape, count_shapes> shape_array;
    float radius = 30.0f;
    float spacing = 10.0f;
    float center_index = count_shapes/2;
    size_t current_selection_index = 0;
    const float OUTLINE_THICKNESS = 5.0f;
    const sf::Color HIGHLIGHT_COLOR = sf::Color::White;
    const sf::Color color1 = sf::Color::Red;
    const sf::Color color2 = sf::Color::Yellow;
    const sf::Color color3 = sf::Color::Green;
    float window_width = static_cast<float>(window.getSize().x);
    float window_height = static_cast<float>(window.getSize().y);
    float block_width = (radius * 2.0f)+spacing;
    float centerX = window_width/2.0f;
    float centerY = window_height/2.0f;

    for (size_t i = 0; i < count_shapes; ++i) {
        shape_array[i] = sf::CircleShape(radius);
        shape_array[i].setOrigin(radius, radius);
        float offset_index= static_cast<float>(i) - static_cast<float>(center_index);
        float new_x = centerX + offset_index*block_width;
        shape_array[i].setPosition(new_x,centerY);

        if (i < center_index ) {
            shape_array[i].setFillColor(color1);
        } else if (i> center_index) {
            shape_array[i].setFillColor(color3);
        }
        else {
            shape_array[i].setFillColor(color2);
        }


    }

    while (window.isOpen()) {
        if (std::equal(game_table.begin(), game_table.end(), final_table.begin())) {

            if (showWinDialog()) {
                resetGame(game_table, shape_array, current_selection_index,
                          color1, color2, color3, centerX, centerY, block_width, radius);
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
                    auto it = std::find(game_table.begin(),game_table.end(),0);
                    size_t old_zero_index = std::distance(game_table.begin(),it);
                    if (switcher.switch_zero_pos(game_table,pressed_index)) {
                        sf::Color color_at_ball_pos =shape_array[pressed_index].getFillColor();
                        sf::Color color_at_zero_pos = shape_array[old_zero_index].getFillColor();
                        shape_array[pressed_index].setFillColor(color_at_zero_pos);
                        shape_array[old_zero_index].setFillColor(color_at_ball_pos);
                        current_selection_index = old_zero_index;
                    }

                }
            }
        }
        for (size_t i = 0; i < count_shapes; ++i) {
            shape_array[i].setOutlineThickness(0.0f);
        }
        shape_array[current_selection_index].setOutlineThickness(OUTLINE_THICKNESS);
        shape_array[current_selection_index].setOutlineColor(HIGHLIGHT_COLOR);

        window.clear(sf::Color::Black);


        for (const auto & shape:shape_array) {
            window.draw(shape);
        }


        window.display();
    }

    return 0;
}
