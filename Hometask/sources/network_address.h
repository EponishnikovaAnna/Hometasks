#pragma once
#include <iostream>
#include <vector>
#include <string>

using namespace std;

class NetworkAddress{
private:
	string ip;
	int port;
public:
	NetworkAddress(const string& address)
	{
		size_t colonPos = address.find(':');
        if(colonPos != string::npos) {
            ip = address.substr(0, colonPos);
            port = stoi(address.substr(colonPos + 1));
        }
	}

	NetworkAddress(const vector<int>& parts){
		if(parts.size() >= 5) {
            ip = to_string(parts[0]) + "." + 
                 to_string(parts[1]) + "." + 
                 to_string(parts[2]) + "." + 
                 to_string(parts[3]);
            port = parts[4];
        }
	}

	NetworkAddress(unsigned int ipHex, unsigned short portHex) {
        ip = to_string((ipHex >> 24) & 0xFF) + "." +
             to_string((ipHex >> 16) & 0xFF) + "." +
             to_string((ipHex >> 8) & 0xFF) + "." +
             to_string(ipHex & 0xFF);
        port = portHex;
    }


    void print(){
    	cout<<"IP-адрес: " << ip << " Порт: " << port << "\n";

    }

};