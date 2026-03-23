#pragma once
#include <string>

using namespace std;

struct ServerConfig {
    string ip_address;
    int port;   
    
    ServerConfig() : ip_address(""), port(0) {}
    
    bool isValid() const {
        return !ip_address.empty() && port > 0 && port <= 65535;
    }
};

class AppSettings {
public:
    AppSettings(int argc, char* argv[]);

    const ServerConfig& getServerConfig() const;
    string getIp() const;
    int getPort() const;
    string getRole() const;
    int getIndex() const;
    string getLibrary() const;
    string getUsername() const;

    void setUsername(const string& name);

private:
    ServerConfig server;
    string role;
    int index;
    string library;
    string username;

    bool parseArguments(int argc, char* argv[]);
    void printHelp();
};