#pragma once
#include <vector>

class GameLogic {
private:
    std::vector<int> final_table;
public:
    std::vector<int> game_table;
    size_t current_selection_index =0;
    explicit GameLogic(size_t n);
    void setup_level(size_t n);
    void reset();
    [[nodiscard]] bool is_win() const;
    void select_left();
    void select_right();
    bool make_move();
};
