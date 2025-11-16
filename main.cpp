#include <iostream>
#include <list>
#include <unordered_map>
#include <tuple>
#include <optional>

template<typename K, typename V>
class ICache {
public:
    virtual V get(const K& num) = 0;
    virtual void put(const K& num, const V& new_value) = 0;
    virtual V& operator[](const K& key) = 0;
    virtual ~ICache() = default;
};

template<typename K, typename V>
class LRUCache : public ICache<K,V> {
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> cache;
    std::list<std::pair<K, V>> lru_list;
    const size_t capacity;
    void update_lru_pos(std::list<std::pair<K,V>>::iterator list_iter) {
        lru_list.splice(lru_list.begin(), lru_list, list_iter);

    }

public:
   explicit  LRUCache(size_t cap) : capacity(cap) {}

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
        else {
            put(num,V());
            auto list_iter = cache[num];
            return list_iter->second;
        }


    }
};
template<typename K, typename V>
class LFUCache : public ICache<K,V> {
    std::unordered_map<int, std::tuple<int, int, std::list<int>::iterator>> cache;
    std::unordered_map<int, std::list<int>> freq_list;
    const size_t capacity;
    int min_freq;

public:
    explicit LFUCache(size_t cap) : capacity(cap), min_freq(0) {}

    void put(int num, int new_val) override {
        if (cache.contains(num)) {
            auto& [value, freq, list_iter] = cache[num];
            value = new_val;
            freq_list[freq].erase(list_iter);
            if (freq == min_freq && freq_list[freq].empty()) {
                min_freq = freq + 1;
            }
            freq++;
            freq_list[freq].push_front(num);
            cache[num] = {value, freq, freq_list[freq].begin()};
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

    int get(int num) override {
        if (!cache.contains(num)) return -1;
        auto& [value, freq, list_iter] = cache[num];
        freq_list[freq].erase(list_iter);
        if (freq == min_freq && freq_list[freq].empty()) {
            min_freq = freq + 1;
        }
        freq++;
        freq_list[freq].push_front(num);
        cache[num] = {value, freq, freq_list[freq].begin()};
        return value;
    }
};

int main() {

    return 0;
}