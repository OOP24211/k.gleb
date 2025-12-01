#include <iostream>
#include <array>
#include <algorithm>

template <size_t N>
class PositionSwitcher {
    int new_value = 0;
public:
    void switch_zero_pos(std::array<int, N>& arr,size_t index){
        new_value = arr[index];
        auto it = std::find(arr.begin(),arr.end(),0);
        size_t zero_index = std::distance(arr.begin(), it);
        if ((index == zero_index-1 or index == zero_index+1 or index == zero_index-2 or index == zero_index+2) and index < N ) {
            arr[zero_index] = new_value;
            arr[index] = 0;
        }
        else {
            std::cout<< "Not valid position"<<std::endl;
        }
    };

};


int main() {
    std::array final_table = {2,2,2,0,1,1,1};
    std::array game_table = {1,1,1,0,2,2,2};
    size_t switch_position = 0;
    PositionSwitcher<game_table.size()> switcher;
    for (size_t i = 0; i < game_table.size(); ++i) {
        std::cout << game_table[i] << (i == game_table.size() - 1 ? "" : ", ");
    }
    std::cout << std::endl;

    while (final_table != game_table) {
        std::cout <<"Print pos to switch: ";
        std::cin >> switch_position;
        switcher.switch_zero_pos(game_table,switch_position);
        for (size_t i = 0; i < game_table.size(); ++i) {
            std::cout << game_table[i] << (i == game_table.size() - 1 ? "" : ", ");
        }
        std::cout << std::endl;

    }

    return 0;
}
