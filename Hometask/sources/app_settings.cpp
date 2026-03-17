#include "app_settings.h"
#include <cstdlib>
#include <iostream>
#include "loger.h"

AppSettings::AppSettings(int argc, char* argv[])
{
    if(!parseArguments(argc, argv))
    {
        LOG_ERROR("Некорректные аргументы командной строки");
        std::cerr << "Usage: -a <ip> -p <port> -r <role> -i <index> -L <library>\n";
        exit(1);
    }
}

bool AppSettings::parseArguments(int argc, char* argv[])
{
    for(int i = 1; i < argc; i++)
    {
        string flag = argv[i];

        if(flag == "-a" && (i+1) < argc)
        {
            ip_address = argv[++i];
        }
        else if(flag == "-p" && (i+1) < argc)
        {
            port = atoi(argv[++i]);
        }
        else if(flag == "-r" && (i+1) < argc)
        {
            role = argv[++i];
        }
        else if(flag == "-i" && (i+1) < argc)
        {
            index = atoi(argv[++i]);
        }
        else if(flag == "-L" && (i+1) < argc)
        {
            library = argv[++i];
        }
    }

    if(ip_address.empty() || port == 0 || role.empty() || index == -1 || library.empty()) {
        LOG_ERROR("Пропущены аргументы командной строки");
        return false;
    }

    return true;    
}

string AppSettings::getIp() const { return ip_address; }
int AppSettings::getPort() const { return port; }
string AppSettings::getRole() const { return role; }
int AppSettings::getIndex() const { return index; }
string AppSettings::getLibrary() const { return library; }
string AppSettings::getUsername() const { return username; }

void AppSettings::setUsername(const string& name)
{
    username = name;
}
