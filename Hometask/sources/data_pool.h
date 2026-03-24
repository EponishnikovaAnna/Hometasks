#pragma once
#include <queue>
#include <stdexcept>
#include <vector>
#include <type_traits>
#include <limits>
#include "loger.h"

template<typename T>
class DataPool
{
    static_assert(std::is_same_v<T, std::vector<int>> ||
                  std::is_same_v<T, std::vector<float>> ||
                  std::is_same_v<T, std::vector<double>>,
                  "DataPool supports only vector<int>, vector<float>, vector<double>");
    
public:
    DataPool(size_t cap) : capacity(cap)
    {
        if(capacity == 0){
            LOG_ERROR("DataPool constructor: capacity is 0");
            throw std::invalid_argument("Размер DataPool должен быть больше 0");
        }
    }
    
    void insert(const T& value)
    {
        if(pool.size() >= capacity)
        {
            pool.pop();
            validationCache.pop();
        }

        pool.push(value);

        bool isValid = false;

        if (value.size() >= 4) {
            auto w = value[3];
            
            if constexpr (std::is_floating_point_v<typename T::value_type>) {
                isValid = std::abs(w) > std::numeric_limits<typename T::value_type>::epsilon();
            } else {
                isValid = w != typename T::value_type(0);
            }
        }
        
        validationCache.push(isValid);
    }

    T first() const
    {
        if(pool.empty()){
            LOG_ERROR("DataPool::first: pool is empty");
            throw std::runtime_error("DataPool пустой");
        }
        return pool.front();
    }

    void removeFirst()
    {
        if(pool.empty()){
            LOG_ERROR("DataPool::removeFirst: pool is empty");
            throw std::runtime_error("DataPool пустой");
        }
        pool.pop();
        validationCache.pop();
    }

    bool isEmpty() const { return pool.empty(); }
    size_t size() const { return pool.size(); }
    
    bool isLastValid() const
    {
        return !validationCache.empty() && validationCache.back();
    }
    
    bool isFirstValid() const
    {
        return !validationCache.empty() && validationCache.front();
    }
    
    T get() const
    {
        if(pool.empty()){
            LOG_ERROR("DataPool::get: pool is empty");
            throw std::runtime_error("DataPool пустой");
        }
        return pool.back();
    }

private:
    std::queue<T> pool;
    std::queue<bool> validationCache;
    size_t capacity;
};