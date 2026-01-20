#pragma once
#include "IConverter.hpp"
#include "AudioFormat.hpp"
#include <vector>

class Mute : public IConverter {
    size_t start_sample_, end_sample_;
public:
    Mute(double start_sec, double end_sec)
        : start_sample_(static_cast<size_t>(start_sec * AudioFormat::SAMPLE_RATE)),
          end_sample_(static_cast<size_t>(end_sec * AudioFormat::SAMPLE_RATE)) {}

    void process(const std::vector<int16_t>& in, std::vector<int16_t>& out) override;

};
