#pragma once
#include "IConverter.hpp"
#include "AudioFormat.hpp"

class Mix : public IConverter {
    std::vector<int16_t> mix_data_;
    size_t offset_samples_;

public:
    Mix(const std::vector<int16_t>& source, double offset_sec)
        : mix_data_(source),
          offset_samples_(static_cast<size_t>(offset_sec * AudioFormat::SAMPLE_RATE)) {}

    void process(const std::vector<int16_t>& in, std::vector<int16_t>& out) override;
};
