#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <sstream>
#include <string>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include "AudioFormat.hpp"


class SoundProcessorException : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class ConfigError : public SoundProcessorException {
public:
    using SoundProcessorException::SoundProcessorException;
};

class ArgumentError : public SoundProcessorException {
public:
    using SoundProcessorException::SoundProcessorException;
};

#pragma pack(push, 1)
struct WavHeader {
    char riff[AudioFormat::CHUNK_ID_SIZE];
    uint32_t fileSize;
    char wave[AudioFormat::CHUNK_ID_SIZE];

};
#pragma pack(pop)

void read_wav(const std::string& path, std::vector<int16_t>& samples) {
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


void write_wav(const std::string& path, const std::vector<int16_t>& samples) {
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




class IConverter {
public:
    virtual ~IConverter() = default;
    virtual void process(const std::vector<int16_t>& in, std::vector<int16_t>& out) = 0;
};




class Mute : public IConverter {
    size_t start_sample_, end_sample_;
public:
    Mute(double start_sec, double end_sec)
        : start_sample_(static_cast<size_t>(start_sec * AudioFormat::SAMPLE_RATE)),
          end_sample_(static_cast<size_t>(end_sec * AudioFormat::SAMPLE_RATE)) {}

    void process(const std::vector<int16_t>& in, std::vector<int16_t>& out) override {
        out = in;
        for (size_t i = 0; i < in.size(); ++i) {
            if (i >= start_sample_ && i < end_sample_) {
                out[i] = 0;
            }
        }
    }
};

class Mix : public IConverter {
    std::vector<int16_t> mix_data_;
    size_t offset_samples_;

public:
    Mix(const std::vector<int16_t>& source, double offset_sec)
        : mix_data_(source),
          offset_samples_(static_cast<size_t>(offset_sec * AudioFormat::SAMPLE_RATE)) {}

    void process(const std::vector<int16_t>& in, std::vector<int16_t>& out) override {
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
};


class Gain : public IConverter {
    float factor_;

public:
    explicit Gain(const float factor) : factor_(factor) {}

    void process(const std::vector<int16_t>& in, std::vector<int16_t>& out) override {
        out.resize(in.size());
        for (size_t i = 0; i < in.size(); ++i) {
            float scaled = static_cast<float>(in[i]) * factor_;
            if (scaled > 32767.0f) scaled = 32767.0f;
            else if (scaled < -32768.0f) scaled = -32768.0f;
            out[i] = static_cast<int16_t>(scaled);
        }
    }
};


// Фабрика
std::unique_ptr<IConverter> createConverter(const std::string& name, const std::vector<std::string>& args,const std::vector<std::vector<int16_t>>& extra_streams) {
    if (name == "mute") {
        if (args.size() != 2) throw ConfigError("mute requires 2 args: start_sec end_sec");
        return std::make_unique<Mute>(std::stod(args[0]), std::stod(args[1]));
    }
    if (name == "mix") {
        if (args.size() != 2) throw ConfigError("mix requires 2 args: $N offset_sec");
        if (args[0].empty() || args[0][0] != '$') throw ConfigError("first arg of mix must be $N");
        const int n = std::stoi(args[0].substr(1));
        if (n < 2) throw ConfigError("mix can only reference $2, $3, ...");
        const auto idx = static_cast<size_t>(n - 2);
        if (idx >= extra_streams.size()) {
            throw ConfigError("referenced file $" + std::to_string(n) + " not provided");
        }
        return std::make_unique<Mix>(extra_streams[idx], std::stod(args[1]));
    }
    if (name == "gain") {
        if (args.size() != 1) throw ConfigError("gain requires 1 arg: factor");
        return std::make_unique<Gain>(std::stof(args[0]));
    }
    throw ConfigError("Unknown converter: " + name);
}

// Парсер конфига
std::vector<std::pair<std::string, std::vector<std::string>>> parseConfig(const std::string &path) {
    std::ifstream file(path);
    if (!file) throw ConfigError("Cannot open config file: " + path);
    std::vector<std::pair<std::string, std::vector<std::string>>> commands;
    std::string line;
    size_t lineno = 0;
    while (std::getline(file, line)) {
        lineno++;
        size_t pos = line.find('#');
        if (pos != std::string::npos) line.erase(pos);
        line.erase(0, line.find_first_not_of(" \t"));
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        std::vector<std::string> args;
        std::string arg;
        while (iss >> arg) args.push_back(arg);
        commands.emplace_back(cmd, args);
    }
    return commands;
}


void printHelp() {
    std::cout << "Usage: sound_processor -c config.txt output.wav input.wav\n";
    std::cout << "Converters:\n";
    std::cout << "  mute start_sec end_sec — silence interval\n";
    std::cout << "  mix $N offset_sec — mix with Nth input file (N>=2)\n";

}


int main(const int argc, char* argv[]) {
    try {
        if (argc < 2) {
            throw ArgumentError("Not enough arguments. Use -h for help.");
        }
        if (std::string(argv[1]) == "-h") {
            printHelp();
            return 0;
        }
        if (std::string(argv[1]) != "-c" || argc < 5) {
            throw ArgumentError("Usage: sound_processor -c config.txt output.wav input1.wav [input2.wav ...]");
        }


        const std::string config_file = argv[2];
        const std::string output_file = argv[3];

        std::vector<std::string> input_files;
        for (int i = 4; i < argc; ++i) {
            input_files.emplace_back(argv[i]);
        }
        std::vector<int16_t> main_samples;
        read_wav(input_files[0], main_samples);

        std::vector<std::vector<int16_t>> extra_streams;
        for (size_t i = 1; i < input_files.size(); ++i) {
            std::vector<int16_t> data;
            read_wav(input_files[i], data);
            extra_streams.push_back(std::move(data));
        }

        auto commands = parseConfig(config_file);
        std::vector<std::unique_ptr<IConverter>> converters;
        converters.reserve(commands.size());
        for (auto& [cmd, args] : commands) {
            converters.push_back(createConverter(cmd, args, extra_streams));
        }


        std::vector<int16_t> current = main_samples;
        for (const auto& conv : converters) {
            std::vector<int16_t> next;
            conv->process(current, next);
            current = std::move(next);
        }

        write_wav(output_file, current);

    } catch (const SoundProcessorException& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
        return 2;
    }
    return 0;
}
