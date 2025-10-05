#pragma once

#include <string>
#include <map>
#include <fstream>
#include "WordStats.hpp"

class BufferManager {
    static constexpr size_t BUFFER_SIZE = (1024 * 1024);
    char buffer_[BUFFER_SIZE];
    std::string current_word_;

    std::ifstream& file_;
    std::map<std::string, Word_stats>& words_map_;
    size_t& total_words_;

    void add_word(std::string& word);
    void process_block(std::streamsize bytes_read_count);

public:
    BufferManager(std::ifstream& file,
                  std::map<std::string, Word_stats>& words_map,
                  size_t& total);
    void process_file();
};

