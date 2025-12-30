// WordsExporter.cpp
#include "WordsExporter.hpp"
#include <vector>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>

WordsExporter::WordsExporter(std::string const& file,
                             std::map<std::string, Word_stats>& words_map,
                             size_t& total)
    : file_(file), words_map_(words_map), total_words_(total) {}

void WordsExporter::calculate_frequency() {
    for (auto& [word, stats] : words_map_) {
        stats.frequency = static_cast<double>(stats.cnt) / static_cast<double>(total_words_);
    }
}

void WordsExporter::export_to_csv() {
    std::vector<std::pair<std::string, Word_stats>> vec(words_map_.begin(), words_map_.end());
    std::sort(vec.begin(), vec.end(), [](const auto& a, const auto& b) {
        return a.second.cnt > b.second.cnt;
    });

    std::ofstream file(file_);
    if (!file.is_open()) {
        std::cerr << "Ошибка при открытии файла вывода результата\n";
        return;
    }

    file << "Word,Count,Frequency\n";
    for (const auto& [word, data] : vec) {
        file << word << "," << data.cnt << ",";
        file << std::fixed << std::setprecision(2) << (data.frequency * 100) << "%\n";
    }
    file.close();
}
