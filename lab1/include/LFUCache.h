#pragma once
#include <unordered_map>
#include <tuple>
#include <list>
#include "ICache.h"
template<typename K, typename V>
class LFUCache : public ICache<K,V> {

    std::unordered_map<K, std::tuple<V, int, typename std::list<K>::iterator>> cache;
    std::unordered_map<int, std::list<K>> freq_list;
    const size_t capacity;
    int min_freq;
    void increment_freq(const K& num){
        auto& [value, current_freq, list_iter] = cache[num];
        freq_list[current_freq].erase(list_iter);
        if (current_freq == min_freq && freq_list[current_freq].empty()) {
            min_freq = current_freq+1;
        }
        ++current_freq;
        freq_list[current_freq].emplace_front(num);
        std::get<2>(cache[num]) = freq_list[current_freq].begin();
    }

public:
    explicit LFUCache(size_t cap) : capacity(cap), min_freq(0) {}

    void put(const K &num, const V& new_val) override {
        if (cache.contains(num)) {
            std::get<0>(cache[num]) = new_val;
            increment_freq(num);
        } else {
            if (cache.size() >= capacity) {
                int old_num = freq_list[min_freq].back();
                freq_list[min_freq].pop_back();
                cache.erase(old_num);
                if (freq_list[min_freq].empty()) {
                    freq_list.erase(min_freq);
                }
            }
            freq_list[1].push_front(num);
            cache[num] = std::make_tuple(new_val, 1, freq_list[1].begin());
            min_freq = 1;
        }
    }

    std::optional<V> get(const K& num) override {
        if (!cache.contains(num)) return std::nullopt;
        increment_freq(num);
        auto& [value, current_freq, list_iter] = cache[num];
        return value;
    }
    V& operator[](const K& num) override {
        if (cache.contains(num)) {
            increment_freq(num);
            auto& [value, current_freq, list_iter] = cache[num];
            return value;
        }
        put(num,V());
        auto& [value, current_freq, list_iter] = cache[num];
        return value;
    }
};