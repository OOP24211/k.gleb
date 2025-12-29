#include "ProgressManager.hpp"

void ProgressManager::next_level() {
    current_level++;
}
size_t ProgressManager::get_ball_count() const {
        return BASE_BALL_COUNT + (current_level -1);
}


size_t ProgressManager::get_current_level() const{
    return current_level;
}

