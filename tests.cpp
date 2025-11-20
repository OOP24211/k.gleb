#include <gtest/gtest.h>
#include "FibCalculator.h"
#include "LRUCache.h"
#include <limits>
#include <stdexcept>

template<typename CacheType>
using Calc = FibCalculator<CacheType, int, unsigned long long>;

// ---------------------------------------------------------------------------
// 1. Тесты базовых значений
TEST(FibCalculator_Basic, BaseCases) {
    Calc<LRUCache<int, unsigned long long>> fib(100);

    EXPECT_EQ(fib.calculate_fib(0), 0ULL);
    EXPECT_EQ(fib.calculate_fib(1), 1ULL);
    EXPECT_EQ(fib.calculate_fib(2), 1ULL);
    EXPECT_EQ(fib.calculate_fib(3), 2ULL);
    EXPECT_EQ(fib.calculate_fib(4), 3ULL);
    EXPECT_EQ(fib.calculate_fib(5), 5ULL);
}

// ---------------------------------------------------------------------------
// 2. Тесты корректности больших значений (до границы unsigned long long)
TEST(FibCalculator_Correctness, KnownValues) {
    Calc<LRUCache<int, unsigned long long>> fib(1000);

    EXPECT_EQ(fib.calculate_fib(10), 55ULL);
    EXPECT_EQ(fib.calculate_fib(20), 6765ULL);
    EXPECT_EQ(fib.calculate_fib(30), 832040ULL);
    EXPECT_EQ(fib.calculate_fib(50), 12586269025ULL);
    EXPECT_EQ(fib.calculate_fib(93), 12200160415121876738ULL);
}


// ---------------------------------------------------------------------------
// 3. Отрицательный аргумент → исключение
TEST(FibCalculator_Exceptions, NegativeArgument) {
    Calc<LRUCache<int, unsigned long long>> fib(100);

    EXPECT_THROW(fib.calculate_fib(-1), std::out_of_range);
    EXPECT_THROW(fib.calculate_fib(-100), std::out_of_range);
}

// ---------------------------------------------------------------------------
// 4. Кэширование работает → повторный вызов мгновенный и тот же результат
TEST(FibCalculator_Cache, HitWorks) {
    Calc<LRUCache<int, unsigned long long>> fib(10);

    unsigned long long r1 = fib.calculate_fib(20);
    unsigned long long r2 = fib.calculate_fib(20);
    unsigned long long r3 = fib.calculate_fib(20);

    EXPECT_EQ(r1, r2);
    EXPECT_EQ(r2, r3);
}

// ---------------------------------------------------------------------------
// 5. Тест с LFU

#include "LFUCache.h"

TEST(FibCalculator_Strategy, LFU_WorksSameAsLRU) {
    Calc<LRUCache<int, unsigned long long>> fib_lru(1000);
    Calc<LFUCache<int, unsigned long long>> fib_lfu(1000);

    EXPECT_EQ(fib_lru.calculate_fib(40), fib_lfu.calculate_fib(40));
    EXPECT_EQ(fib_lru.calculate_fib(60), fib_lfu.calculate_fib(60));
}


// ---------------------------------------------------------------------------
// Запуск всех тестов
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}