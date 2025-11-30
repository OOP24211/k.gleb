#pragma once
#include <string>
#include <iostream>
#include "FibCalculator.h"

template <typename CacheType>
class Fibonacci_App {
    FibCalculator<CacheType> fib_calculator;
public:
    explicit Fibonacci_App(size_t capacity) : fib_calculator(capacity) {}
    void Run(){
        std::cout << "print 'n' or 'exit' for exit:"<< std::endl;
        std::string input;
        while (true) {
            std::cout << "> ";
            std::getline(std::cin,input);
            if (input == "exit") break;
            try {
                int N = std::stoi(input);
                unsigned long long const result = fib_calculator.calculate_fib(N);
                std::cout << "F("<<N<<") = " << result<<std::endl;

            }
            catch (const std::invalid_argument&) {
                std::cout << "error: invalid format arg" << std::endl;
            } catch (const std::out_of_range& e) {
                std::cout << "out of range: " << e.what() << std::endl;
            }


        }

    }
};