#include "network_address.h"
#include "ip_utils.h"

using namespace std;

NetworkAddress::NetworkAddress(const string& address)
{
    size_t colonPos = address.find(':');
    if(colonPos == string::npos) {
        LOG_ERROR("Неверный формат адреса: " + address);
        throw invalid_argument("Неверный формат адреса. Ожидается IP:ПОРТ");
    }
    
    string ipStr = address.substr(0, colonPos);
    string portStr = address.substr(colonPos + 1);
    
    if(!IPUtils::isValidIp(ipStr)) {
        throw invalid_argument("Неверный формат IP-адреса: " + ipStr);
    }
    
    if(portStr.empty()) {
        LOG_ERROR("Порт не указан");
        throw invalid_argument("Порт не может быть пустым");
    }
    
    port = stoi(portStr);
    IPUtils::validatePort(port);
    
    ip = ipStr;
}

NetworkAddress::NetworkAddress(const vector<int>& parts)
{
    if(parts.size() != 5) {
        string msg = "Вектор должен содержать 5 элементов (4 части IP и порт), получено " + 
                     to_string(parts.size());
        LOG_ERROR(msg);
        throw invalid_argument(msg);
    }

    for(int i = 0; i < 4; i++) {
        IPUtils::validateIpPart(parts[i], "IP адрес");
    }

    IPUtils::validatePort(parts[4]);

    ip = IPUtils::formatIp({parts[0], parts[1], parts[2], parts[3]});
    port = parts[4];
}

NetworkAddress::NetworkAddress(unsigned int ipHex, unsigned short portHex)
{
    int part1 = (ipHex >> 24) & 0xFF;
    int part2 = (ipHex >> 16) & 0xFF;
    int part3 = (ipHex >> 8) & 0xFF;
    int part4 = ipHex & 0xFF;
    
    IPUtils::validateIpPart(part1, "IP адрес (hex)");
    IPUtils::validateIpPart(part2, "IP адрес (hex)");
    IPUtils::validateIpPart(part3, "IP адрес (hex)");
    IPUtils::validateIpPart(part4, "IP адрес (hex)");
    IPUtils::validatePort(portHex);

    ip = IPUtils::formatIpFromHex(ipHex);
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