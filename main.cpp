#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <windows.h>
#include <iostream>
#include <map>


struct Word_stats {
    int cnt = 0; // количество вхождений слова
    double frequency = 0.0;
};

namespace LetterUtils {
    static bool is_letter(unsigned char const symbol) {
        return
            (symbol >= 'A' && symbol <= 'Z') ||
            (symbol >= 'a' && symbol <= 'z') ||
            (symbol >= 0xC0 && symbol <= 0xDF) || // заглавные кириллические
            (symbol >= 0xE0 && symbol <= 0xFF) || // строчные кириллические
            symbol == 0xA8 || symbol == 0xB8;   // Ё, ё

    }
    static void to_low_reg(unsigned char& symbol) {
        if (symbol >= 'A' && symbol <= 'Z') {
            symbol += 32;
        } else if (symbol >= 0xC0 && symbol <= 0xDF) {
            symbol += 0x20;
        } else if (symbol == 0xA8) {
            symbol = 0xB8;
        }
    }
};



class BufferManager {
    static constexpr size_t BUFFER_SIZE = (1024*1024);

    char buffer_[BUFFER_SIZE];
    std::string current_word_;

    std::ifstream& file_;
    std::map<std::string, Word_stats>& words_map_;
    size_t& total_words_;
     void add_word(std::string& word) {
        if (word.empty()) return;

        auto& map = words_map_;
        if (auto const is_found= map.find(word); is_found != map.end()) {
            is_found->second.cnt++;
        } else {
            map[word].cnt++;
        }
         (total_words_)++;

        word.clear();
    }
     void process_block(std::streamsize const bytes_read_count) {
        for (int j = 0; j < bytes_read_count; ++j) {

            if (auto symbol = static_cast<unsigned char>(buffer_[j]);LetterUtils::is_letter(symbol)){
                LetterUtils::to_low_reg(symbol);
                current_word_ += static_cast<char>(symbol);
            } else {
                add_word(current_word_);
            }
        }
    }
public:
    BufferManager(std::ifstream& file,
                  std::map<std::string, Word_stats>& words_map,
                  size_t& total)
    :  file_(file), words_map_(words_map), total_words_(total),buffer_{} {}
    void process_file() {
        while (file_.read(buffer_, BUFFER_SIZE)) {
            std::streamsize const bytes_read_count = file_.gcount();
            process_block(bytes_read_count);
        }

        // Обработать остаток (если файл закончился не на границе буфера)
        if (std::streamsize const bytes_read_count = file_.gcount(); bytes_read_count > 0) {
            process_block(bytes_read_count);
        }

        // Обработать последнее слово, если осталось
        add_word(current_word_);
        current_word_.clear();
    }


};

class WordsExporter {
    std::string const file_;
    std::map<std::string, Word_stats>& words_map_;
    size_t& total_words_;

public:
    WordsExporter(std::string const &file,
              std::map<std::string, Word_stats>& words_map,
              size_t& total)
:  file_(file), words_map_(words_map), total_words_(total) {}
    void calculate_frequency() {
        for (auto& [word, Word_stats] : words_map_) {
            Word_stats.frequency = static_cast<double>(Word_stats.cnt) /  static_cast<double>(total_words_);
        }
    }

    void export_to_csv() {
        std::vector<std::pair<std::string, Word_stats>> vec(words_map_.begin(), words_map_.end());
        std::sort(vec.begin(), vec.end(), [](const auto& a, const auto& b) {
            return a.second.cnt > b.second.cnt;
        });
        std::ofstream file(file_);
        if (!file.is_open()) {
            std::cerr << "Ошибка при открытии файла вывода результата" << std::endl;
            return;
        }
        file << "Word,Count,Frequency\n";

        for (const auto& [word, data] : vec) {
            file << word << "," << data.cnt << ",";
            file << std::fixed << std::setprecision(2) << data.frequency * 100 << "%\n";
        }
        file.close();
    }


};











int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8); // Для консоли
    SetConsoleCP(CP_UTF8);
    if (argc != 3) {
        std::cerr << "Передано неправильное количество аргументов";
        return EXIT_FAILURE;
    }
    std::string const file_input_name = argv[1];
    std::string const file_output_name = argv[2];
    std::ifstream file_input(file_input_name);
    if (!file_input.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл\n";
        return EXIT_FAILURE;
    }
    std::map<std::string,Word_stats> Words_info;
    size_t total = 0;
    BufferManager(file_input, Words_info, total).process_file();
    file_input.close();
    WordsExporter(file_output_name, Words_info, total).calculate_frequency();
    WordsExporter(file_output_name, Words_info, total).export_to_csv();

    return EXIT_SUCCESS;
}

