#pragma once
#include "IConverter.hpp"

class Gain : public IConverter {
    float factor_;

public:
    explicit Gain(const float factor) : factor_(factor) {}

    void process(const std::vector<int16_t>& in, std::vector<int16_t>& out) override;
};
