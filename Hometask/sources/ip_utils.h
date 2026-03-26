#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <regex>
#include "loger.h"

namespace IPUtils {
    inline std::vector<int> parseIpString(const std::string& ipStr) {
        std::stringstream ss(ipStr);
        std::string part;
        std::vector<int> parts;
        
        while(std::getline(ss, part, '.')) {
            int num = std::stoi(part);
            parts.push_back(num);
        }
        
        return parts;
    }

    inline void validateIpPart(int part, const std::string& context = "") {
        if(part < 0 || part > 255) {
            std::string msg = context + ": часть " + std::to_string(part) + 
                              " выходит за пределы допустимого диапазона (0-255)";
            LOG_ERROR(msg);
            throw std::out_of_range(msg);
        }
    }

    inline void validatePort(int portNum) {
        if(portNum < 0 || portNum > 65535) {
            std::string msg = "Порт " + std::to_string(portNum) + 
                              " выходит за пределы допустимого диапазона (0-65535)";
            LOG_ERROR(msg);
            throw std::out_of_range(msg);
        }
    }

    inline bool isValidIp(const std::string& ip) {
        std::regex ipPattern(R"(^(\d{1,3}\.){3}\d{1,3}$)");
        
        if(!std::regex_match(ip, ipPattern)) {
            return false;
        }
        
        try {
            auto parts = parseIpString(ip);
            if(parts.size() != 4) return false;
            
            for(int part : parts) {
                if(part < 0 || part > 255) return false;
            }
            return true;
        } catch(...) {
            return false;
        }
    }

    inline std::string formatIp(const std::vector<int>& parts) {
        if(parts.size() != 4) return "";
        return std::to_string(parts[0]) + "." + 
               std::to_string(parts[1]) + "." + 
               std::to_string(parts[2]) + "." + 
               std::to_string(parts[3]);
    }

    inline std::string formatIpFromHex(unsigned int ipHex) {
        int part1 = (ipHex >> 24) & 0xFF;
        int part2 = (ipHex >> 16) & 0xFF;
        int part3 = (ipHex >> 8) & 0xFF;
        int part4 = ipHex & 0xFF;
        
        return std::to_string(part1) + "." +
               std::to_string(part2) + "." +
               std::to_string(part3) + "." +
               std::to_string(part4);
    }
    
}