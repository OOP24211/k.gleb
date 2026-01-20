#include "../ConverterFactory.hpp"
#include "../Exceptions.hpp"
#include "../Mute.hpp"
#include "../Mix.hpp"
#include "../include/Gain.hpp"

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
