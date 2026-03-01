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
        }

        pool.push(value);
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
    }

    bool isEmpty() const
    {
        return pool.empty();
    }

    size_t size() const
    {
        return pool.size();
    }

private:
    std::queue<T> pool;
    size_t capacity;
};
