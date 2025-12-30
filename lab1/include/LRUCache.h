#pragma once
#include <unordered_map>
#include <list>
#include "ICache.h"

template<typename K, typename V>
class LRUCache : public ICache<K,V> {
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> cache;
    std::list<std::pair<K, V>> lru_list;
    const size_t capacity;
    void update_lru_pos(std::list<std::pair<K,V>>::iterator list_iter) {
        lru_list.splice(lru_list.begin(), lru_list, list_iter);

    }

public:
    explicit  LRUCache(size_t const cap) : capacity(cap) {}

    void put(const K& num, const V& new_val) override {
        if (cache.contains(num)) {
            auto list_iter = cache[num];
            list_iter->second = new_val;
            update_lru_pos(list_iter);
        } else {
            if (cache.size() >= capacity) {
                K old_num = lru_list.back().first;
                lru_list.pop_back();
                cache.erase(old_num);
            }
            lru_list.emplace_front(num, new_val);
            cache[num] = lru_list.begin();
        }
    }

    std::optional<V> get(const K& num) override {
        if (!cache.contains(num)) return std::nullopt;
        auto list_iter = cache[num];
        update_lru_pos(list_iter);
        return list_iter->second;
    }
    V& operator[](const K& num) override {
        if (cache.contains(num)) {
            auto list_iter = cache[num];
            update_lru_pos(list_iter);
            return list_iter->second;
        }
        put(num,V());
        auto list_iter = cache[num];
        return list_iter->second;
    }
};