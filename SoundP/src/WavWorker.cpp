#include "../include/WavWorker.hpp"
#include <cstring>
#include "../Exceptions.hpp"


void WavWorker::read(const std::string& path,std::vector<int16_t>& samples) {
        std::ifstream input_file(path, std::ios::binary);
        if (!input_file) {
            throw SoundProcessorException("Cannot open input file: " + path);
        }

        WavHeader header{};
        input_file.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));
        if (input_file.gcount() != sizeof(WavHeader)) {
            throw SoundProcessorException("File too small");
        }

        if (memcmp(header.riff, "RIFF", AudioFormat::CHUNK_ID_SIZE) != 0 || memcmp(header.wave, "WAVE", AudioFormat::CHUNK_ID_SIZE) != 0) {
            throw SoundProcessorException("Not a RIFF/WAVE file");
        }

        uint16_t channels = 0;
        uint32_t sampleRate = 0;
        uint16_t bitsPerSample = 0;
        uint32_t dataSize = 0;
        bool found_fmt = false, found_data = false;

        char chunk_id[AudioFormat::CHUNK_ID_SIZE];
        uint32_t chunk_size;

        while (input_file.read(chunk_id, AudioFormat::CHUNK_ID_SIZE)) {
            input_file.read(reinterpret_cast<char*>(&chunk_size), AudioFormat::CHUNK_ID_SIZE);
            if (input_file.gcount() != AudioFormat::CHUNK_ID_SIZE) break;

            if (memcmp(chunk_id, "fmt ", AudioFormat::CHUNK_ID_SIZE) == 0) {
                if (chunk_size < AudioFormat::FMT_CHUNK_SIZE) throw SoundProcessorException("Invalid fmt chunk");
                uint16_t format;
                input_file.read(reinterpret_cast<char*>(&format), 2);
                input_file.read(reinterpret_cast<char*>(&channels), 2);
                input_file.read(reinterpret_cast<char*>(&sampleRate), 4);
                input_file.seekg(6, std::ios::cur);
                input_file.read(reinterpret_cast<char*>(&bitsPerSample), 2);
                if (format != AudioFormat::FORMAT_PCM || channels != AudioFormat::CHANNELS|| sampleRate != AudioFormat::SAMPLE_RATE || bitsPerSample != AudioFormat::BITS_PER_SAMPLE) {
                    throw SoundProcessorException("Unsupported format: must be PCM mono 16-bit 44100 Hz");
                }
                found_fmt = true;
                if (chunk_size > AudioFormat::FMT_CHUNK_SIZE) {
                    input_file.seekg(chunk_size - AudioFormat::FMT_CHUNK_SIZE, std::ios::cur);
                }
            } else if (memcmp(chunk_id, "data", AudioFormat::CHUNK_ID_SIZE) == 0) {
                dataSize = chunk_size;
                found_data = true;
                break;
            } else {
                input_file.seekg(chunk_size, std::ios::cur);
            }
        }

        if (!found_fmt || !found_data) {
            throw SoundProcessorException("Missing fmt or data chunk");
        }

        if (dataSize % 2 != 0) {
            throw SoundProcessorException("Invalid data size");
        }
        samples.resize(dataSize / 2);
        input_file.read(reinterpret_cast<char*>(samples.data()), dataSize);
        if (input_file.gcount() != static_cast<std::streamsize>(dataSize)) {
            throw SoundProcessorException("Failed to read audio data");
        }
    }

void WavWorker::write(const std::string& path, const std::vector<int16_t>& samples) {
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        throw SoundProcessorException("Cannot create output file: " + path);
    }

    const auto data_size = static_cast<uint32_t>(samples.size() * sizeof(int16_t));
    const uint32_t file_size = AudioFormat::HEADER_SIZE + data_size;

    file.write("RIFF", AudioFormat::CHUNK_ID_SIZE);
    file.write(reinterpret_cast<const char*>(&file_size), AudioFormat::CHUNK_ID_SIZE);
    file.write("WAVE", AudioFormat::CHUNK_ID_SIZE);
    file.write("fmt ", AudioFormat::CHUNK_ID_SIZE);
    file.write(reinterpret_cast<const char*>(&AudioFormat::FMT_CHUNK_SIZE), 4);
    file.write(reinterpret_cast<const char*>(&AudioFormat::FORMAT_PCM), 2);
    file.write(reinterpret_cast<const char*>(&AudioFormat::CHANNELS), 2);
    file.write(reinterpret_cast<const char*>(&AudioFormat::SAMPLE_RATE), 4);
    file.write(reinterpret_cast<const char*>(&AudioFormat::BYTE_RATE), 4);
    file.write(reinterpret_cast<const char*>(&AudioFormat::BLOCK_ALIGN), 2);
    file.write(reinterpret_cast<const char*>(&AudioFormat::BITS_PER_SAMPLE), 2);
    file.write("data", AudioFormat::CHUNK_ID_SIZE);
    file.write(reinterpret_cast<const char*>(&data_size), AudioFormat::CHUNK_ID_SIZE);
    file.write(reinterpret_cast<const char*>(samples.data()), data_size);
}
