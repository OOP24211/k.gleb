#include "GameLogic.hpp"
#include "PositionSwitcher.hpp"


GameLogic::GameLogic(const size_t n)
    {
        setup_level(n);
    }
void GameLogic::setup_level(const size_t n) {
        game_table.clear();
        final_table.clear();
        for (size_t i = 0; i < n; i++) {
            game_table.push_back(1);
            final_table.push_back(2);
        }
        game_table.push_back(0);
        final_table.push_back(0);
        for (size_t i = 0; i < n; i++) {
            game_table.push_back(2);
            final_table.push_back(1);
        }
        current_selection_index = game_table.size()/2;

    }
    void GameLogic::reset() {
        const size_t n = game_table.size() / 2;
        setup_level(n);
    }
    bool GameLogic::is_win() const {
        return std::equal(game_table.begin(), game_table.end(), final_table.begin());    }
    void GameLogic::select_left() {
        if (current_selection_index >0) {
            current_selection_index--;
        }
    }

    void GameLogic::select_right() {
        if (current_selection_index < game_table.size()-1) {
            current_selection_index++;
        }
    }
    bool GameLogic::make_move() {
        if (PositionSwitcher::switch_zero_pos(game_table,current_selection_index)) {
            const auto it = std::ranges::find(game_table,0);
            const size_t new_zero_index = std::distance(game_table.begin(),it);
            current_selection_index = new_zero_index;
            return true;
        }
        return false;

    }
