#include "network_address.h"

using namespace std;

void NetworkAddress::validateIpPart(int part, const string& context) {
    if(part < 0 || part > 255) {
        LOG_ERROR(context + ": часть " + to_string(part) + 
                  " выходит за пределы допустимого диапазона (0-255)");
        throw out_of_range(context + ": IP часть должна быть в диапазоне 0-255");
    }
}

void NetworkAddress::validatePort(int portNum) {
    if(portNum < 0 || portNum > 65535) {
        LOG_ERROR("Порт " + to_string(portNum) + 
                  " выходит за пределы допустимого диапазона (0-65535)");
        throw out_of_range("Порт должен быть в диапазоне 0-65535");
    }
}

NetworkAddress::NetworkAddress(const string& address)
{
    size_t colonPos = address.find(':');
    if(colonPos == string::npos) {
        LOG_ERROR("Неверный формат адреса: " + address);
        throw invalid_argument("Неверный формат адреса. Ожидается IP:ПОРТ");
    }
    
    string ipStr = address.substr(0, colonPos);
    string portStr = address.substr(colonPos + 1);
    
    size_t dotPos = 0;
    size_t prevPos = 0;
    int partIndex = 0;
    
    while((dotPos = ipStr.find('.', prevPos)) != string::npos) {
        if(partIndex >= 4) {
            LOG_ERROR("IP-адрес содержит больше 4 частей");
            throw invalid_argument("IP-адрес должен содержать 4 части");
        }
        
        string partStr = ipStr.substr(prevPos, dotPos - prevPos);
        if(partStr.empty()) {
            LOG_ERROR("Пустая часть IP-адреса");
            throw invalid_argument("IP-адрес не может содержать пустые части");
        }
        
        int part = stoi(partStr);
        validateIpPart(part, "IP адрес");
        partIndex++;
        prevPos = dotPos + 1;
    }
    
    if(partIndex != 3) {
        LOG_ERROR("IP-адрес содержит " + to_string(partIndex + 1) + 
                  " частей, ожидалось 4");
        throw invalid_argument("IP-адрес должен содержать 4 части");
    }
    
    string lastPartStr = ipStr.substr(prevPos);
    if(lastPartStr.empty()) {
        LOG_ERROR("Последняя часть IP-адреса пуста");
        throw invalid_argument("IP-адрес не может содержать пустые части");
    }
    
    int lastPart = stoi(lastPartStr);
    validateIpPart(lastPart, "IP адрес");

    if(portStr.empty()) {
        LOG_ERROR("Порт не указан");
        throw invalid_argument("Порт не может быть пустым");
    }
    
    port = stoi(portStr);
    validatePort(port);
    
    ip = ipStr;
}

NetworkAddress::NetworkAddress(const vector<int>& parts)
{
    if(parts.size() < 5) {
        LOG_ERROR("Вектор содержит " + to_string(parts.size()) + 
                  " элементов, ожидалось 5");
        throw invalid_argument("Вектор должен содержать 5 элементов (4 части IP и порт)");
    }
    if(parts.size() > 5) {
        LOG_ERROR("Вектор содержит " + to_string(parts.size()) + 
                  " элементов, ожидалось 5");
        throw invalid_argument("Вектор содержит больше 5 элементов");
    }

    for(int i = 0; i < 4; i++) {
        validateIpPart(parts[i], "IP адрес");
    }

    validatePort(parts[4]);

    ip = to_string(parts[0]) + "." + 
         to_string(parts[1]) + "." + 
         to_string(parts[2]) + "." + 
         to_string(parts[3]);
    port = parts[4];
}

NetworkAddress::NetworkAddress(unsigned int ipHex, unsigned short portHex)
{
    int part1 = (ipHex >> 24) & 0xFF;
    int part2 = (ipHex >> 16) & 0xFF;
    int part3 = (ipHex >> 8) & 0xFF;
    int part4 = ipHex & 0xFF;
    
    validateIpPart(part1, "IP адрес (hex)");
    validateIpPart(part2, "IP адрес (hex)");
    validateIpPart(part3, "IP адрес (hex)");
    validateIpPart(part4, "IP адрес (hex)");

    validatePort(portHex);

    ip = to_string(part1) + "." +
         to_string(part2) + "." +
         to_string(part3) + "." +
         to_string(part4);
    port = portHex;
}

void NetworkAddress::print() const
{
    cout << "IP-адрес: " << ip << " Порт: " << port << "\n";
}

string NetworkAddress::getIp() const
{
    return ip;
}

int NetworkAddress::getPort() const
{
    return port;
}