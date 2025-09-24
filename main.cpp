#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <windows.h>
#include <iostream>
#include <map>
#define BUFFER_SIZE  (1024 * 1024) // 1 MB

struct Word_stats {
    std::string word;
    int cnt = 0; // количество вхождений слова
    double frequency = 0.0;
};

struct Buffer {
    std::ifstream* file_in;           // указатель на входной файл
    std::map<std::string, Word_stats>* words_map;
    size_t* total_words;                 // указатель на счётчик всех слов в тексте
    std::string current_word;
    char buffer[BUFFER_SIZE];         // буфер чтения
           // текущее собираемое слово
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







void init_buffer(Buffer& buff, std::ifstream& file, std::map<std::string, Word_stats>& words_map, size_t& total) {
    buff.file_in = &file;
    buff.words_map = &words_map;
    buff.total_words = &total;
    buff.current_word.clear();
}

void add_word(std::string& word, const Buffer& buff) {
    if (word.empty() || !buff.words_map) return;

    auto& map = *buff.words_map;
    if (auto const is_found= map.find(word); is_found != map.end()) {
        is_found->second.cnt++;
    } else {
        map[word].cnt++;
    }
    if (buff.total_words) {
        (*buff.total_words)++;
    }
    word.clear();
}


void process_block(Buffer& buff, std::streamsize const bytes_read_count) {
    std::string& current_word = buff.current_word;

    for (int j = 0; j < bytes_read_count; ++j) {

        if (auto symbol = static_cast<unsigned char>(buff.buffer[j]);LetterUtils::is_letter(symbol)){
            LetterUtils::to_low_reg(symbol);
            current_word += static_cast<char>(symbol);
            } else {
                add_word(current_word, buff);
            }
    }
}

void process_file(Buffer& buff) {
    std::ifstream& file_in = *buff.file_in;

    while (file_in.read(buff.buffer, BUFFER_SIZE)) {
        std::streamsize const bytes_read_count = file_in.gcount();
        process_block(buff, bytes_read_count);
    }

    // Обработать остаток (если файл закончился не на границе буфера)
    if (std::streamsize const bytes_read_count = file_in.gcount(); bytes_read_count > 0) {
        process_block(buff, bytes_read_count);
    }

    // Обработать последнее слово, если осталось
    add_word(buff.current_word, buff);
    buff.current_word.clear();
}

void calculate_frequency(Buffer& buff, std::map<std::string, Word_stats>& Words_info) {
    for (auto& [word, Word_stats] : Words_info) {
        Word_stats.frequency = static_cast<double>(Word_stats.cnt) /  static_cast<double>(*buff.total_words);
    }
}


void sort_and_input_toCSV(const std::string& file_output, const std::map<std::string, Word_stats>& Words_info) {
    std::vector<std::pair<std::string, Word_stats>> vec(Words_info.begin(), Words_info.end());
    std::sort(vec.begin(), vec.end(), [](const auto& a, const auto& b) {
        return a.second.cnt > b.second.cnt;
    });
    std::ofstream file(file_output);
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
    Buffer buff;
    init_buffer(buff, file_input, Words_info, total);
    process_file(buff);
    file_input.close();
    calculate_frequency(buff,Words_info);
    sort_and_input_toCSV(file_output_name, Words_info);

    return EXIT_SUCCESS;
}

