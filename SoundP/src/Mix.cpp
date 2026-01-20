#include "Mix.hpp"
#include <algorithm>

void Mix::process(const std::vector<int16_t>& in, std::vector<int16_t>& out) {
        out.resize(in.size());
        for (size_t i = 0; i < in.size(); ++i) {
            int32_t val = in[i];
            if (i >= offset_samples_) {
                if (const size_t mix_pos = i - offset_samples_; mix_pos < mix_data_.size()) {
                    val = (val + mix_data_[mix_pos]) / 2;
                }
            }
            out[i] = static_cast<int16_t>(
                std::clamp<int32_t>(val, -32768, 32767)
            );
        }
    }
