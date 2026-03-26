#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <cctype>

inline std::string filterPrintable(const std::string& input)
{
    std::string result;
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
bool tryReadValue(std::istream& in, T& value) {
    std::streampos pos = in.tellg();
    
    if (in >> value) {
        return true;
    }

    in.clear();
    in.seekg(pos);

    std::string dummy;
    if (in >> dummy) {

    }
    
    return false;
}

template<typename T>
bool validateWComponent(const std::vector<T>& vec) {
    if (vec.size() < 4) {
        return false;
    }
    try {
        T w = vec.at(3);
        if constexpr (std::is_floating_point_v<T>) {
            return std::abs(w) > std::numeric_limits<T>::epsilon();
        } else {
            return w != T(0);
        }
    } catch (const std::out_of_range&) {
        return false;
    }
}