#include "BufferManager.hpp"
#include "LetterUtils.hpp"
#include <iostream>

void BufferManager::add_word(std::string& word) {
    if (word.empty()) return;

    auto& map = words_map_;
    auto it = map.find(word);
    if (it != map.end()) {
        it->second.cnt++;
    } else {
        map[word].cnt = 1;
    }
    ++total_words_;
    word.clear();
}

void BufferManager::process_block(std::streamsize const bytes_read_count) {
    for (std::streamsize j = 0; j < bytes_read_count; ++j) {
        unsigned char symbol = static_cast<unsigned char>(buffer_[j]);
        if (LetterUtils::is_letter(symbol)) {
            LetterUtils::to_low_reg(symbol);
            current_word_ += static_cast<char>(symbol);
        } else {
            add_word(current_word_);
        }
    }
}

BufferManager::BufferManager(std::ifstream& file,
                             std::map<std::string, Word_stats>& words_map,
                             size_t& total)
    : file_(file), words_map_(words_map), total_words_(total) {}

void BufferManager::process_file() {
    while (file_.read(buffer_, BUFFER_SIZE)) {
        std::streamsize const bytes_read_count = file_.gcount();
        process_block(bytes_read_count);
    }

    if (std::streamsize const bytes_read_count = file_.gcount(); bytes_read_count > 0) {
        process_block(bytes_read_count);
    }

    add_word(current_word_);
}