#pragma once
#include <queue>
#include <stdexcept>

template<typename T>
class DataPool
{
public:
    DataPool(size_t cap) : capacity(cap)
    {
        if(capacity == 0)
            throw std::invalid_argument("Размер DataPool должен быть больше 0");
    }
    
    void insert(const T& value)
    {
        if(pool.size() >= capacity)
        {
            pool.pop();
            validationCache.pop();
        }

        pool.push(value);

        bool isValid = value[3] != 0;
        validationCache.push(isValid);
    }

    T first() const
    {
        if(pool.empty())
            throw std::runtime_error("DataPool пустой");

        return pool.front();
    }

    void removeFirst()
    {
        if(pool.empty())
            throw std::runtime_error("DataPool пустой");

        pool.pop();
        validationCache.pop();
    }

    bool isEmpty() const
    {
        return pool.empty();
    }

    size_t size() const
    {
        return pool.size();
    }

    bool isLastValid() const
    {
        if(validationCache.empty())
            return false;
        return validationCache.back();
    }

    bool isFirstValid() const
    {
        if(validationCache.empty())
            return false;
        return validationCache.front();
    }


private:
    std::queue<T> pool;
    std::queue<bool> validationCache;
    size_t capacity;
};