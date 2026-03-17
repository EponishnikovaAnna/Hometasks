#pragma once
#include <iostream>
#include <vector>
#include <limits>

template<typename T>
bool tryReadValue(std::istream& in, T& value) {
    if (in >> value)
        return true;
    in.clear();
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return false;
}

template<typename T>
bool validateWComponent(const std::vector<T>& vec) {
    return vec[3] != 0;
}

inline void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}