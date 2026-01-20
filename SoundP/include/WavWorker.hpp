#pragma once

#include <fstream>
#include <vector>
#include "AudioFormat.hpp"

class WavWorker {
#pragma pack(push, 1)
    struct WavHeader {
        char riff[AudioFormat::CHUNK_ID_SIZE];
        uint32_t fileSize;
        char wave[AudioFormat::CHUNK_ID_SIZE];
    };
#pragma pack(pop)

public:
    static void read(const std::string& path, std::vector<int16_t>& samples);
    static void write(const std::string& path, const std::vector<int16_t>& samples);
};
