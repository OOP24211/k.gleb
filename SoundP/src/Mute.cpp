#include "Mute.hpp"

void Mute::process(const std::vector<int16_t>& in, std::vector<int16_t>& out) {
        out = in;
        for (size_t i = 0; i < in.size(); ++i) {
            if (i >= start_sample_ && i < end_sample_) {
                out[i] = 0;
            }
        }
    }
