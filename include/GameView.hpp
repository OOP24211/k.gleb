#pragma once
#include <SFML/Graphics.hpp>


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
    explicit GameView (sf::RenderWindow& win);
    bool load_assets();
    void draw(const std::vector<int> &game_table, size_t current_selection_index);
    void draw_win_overlay(size_t level,const sf::Font& font) const;
    void update_window_size( size_t n);

};
