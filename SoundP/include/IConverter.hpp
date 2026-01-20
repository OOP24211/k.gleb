#pragma once
#include <vector>
#include <cstdint>
class IConverter {
public:
    virtual ~IConverter() = default;
    virtual void process(const std::vector<int16_t>& in, std::vector<int16_t>& out) = 0;
};
