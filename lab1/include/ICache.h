#pragma once
#include <optional>

template<typename K, typename V>
class ICache {
public:
    virtual std::optional<V> get(const K& num) = 0;
    virtual void put(const K& num, const V& new_value) = 0;
    virtual V& operator[](const K& key) = 0;
    virtual ~ICache() = default;
};
