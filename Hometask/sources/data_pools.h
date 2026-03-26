#pragma once
#include "data_pool.h"
#include <vector>

constexpr size_t DEFAULT_POOL_CAPACITY = 3;

struct DataPools {
    DataPool<std::vector<int>> intPool;
    DataPool<std::vector<float>> floatPool;
    DataPool<std::vector<double>> doublePool;

    explicit DataPools(size_t capacity = DEFAULT_POOL_CAPACITY) 
        : intPool(capacity)
        , floatPool(capacity)
        , doublePool(capacity)
    {}

    template<typename T>
    DataPool<std::vector<T>>& get() {
        if constexpr (std::is_same_v<T, int>) {
            return intPool;
        } else if constexpr (std::is_same_v<T, float>) {
            return floatPool;
        } else if constexpr (std::is_same_v<T, double>) {
            return doublePool;
        } else {
            static_assert(sizeof(T) == 0, "Unsupported type");
        }
    }
    
    template<typename T>
    const DataPool<std::vector<T>>& get() const {
        if constexpr (std::is_same_v<T, int>) {
            return intPool;
        } else if constexpr (std::is_same_v<T, float>) {
            return floatPool;
        } else if constexpr (std::is_same_v<T, double>) {
            return doublePool;
        } else {
            static_assert(sizeof(T) == 0, "Unsupported type");
        }
    }
};