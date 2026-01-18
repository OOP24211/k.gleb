#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "iostream"

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
    char riff[4];        // "RIFF"
    uint32_t fileSize;   // fileSize + 8
    char wave[4];        // "WAVE"
    char fmt[4];         // "fmt "
    uint32_t fmtSize;    // 16 for PCM
    uint16_t format;     // 1 = PCM
    uint16_t channels;   // 1 = mono
    uint32_t sampleRate; // 44100
    uint32_t byteRate;   // sampleRate * channels * bits/8
    uint16_t blockAlign; // channels * bits/8
    uint16_t bitsPerSample; // 16
    char data[4];        // "data"
    uint32_t dataSize;   // number of bytes in data
};
#pragma pack(pop)

class IConverter {
public:
    virtual ~IConverter() = default;
    virtual void process (
        const std::vector<int16_t>& inSamples, std::vector<int16_t>& outSamples,
        const std::vector<std::vector<int16_t>>& extra_streams,
        size_t global_sample_offset) =0;
    virtual std::string name() const =0;
    virtual std::string help() const =0;
};

class Mute : public IConverter {
    size_t start_sample, end_sample;
public:
    Mute(double start_sec, double end_sec):
        start_sample(static_cast<size_t>(start_sec*44100)),
        end_sample(static_cast<size_t>(end_sec*44100)){}
    void process(const std::vector<int16_t>& inSamples, std::vector<int16_t>& outSamples,
        const std::vector<std::vector<int16_t>>& /*extra*/,
        size_t global_offset) override {
        outSamples.resize(inSamples.size());
        for (size_t i = 0; i < inSamples.size(); ++i) {
            size_t pos = global_offset + i;
            if (pos >= start_sample && pos < end_sample) {
                outSamples[i] = 0;
            }else {
                outSamples[i] = inSamples[i];
            }
        }
    }
    std::string name() const override {return "Mute";}
    std::string help() const override {return "Mute start_sec end_sec — silence interval";}

};



std:: unique_ptr<IConverter> createCoverter(const std::string& name,const std::vector<std::string>& args) {
    if (name == "Mute") {
        if (args.size() != 2) throw ConfigError("mute requires 2 args: start_sec end_sec");
        return std::make_unique<Mute>(std::stod(args[0]),std::stod(args[1]));
    }
    throw ConfigError("Unknown converter: " + name);
};
std::vector<std::unique_ptr<IConverter>> read_config(const std::string& config_path,size_t num_inputs) {
    std::ifstream file(config_path);
    if (!file) throw ConfigError(std::string("Cannot open config file"));
    std::vector<std::unique_ptr<IConverter>> converters;
    std::string line;
    size_t line_number = 0;
    while (std::getline(file, line)) {
        line_number++;
        size_t comment = line.find('#');
        if (comment != std::string::npos) line.erase(comment);
        line.erase(0,line.find_first_not_of('\t'));
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;
        if (cmd.empty()) continue;
        std::vector<std::string> args;
        std::string arg;
        while (iss>>arg) {
            args.push_back(arg);
        }
        try {
            converters.push_back(createCoverter(cmd,args));
        } catch (const std::exception & e) {
            throw ConfigError("Line " + std::to_string(line_number) + ": " + e.what());
        }

    }
    return converters;

}

void printHelp() {
    std::cout << "Converters:\n";
    std::cout << "  mute " << Mute(0,0).help() << "\n";
}

int main(int argc,char* argv[]) {
    try {
        if (argc < 2) {
            throw ArgumentError("Not enough arguments. Use -h for help.");
        }
        if (std::string(argv[1]) == "-h") {
            printHelp();
            return 0;
        }

        std::string config_file = argv[2];
        std::string output_file = argv[3];
        std::string input_file = argv[4];

    }


}