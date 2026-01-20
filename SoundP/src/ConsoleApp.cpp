#include "ConsoleApp.hpp"
#include <vector>
#include <cstdint>
#include <iostream>
#include "Exceptions.hpp"
#include "WavWorker.hpp"
#include "ConfigParser.hpp"
#include "IConverter.hpp"
#include "ConverterFactory.hpp"



int ConsoleApp::run() {
    try {
        if (argc_ < 2) {
            throw ArgumentError("Not enough arguments. Use -h for help.");
        }
        if (std::string(argv_[1]) == "-h") {
            printHelp();
            return 0;
        }
        if (std::string(argv_[1]) != "-c" || argc_ < 5) {
            throw ArgumentError("Usage: sound_processor -c config.txt output.wav input1.wav [input2.wav ...]");
        }

        const std::string config_file = argv_[2];
        const std::string output_file = argv_[3];

        std::vector<std::string> input_files;
        for (int i = 4; i < argc_; ++i) {
            input_files.emplace_back(argv_[i]);
        }

        std::vector<int16_t> main_samples;
        WavWorker::read(input_files[0], main_samples);

        std::vector<std::vector<int16_t>> extra_streams;
        for (size_t i = 1; i < input_files.size(); ++i) {
            std::vector<int16_t> data;
            WavWorker::read(input_files[i], data);
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

        WavWorker::write(output_file, current);

    } catch (const SoundProcessorException& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
        return 2;
    }
    return 0;
}
