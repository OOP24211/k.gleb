#include "../include/Gain.hpp"

void Gain::process(const std::vector<int16_t>& in, std::vector<int16_t>& out) {
        out.resize(in.size());
        for (size_t i = 0; i < in.size(); ++i) {
            float scaled = static_cast<float>(in[i]) * factor_;
            if (scaled > 32767.0f) scaled = 32767.0f;
            else if (scaled < -32768.0f) scaled = -32768.0f;
            out[i] = static_cast<int16_t>(scaled);
        }
    }
