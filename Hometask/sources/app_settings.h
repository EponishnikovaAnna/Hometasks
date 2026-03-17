#pragma once
#include <string>

using namespace std;

class AppSettings {
public:
    AppSettings(int argc, char* argv[]);

    string getIp() const;
    int getPort() const;
    string getRole() const;
    int getIndex() const;
    string getLibrary() const;
    string getUsername() const;

    void setUsername(const string& name);

private:
    string ip_address;
    int port = 0;
    string role;
    int index = -1;
    string library;
    string username;

    bool parseArguments(int argc, char* argv[]);
};