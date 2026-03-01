#ifndef VECTOR_UTILS_H
#define VECTOR_UTILS_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>

const int VECTOR_SIZE = 4;

inline std::string filterPrintable(const std::string& input)
{
    std::string result;
    bool hasNonPrintable = false;
    
    for(char c: input)
    {
        if(isprint(static_cast<unsigned char>(c)))
        {
            result += c;
        }
    }
    return result;
}

template<typename T>
bool tryReadValue(std::istream& in, T& value)
{
    if(in >> value)
        return true;

    in.clear();
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return false;
}

template<typename T>
bool validateWComponent(const std::vector<T>& vec)
{
    if (vec.size() < VECTOR_SIZE) return false;
    return vec[VECTOR_SIZE - 1] != 0;
}

template<typename T>
void inputVector(std::istream& in, std::vector<T>& vec, const std::string& typeName) {
    std::cout << "Введите " << VECTOR_SIZE << " числа типа " << typeName << ":\n";
    for(int i = 0; i < VECTOR_SIZE; i++) {
        while (!tryReadValue(in, vec[i])){
            std::cout << "Повторите ввод " << i+1 << "-го числа: ";
        }
    }
}

template<typename T>
void printVector(const std::vector<T>& vec, const std::string& typeName) {
    std::cout << "Вектор (" << typeName << "): (";
    for(size_t i = 0; i < vec.size(); i++) {
        std::cout << vec[i];
        if(i < vec.size() - 1) std::cout << ", ";
    }
    std::cout << ")\n";
}

#endif
