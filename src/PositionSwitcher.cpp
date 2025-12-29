#include "PositionSwitcher.hpp"

bool PositionSwitcher::switch_zero_pos(std::vector<int>& vec, const size_t index){
    const auto it = std::ranges::find(vec,0);
    const int zero_idx = static_cast<int>(std::distance(vec.begin(), it));
    const int target_idx = static_cast<int>(index);
    if (const int diff = std::abs(target_idx - zero_idx); diff > 0 && diff <= 2) {
        std::swap(vec[zero_idx], vec[index]);
        return true;
    }
    return false;

};