#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "data_pools.h"
#include "app_settings.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

template<typename T>
bool tryReadValue(std::istream& in, T& value);

template<typename T>
bool validateWComponent(const std::vector<T>& vec);

class MenuItem {
public:
    string commandName;
    MenuItem(const string& name);
    virtual ~MenuItem() = default;
    string getCommandName() const;
    virtual void action() = 0;
};

class NameMenuItem : public MenuItem {
public:
    NameMenuItem(string& nameRef);
    void action() override;
private:
    string& programName;
};

class TypeMenuItem : public MenuItem {
public:
    TypeMenuItem(string& typeRef);
    void action() override;
private:
    string& vectorType;
    string toLower(string str);
};

class VectorMenuItem : public MenuItem {
public:
    VectorMenuItem(DataPools& poolsRef, const string& typeRef);
    void action() override;
private:
    DataPools& pools;
    const string& vectorType;
    
    void actionInt();
    void actionFloat();
    void actionDouble();
};

class ExitMenuItem : public MenuItem {
public:
    ExitMenuItem(const string& cmd, bool& flag);
    void action() override;
private:
    bool& running;
};

class NetworkAddressMenuItem : public MenuItem {  
public:
    NetworkAddressMenuItem();
    void action() override;
private:
    void createFromString();
    void createFromVector();
    void createFromHex();
};

class MatrixMultiplyMenuItem : public MenuItem {
public:
    MatrixMultiplyMenuItem(DataPools& poolsRef,
                           const string& typeRef,
                           const AppSettings& settings);
    void action() override;
    
private:
    template<typename T>
    void sendAndReceive(const std::vector<T>& vec);

    DataPools& pools;
    const string& vectorType;
    const AppSettings& appSettings;
};

template<typename T>
void MatrixMultiplyMenuItem::sendAndReceive(const std::vector<T>& vec)
{
    const ServerConfig& server = appSettings.getServerConfig();
    
    struct sockaddr_in sockAddr;
    int res;
    int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    std::cout << "\n  client socket:\t" << sock << std::endl;

    if (sock == -1) {
        LOG_ERROR(std::string("socket creation failed: ") + strerror(errno));
        std::cout << "Ошибка: невозможно создать сокет\n";
        return;
    }

    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_port = htons(server.port);

    std::cout << "Подключение к " << server.ip_address << ":" << server.port << std::endl;
    
    res = inet_pton(PF_INET, server.ip_address.c_str(), &sockAddr.sin_addr);
    if (res < 0) {
        LOG_ERROR(std::string("inet_pton failed: ") + strerror(errno));
        std::cout << "Ошибка: неверный формат IP-адреса\n";
        close(sock);
        return;
    } else if (res == 0) {
        LOG_ERROR("Invalid IP format: " + server.ip_address);
        std::cout << "Ошибка: неверный формат IP-адреса\n";
        close(sock);
        return;
    }

    res = connect(sock, (struct sockaddr*)&sockAddr, sizeof(sockAddr));
    std::cout << "  client connect:\tto "
              << inet_ntoa(sockAddr.sin_addr)
              << ":(" << ntohs(sockAddr.sin_port) << ")\n";

    if (res == -1) {
        LOG_ERROR(std::string("connect failed: ") + strerror(errno));
        std::cout << "Ошибка: не удалось подключиться к серверу\n";
        close(sock);
        return;
    }
    
    std::cout << "Подключение установлено!" << std::endl;

    json request;
    request["vector"] = vec;
    std::string jsonStr = request.dump();

    uint32_t jsonLength = htonl(jsonStr.length());

    std::vector<char> buffer(sizeof(jsonLength) + jsonStr.length());
    memcpy(buffer.data(), &jsonLength, sizeof(jsonLength));
    memcpy(buffer.data() + sizeof(jsonLength), jsonStr.c_str(), jsonStr.length());

    ssize_t sent = send(sock, buffer.data(), buffer.size(), 0);
    if (sent != static_cast<ssize_t>(buffer.size())) {
        LOG_ERROR("Failed to send complete packet");
        std::cout << "Ошибка: отправлено только " << sent 
                  << " из " << buffer.size() << " байт\n";
        close(sock);
        return;
    }

    std::cout << "Данные отправлены, ждем ответ..." << std::endl;

    char recvBuffer[2048];
    ssize_t received = recv(sock, recvBuffer, sizeof(recvBuffer), 0);

    if (received <= 0) {
        LOG_ERROR("Failed to receive response");
        std::cout << "Ошибка приема ответа от сервера\n";
        close(sock);
        return;
    }
    
    if (received < sizeof(uint32_t)) {
        LOG_ERROR("Response too short");
        std::cout << "Ошибка: ответ слишком короткий\n";
        close(sock);
        return;
    }

    uint32_t responseLength;
    memcpy(&responseLength, recvBuffer, sizeof(responseLength));
    responseLength = ntohl(responseLength);
    
    std::cout << "Получена длина ответа: " << responseLength << " байт" << std::endl;
    
    // Проверяем, что пришло достаточно данных
    if (received < sizeof(uint32_t) + responseLength) {
        LOG_ERROR("Incomplete response received");
        std::cout << "Ошибка: получено " << received 
                  << " байт, ожидалось " << sizeof(uint32_t) + responseLength << "\n";
        close(sock);
        return;
    }

    std::string responseStr(recvBuffer + sizeof(uint32_t), responseLength);
    
    try {
        json response = json::parse(responseStr);
        std::cout << "Получен JSON ответ: " << responseStr << std::endl;

        std::vector<T> receivedVec;
        if(response.contains("result")){
            receivedVec = response["result"].get<std::vector<T>>();
            std::cout << "  received vector: ";
            for (size_t i = 0; i < receivedVec.size(); i++) {
                std::cout << receivedVec[i];
                if (i < receivedVec.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl;

            if constexpr (std::is_same_v<T, int>) {
                pools.intPool.insert(receivedVec);
                std::cout << "Результат сохранён в пул int\n";
            } else if constexpr (std::is_same_v<T, float>) {
                pools.floatPool.insert(receivedVec);
                std::cout << "Результат сохранён в пул float\n";
            } else if constexpr (std::is_same_v<T, double>) {
                pools.doublePool.insert(receivedVec);
                std::cout << "Результат сохранён в пул double\n";
            }
        } else if(response.contains("error")){
            std::cout << "Ошибка сервера: " << response["error"] << "\n";
        }
    } catch(const json::parse_error& e) {
        std::cout << "Ошибка парсинга JSON: " << e.what() << std::endl;
        std::cout << "Полученная строка: " << responseStr << std::endl;
    }

    shutdown(sock, SHUT_RDWR);
    close(sock);
    std::cout << "  client shutdown\n";
}