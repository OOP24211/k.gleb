#include "GameView.hpp"
#include <iostream>


GameView::GameView (sf::RenderWindow& win) :window(win) {
        ballShape.setRadius(RADIUS);
        ballShape.setOrigin(RADIUS,RADIUS);
    }

    bool GameView::load_assets() {
        if (!glossyShader.loadFromFile("obj/shaders/glossy_sphere.frag",sf::Shader::Fragment)) {
            std::cerr<<"Shader Error! Using flat color"<<std::endl;
            return false;
          }
        if (backgroundTextureMember.loadFromFile("obj/textures/background.png")) {
            const float window_width = static_cast<float>(window.getSize().x);
            const float window_height = static_cast<float>(window.getSize().y);

            backgroundSprite.setTexture(backgroundTextureMember);
            const float scaleX = window_width / static_cast<float>(backgroundTextureMember.getSize().x);
            const float scaleY = window_height / static_cast<float>(backgroundTextureMember.getSize().y);
            backgroundSprite.setScale(scaleX,scaleY);
        }
        return true;
    }
    void GameView::draw(const std::vector<int> &game_table, size_t current_selection_index) {

        window.draw(backgroundSprite);

        float window_width = static_cast<float>(window.getSize().x);
        float window_height = static_cast<float>(window.getSize().y);
        float block_width = (RADIUS * 2.0f) + SPACING;
        float centerX = window_width / 2.0f;
        float centerY = window_height / 2.0f;
        size_t count_shapes = game_table.size();
        float center_index = static_cast<float>(count_shapes) / 2.0f;

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
                glossyShader.setUniform("u_base_color", sf::Glsl::Vec4(currentColor));
                glossyShader.setUniform("u_center", sf::Vector2f(new_x, centerY));
                glossyShader.setUniform("u_radius", RADIUS);
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
void GameView::draw_win_overlay(size_t level,const sf::Font& font) const {
    sf::RectangleShape dim(sf::Vector2f(window.getSize()));
    dim.setFillColor(sf::Color(0,0,0,180));
    window.draw(dim);

    sf::RectangleShape board;
    board.setSize(sf::Vector2f(400, 200));
    board.setFillColor(sf::Color(50, 50, 50, 230));
    board.setOutlineThickness(3);
    board.setOutlineColor(sf::Color::Yellow);
    board.setOrigin(200, 100);
    board.setPosition(static_cast<float>( window.getSize().x) / 2.0f, static_cast<float>(window.getSize().y) / 2.0f);
    window.draw(board);

    sf::Text text;
    text.setFont(font);
    text.setString("LEVEL " + std::to_string(level) + " CLEARED!");
    text.setCharacterSize(35);
    text.setFillColor(sf::Color::Yellow);

    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    text.setPosition(static_cast<float>(window.getSize().x) / 2.0f, static_cast<float>(window.getSize().y) / 2.0f - 30);
    window.draw(text);

    text.setString("Press ESC to continue");
    text.setCharacterSize(18);
    text.setFillColor(sf::Color::White);
    bounds = text.getLocalBounds();
    text.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
    text.setPosition(static_cast<float>( window.getSize().x) / 2.0f, static_cast<float>(window.getSize().y) / 2.0f + 40);
    window.draw(text);


}

    void GameView::update_window_size(const size_t n){
        const float block_width = (RADIUS * 2.0f) + SPACING;
        const float total_elements = static_cast<float>(n)*2+1;
        float new_width = (static_cast<float>(total_elements)* block_width)+100.0f;
        if  (new_width < 800.0f) new_width = 800.0f;
        window.setSize(sf::Vector2u(static_cast<unsigned int >(new_width),600));
        const sf::FloatRect visibleArea(0,0,new_width,600);
        window.setView(sf::View(visibleArea));
        if (backgroundTextureMember.getSize().x >0) {
            const float scaleX =new_width / static_cast<float>(backgroundTextureMember.getSize().x);
            const float scaleY =600.0f / static_cast<float>(backgroundTextureMember.getSize().y);
            backgroundSprite.setScale(scaleX,scaleY);
        }

    }
