#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <windows.h>
#include "BufferManager.hpp"
#include "WordsExporter.hpp"
#include "WordStats.hpp"

int main(int argc, char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    if (argc != 3) {
        std::cerr << "Передано неправильное количество аргументов\n";
        return EXIT_FAILURE;
    }

    std::string const file_input_name = argv[1];
    std::string const file_output_name = argv[2];

    std::ifstream file_input(file_input_name);
    if (!file_input.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл\n";
        return EXIT_FAILURE;
    }

    std::map<std::string, Word_stats> Words_info;
    size_t total = 0;

    BufferManager manager(file_input, Words_info, total);
    manager.process_file();
    file_input.close();

    WordsExporter exporter(file_output_name, Words_info, total);
    exporter.calculate_frequency();
    exporter.export_to_csv();

    return EXIT_SUCCESS;
}
