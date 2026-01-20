#pragma once
#include <cstdint>

namespace AudioFormat {
    constexpr uint16_t FORMAT_PCM = 1;
    constexpr uint32_t SAMPLE_RATE = 44100;
    constexpr uint16_t CHANNELS = 1;
    constexpr uint16_t BITS_PER_SAMPLE = 16;
    constexpr uint32_t BYTE_RATE = SAMPLE_RATE * CHANNELS * BITS_PER_SAMPLE / 8;
    constexpr uint16_t BLOCK_ALIGN = CHANNELS * BITS_PER_SAMPLE / 8;
    constexpr uint32_t FMT_CHUNK_SIZE = 16;
    constexpr uint32_t HEADER_SIZE = 36;
    constexpr size_t CHUNK_ID_SIZE = 4;

}
