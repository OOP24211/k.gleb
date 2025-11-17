#include <iostream>
#include <list>
#include <unordered_map>
#include <tuple>
#include <optional>
#define CACHE_SIZE 10


template<typename K, typename V>
class ICache {
public:
    virtual std::optional<V> get(const K& num) = 0;
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


template<typename CacheType, typename  K = int, typename V = long long>
class FibCalculator {
    CacheType fib_cache;
public:
    template <typename... Args>
    explicit FibCalculator(Args&&... args) : fib_cache(std::forward<Args>(args)...) {
        fib_cache.put(0,0);
        fib_cache.put(1,1);
    }

    V calculate_fib(const K &n) {
        if (n<0) {
            throw std::out_of_range("n must be >0");
        }
        if (n == 0) return 0;
        if (n == 1) return 1;
        std::optional<V> result = fib_cache.get(n);
        if (result.has_value()) {
            return result.value();
        }
        V val1 = calculate_fib(n-1);
        V val2 = calculate_fib(n-2);

        V result_n = val1 + val2;

        fib_cache.put(n,result_n);
        return result_n;
    }

};

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
                long long const result = fib_calculator.calculate_fib(N);
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
            using CacheStrategy = LRUCache<int, long long>;
            Fibonacci_App<CacheStrategy> app(CACHE_SIZE);
            app.Run();
            break;
        }
        case 2: {
            using CacheStrategy = LFUCache<int, long long>;
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