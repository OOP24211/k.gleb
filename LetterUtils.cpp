#include "LetterUtils.hpp"

namespace LetterUtils {

    bool is_letter(unsigned char const symbol) {
        return
            (symbol >= 'A' && symbol <= 'Z') ||
            (symbol >= 'a' && symbol <= 'z') ||
            (symbol >= 0xC0 && symbol <= 0xDF) || // заглавные кириллические
            (symbol >= 0xE0 && symbol <= 0xFF) || // строчные кириллические
            symbol == 0xA8 || symbol == 0xB8;     // Ё, ё
    }

    void to_low_reg(unsigned char& symbol) {
        if (symbol >= 'A' && symbol <= 'Z') {
            symbol += 32;
        } else if (symbol >= 0xC0 && symbol <= 0xDF) {
            symbol += 0x20;
        } else if (symbol == 0xA8) {
            symbol = 0xB8;
        }
    }

}