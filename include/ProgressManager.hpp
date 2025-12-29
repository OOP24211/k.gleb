# pragma once

class ProgressManager {
private:
    size_t current_level = 1;
    const size_t BASE_BALL_COUNT=3;
public:
    void next_level();
    [[nodiscard]] size_t get_ball_count() const;
    [[nodiscard]] size_t get_current_level() const;
};