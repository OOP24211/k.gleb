#pragma once
#include <iostream>
#include "ICache.h"

template<typename CacheType, typename  K = int, typename V =unsigned long long>
class FibCalculator {
    CacheType fib_cache;
public:
    template <typename... Args>
    explicit FibCalculator(Args&&... args) : fib_cache(std::forward<Args>(args)...) {
        fib_cache.put(0,0);
        fib_cache.put(1,1);
    }

    V calculate_fib(const K &n) {
        if (n<0) throw std::out_of_range("n must be >0");

        std::optional<V> result = fib_cache.get(n);
        if (result.has_value()) {
            return result.value();
        }
        V result_n = calculate_fib(n-1) + calculate_fib(n-2);

        fib_cache.put(n,result_n);
        return result_n;
    }

};