#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include "loger.h"

class NetworkAddress {
private:
    std::string ip;
    int port;
    
public:
    NetworkAddress(const std::string& address);
    NetworkAddress(const std::vector<int>& parts);
    NetworkAddress(unsigned int ipHex, unsigned short portHex);
    
    void print() const;
    std::string getIp() const;
    int getPort() const;
};