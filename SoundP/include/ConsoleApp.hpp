#pragma once
#include <iostream>


class ConsoleApp {
    static void printHelp() {
        std::cout << "Usage: sound_processor -c config.txt output.wav input.wav\n";
        std::cout << "Converters:\n";
        std::cout << "  mute start_sec end_sec - silence interval\n";
        std::cout << "  mix $N offset_sec - mix with Nth input file (N>=2)\n";
        std::cout << "  gain factor - adjust volume (e.g., 0.5 = half, 2.0 = double)\n";
    }
    int argc_;
    char** argv_;

public:
    explicit ConsoleApp(int argc, char* argv[]) : argc_(argc), argv_(argv) {}
    int run();
};
