#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include "loger.h"

using namespace std;

class NetworkAddress{
private:
    string ip;
    int port;
    
    void validateIpPart(int part, const string& context);
    void validatePort(int portNum);
    
public:
    NetworkAddress(const string& address);
    NetworkAddress(const vector<int>& parts);
    NetworkAddress(unsigned int ipHex, unsigned short portHex);
    
    void print() const;
    string getIp() const;
    int getPort() const;
};