#include "ConfigParser.hpp"
#include "Exceptions.hpp"
#include <fstream>
#include <sstream>

std::vector<std::pair<std::string, std::vector<std::string>>> parseConfig(const std::string &path) {
    std::ifstream file(path);
    if (!file) throw ConfigError("Cannot open config file: " + path);
    std::vector<std::pair<std::string, std::vector<std::string>>> commands;
    std::string line;
    size_t lineno = 0;
    while (std::getline(file, line)) {
        lineno++;
        if (size_t pos = line.find('#'); pos != std::string::npos) line.erase(pos);
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
