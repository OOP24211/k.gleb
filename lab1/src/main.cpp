#include <iostream>
#include <string>
#include "LRUCache.h"
#include "LFUCache.h"
#include "Fibonacci_App.h"
#define CACHE_SIZE (10*1024)


int main() {
    std::string opt_str;
    int opt = 0;
    std::cout<<"Choose Cache strategy (1 - LRU, 2 - LFU): "<<std::endl;
    std::getline(std::cin, opt_str);
    try {
        opt = std::stoi(opt_str);
    }
    catch (const std::invalid_argument&) {
        std::cout <<"error: must be int number"<< std::endl;
        return EXIT_FAILURE;
    }
    switch (opt) {
        case 1: {
            using CacheStrategy = LRUCache<int, unsigned long long>;
            Fibonacci_App<CacheStrategy> app(CACHE_SIZE);
            app.Run();
            break;
        }
        case 2: {
            using CacheStrategy = LFUCache<int, unsigned long long>;
            Fibonacci_App<CacheStrategy> app(CACHE_SIZE);
            app.Run();
            break;
        }
        default:
            std::cout<< "Not correct option"<< std::endl;
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}