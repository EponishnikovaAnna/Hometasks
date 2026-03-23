#include "app_settings.h"
#include <cstdlib>
#include <iostream>
#include "loger.h"

AppSettings::AppSettings(int argc, char* argv[])
    : server()
    , role("")
    , index(-1)
    , library("")
    , username("")
{
    if(!parseArguments(argc, argv))
    {
        LOG_ERROR("Некорректные аргументы командной строки");
        printHelp();
        exit(1);
    }
}

void AppSettings::printHelp()
{
    std::cout << "Использование: ./MyApp -i <IPv4> -p <порт> [опциональные флаги]\n\n";
    std::cout << "Обязательные аргументы (выделены в структуру ServerConfig):\n";
    std::cout << "  -i <IPv4>    IPv4 адрес сервера (например: 127.0.0.1)\n";
    std::cout << "  -p <порт>    Порт сервера (например: 8080)\n\n";
    std::cout << "Опциональные аргументы:\n";
    std::cout << "  --help       Показать эту справку\n";
    std::cout << "  -r <роль>    Роль клиента (например: client)\n";
    std::cout << "  -idx <индекс> Индекс клиента\n";
    std::cout << "  -L <библиотека> Путь к динамической библиотеке\n\n";
    std::cout << "Примеры:\n";
    std::cout << "  ./MyApp -i 192.168.1.100 -p 8080\n";
    std::cout << "  ./MyApp -i 127.0.0.1 -p 9090 -r master -idx 1 -L ./libplugin.so\n";
}

bool AppSettings::parseArguments(int argc, char* argv[])
{
    bool hasServerIp = false;
    bool hasServerPort = false;

    for(int i = 1; i < argc; i++)
    {
        string flag = argv[i];

        if(flag == "--help")
        {
            printHelp();
            exit(0);
        }
        else if(flag == "-i" && (i+1) < argc)
        {
            server.ip_address = argv[++i];  
            hasServerIp = true;

            int dots = 0;
            bool valid = true;
            for(char c : server.ip_address) {
                if(c == '.') dots++;
                else if(!isdigit(c) && c != '.') {
                    valid = false;
                    break;
                }
            }
            if(!valid || dots != 3) {
                LOG_ERROR("Неверный формат IPv4: " + server.ip_address);
                return false;
            }
        }
        else if(flag == "-p" && (i+1) < argc)
        {
            server.port = atoi(argv[++i]);
            hasServerPort = true;
            
            if(server.port <= 0 || server.port > 65535) {
                LOG_ERROR("Неверный порт: " + std::to_string(server.port));
                return false;
            }
        }
        else if(flag == "-r" && (i+1) < argc)
        {
            role = argv[++i];
        }
        else if(flag == "-idx" && (i+1) < argc)
        {
            index = atoi(argv[++i]);
        }
        else if(flag == "-L" && (i+1) < argc)
        {
            library = argv[++i];
        }
        else
        {
            LOG_ERROR("Неизвестный или некорректный флаг: " + flag);
            return false;
        }
    }

    if(!hasServerIp) {
        LOG_ERROR("Отсутствует обязательный флаг -i (IPv4 сервера)");
        return false;
    }
    
    if(!hasServerPort) {
        LOG_ERROR("Отсутствует обязательный флаг -p (порт сервера)");
        return false;
    }

    return true;    
}

const ServerConfig& AppSettings::getServerConfig() const { 
    return server; 
}

string AppSettings::getIp() const { 
    return server.ip_address; 
}

int AppSettings::getPort() const { 
    return server.port; 
}

string AppSettings::getRole() const { 
    return role; 
}

int AppSettings::getIndex() const { 
    return index; 
}

string AppSettings::getLibrary() const { 
    return library; 
}

string AppSettings::getUsername() const { 
    return username; 
}

void AppSettings::setUsername(const string& name)
{
    username = name;
}