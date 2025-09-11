#include <algorithm>
#include <iostream>
#include <locale>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <cctype>
#include <cmath>
#include <vector>
#include <iomanip>
#include <windows.h>

struct Words_data {
    std::size_t word_count;
    double per;
};

// Проверка на то, является ли элемент спец символом
bool is_punct_char(const char32_t ch) {
    static const char32_t punct[] = {
        U'?', U',', U'!', U'.', U':', U';', U'(', U')',
        U'"', U'«', U'»', U'“', U'”', U'‘', U'’',
        U'—', U'–', U'…', U'-', 0
    };

    for (int i = 0; punct[i] != 0; ++i) {
        if (ch == punct[i]) return true;
    }
    return false;
}

// очистка от спец символов
std::string clean_utf8(const std::string &input) {
    std::string output;
    size_t i = 0;

    while (i < input.size()) {
        unsigned char c = input[i];
        int len;

        // Определяем длину символа UTF-8
        if ((c & 0x80) == 0) len = 1;
        else if ((c & 0xE0) == 0xC0) len = 2;
        else if ((c & 0xF0) == 0xE0) len = 3;
        else if ((c & 0xF8) == 0xF0) len = 4;
        else {
            i++;
            continue;
        } // Пропускаем невалидные байты

        if (i + len > input.size()) break;

        // Собираем codepoint
        char32_t cp = 0;
        if (len == 1) cp = c;
        else if (len == 2) cp = ((c & 0x1F) << 6) | (input[i + 1] & 0x3F);
        else if (len == 3) cp = ((c & 0x0F) << 12) | ((input[i + 1] & 0x3F) << 6) | (input[i + 2] & 0x3F);
        else if (len == 4)
            cp = ((c & 0x07) << 18) | ((input[i + 1] & 0x3F) << 12) | ((input[i + 2] & 0x3F) << 6) | (
                     input[i + 3] & 0x3F);

        // Проверяем, является ли символ пунктуацией
        if (is_punct_char(cp)) {
            output += ' ';
        } else {
            // Добавляем все байты символа
            output.append(input.begin() + i, input.begin() + i + len);
        }

        i += len;
    }

    return output;
}


void sort_and_input_toCSV(const std::string &filename, const std::map<std::string, Words_data> &words_data) {
    std::vector<std::pair<std::string, Words_data> > vec(
        words_data.begin(), words_data.end()
    );
    std::sort(vec.begin(), vec.end(), [](const auto &a, const auto &b) {
        return a.second.per > b.second.per;
    });
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Ошибка при открытии файла вывода результата" << std::endl;
        return;
    }
    //file << "\xEF\xBB\xBF"; -> строка, если csv открывается в Excel (BOM)
    file << "Слово,Количество повторений,Процент\n";
    for (const auto &[word, data]: vec) {
        file << word << "," << data.word_count << ","
                << std::fixed << std::setprecision(2) << data.per << "%" << "\n";
    }
    file.close();
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::ifstream file("input.txt");
    if (!file.is_open()) {
        std::cerr << "Ошибка при открытии исходного файла " << std::endl;
        return EXIT_FAILURE;
    }

    std::map<std::string, Words_data> words_data;
    std::string line;
    double words_count = 0.0;

    while (std::getline(file, line)) {
        line = clean_utf8(line);
        std::stringstream ss(line);
        std::string word;
        while (ss >> word) {
            words_count += 1;
            if (!word.empty()) {
                words_data[word].word_count++;
            }
        }
    }
    file.close();

    if (words_count != 0.0) {
        for (const auto &[key, value]: words_data) {
            words_data[key].per = ((static_cast<double>(words_data[key].word_count) / words_count) * 100);
        }
        sort_and_input_toCSV("output.csv", words_data);
        std::cout << "Успешно!" << std::endl;
        return EXIT_SUCCESS;
    } else {
        std::cout << "Ошибка! Файл пуст" << std::endl;
        return EXIT_FAILURE;
    }
}
