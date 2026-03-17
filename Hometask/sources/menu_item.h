#pragma once
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <typeinfo>
#include <limits>
#include <sstream>
#include "data_pool.h"
#include "network_address.h"
#include "app_settings.h"
#include "loger.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

template<typename T>
bool tryReadValue(std::istream& in, T& value);

template<typename T>
bool validateWComponent(const std::vector<T>& vec);

class MenuItem {
public:
    string commandName;
    MenuItem(const string& name) : commandName(name) {}
    virtual ~MenuItem() = default;
    string getCommandName() const { return commandName; }
    virtual void action() = 0;
};

class NameMenuItem : public MenuItem {
public:
    NameMenuItem(string& nameRef) : MenuItem("name"), programName(nameRef) {}
    void action() override {
        cout << "Введите имя программы: ";
        getline(cin, programName);
        cout << "Имя программы: \"" << programName << "\" сохранено\n";
    }
private:
    string& programName;
};

class TypeMenuItem : public MenuItem {
public:
    TypeMenuItem(string& typeRef) : MenuItem("type"), vectorType(typeRef) {}
    
    void action() override {
        cout << "Введите тип вектора (int, float, double): ";
        getline(cin, vectorType);
        vectorType = toLower(vectorType);

        while(vectorType != "int" && vectorType != "float" && vectorType != "double") {
            LOG_ERROR("TypeMenuItem::action: unkonown type");
            cout << "Ошибка! Допустимые типы: int, float, double\n";
            cout << "Повторите ввод: ";
            getline(cin, vectorType);
            vectorType = toLower(vectorType);
        }
        cout << "Тип вектора: " << vectorType << "\n";
    }

private:
    string& vectorType;
    string toLower(string str) {
        transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }
};

class VectorMenuItem : public MenuItem {
public:
    VectorMenuItem(DataPool<vector<int>>& intPool, 
                   DataPool<vector<float>>& floatPool, 
                   DataPool<vector<double>>& doublePool,
                   const string& typeRef)
        : MenuItem("vector"), 
          intPool(intPool), 
          floatPool(floatPool), 
          doublePool(doublePool),
          vectorType(typeRef) {}
    
    void action() override {
        if(vectorType.empty()) {
            LOG_ERROR("VectorMenuItem::action: type is not selected");
            cout << "Ошибка: сначала выберите тип вектора командой 'type'!\n";
            return;
        }

        if(vectorType == "int") {
            actionInt();
        } else if(vectorType == "float") {
            actionFloat();
        } else if(vectorType == "double") {
            actionDouble();
        } else {
            LOG_ERROR("VectorMenuItem::action: unkonown type of vector");
            cout << "Ошибка: неизвестный тип вектора '" << vectorType << "'\n";
        }
    }

private:
    DataPool<vector<int>>& intPool;
    DataPool<vector<float>>& floatPool;
    DataPool<vector<double>>& doublePool;
    const string& vectorType;
    
    void actionInt() {
        vector<int> vec(4);
        cout << "Введите 4 целых числа через пробел:\n";
        string str;
        getline(cin, str);

        string s;
        stringstream ss(str);

        int count = 0;
        while(getline(ss, s, ' ') && count < 4) {  
            vec[count] = stoi(s);  
            count++;
        }

        for(int i = 0; i < 4; i++) {
            cout << vec[i] << " "; 
        }
        cout<<"\n";

        intPool.insert(vec);

        if(!intPool.isLastValid()) { 
            LOG_ERROR("VectorMenuItem::actionInt: четверый компонент нулевой");
            cout << "Ошибка: 4-й компонент не может быть нулевым!\n";
        } else {
            cout << "Вектор int успешно сохранён в пул!\n";
        }
}
    
    void actionFloat() {
        vector<float> vec(4);
        cout << "Введите 4 числа (float) через пробел:\n";
        string str;
        getline(cin, str);

        string s;
        stringstream ss(str);

        int count = 0;
        while(getline(ss, s, ' ') && count < 4){
            vec[count] = stof(s);
            count++;
        }

        for(int i = 0; i < 4; i++) {
            cout << vec[i] << " "; 
        }

        cout<<"\n";

        floatPool.insert(vec);

        if(!floatPool.isLastValid()) { 
            LOG_ERROR("VectorMenuItem::actionFLoat: четверый компонент нулевой");
            cout << "Ошибка: 4-й компонент не может быть нулевым!\n";
        } else {
            cout << "Вектор float успешно сохранён в пул!\n";
        }
    }
    
    void actionDouble() {
        vector<double> vec(4);
        cout << "Введите 4 числа (double) через пробел:\n";

        string str;
        getline(cin, str);

        string s;
        stringstream ss(str);

        int count = 0;
        while(getline(ss, s, ' ') && count < 4){
            vec[count] = stod(s);
            count++;
        }

        for(int i = 0; i < 4; i++) {
            cout << vec[i] << " "; 
        }

        cout<<"\n";

        doublePool.insert(vec);

        if(!doublePool.isLastValid()) { 
            LOG_ERROR("VectorMenuItem::actionDouble: четверый компонент нулевой");
            cout << "Ошибка: 4-й компонент не может быть нулевым!\n";
        } else {
            cout << "Вектор double успешно сохранён в пул!\n";
        }
    }
};

class ExitMenuItem : public MenuItem {
public:
    ExitMenuItem(const string& cmd, bool& flag) : MenuItem(cmd), running(flag) {}
    void action() override {
        running = false;
        cout << "Завершение программы\n";
    }
private:
    bool& running;
};

class NetworkAddressMenuItem: public MenuItem {  
public:
    NetworkAddressMenuItem() : MenuItem("network") {}  

    void action() override {
        cout << "Выберите способ ввода:\n";
        cout << "string - через строку (пример: 192.168.1.1:8080)\n";
        cout << "vector - через вектор (5 чисел: IP4 части и порт)\n";
        cout << "hex - через hex-значения (IP hex и порт hex)\n";
        cout << "Ваш выбор: ";

        string choice;
        getline(cin, choice);

        if(choice == "string"){
            createFromString();
        }
        else if(choice == "vector"){
            createFromVector();
        }
        else if(choice == "hex"){
            createFromHex();
        }
        else{
            LOG_ERROR("NetworkAddressMenuItem::action: unknown input format: " + choice);
            cout << "Неизвестный формат ввода\n";
        }
    }

private:
    void createFromString()
    {
        cout << "Введите адрес в формате IP:ПОРТ (например: 192.168.1.1:8080): ";
        string addrStr;
        getline(cin, addrStr);
        
        try {
            NetworkAddress addr(addrStr);
            cout << "Создан сетевой адрес:\n";
            addr.print();
        } catch(const exception& e) {
            LOG_ERROR(string("NetworkAddressMenuItem::createFromString: ") + e.what());
            cout << "Ошибка: " << e.what() << endl;
        }
    }

    void createFromVector()
    {
        cout << "Введите 5 целых чисел (4 части IP и порт) через пробел: ";
        string line;
        getline(cin, line);
        
        vector<int> parts;
        stringstream ss(line);
        int num;
        
        while(ss >> num) {
            parts.push_back(num);
        }
        
        if(parts.size() == 5) {
            try {
                NetworkAddress addr(parts);
                cout << "Создан сетевой адрес:\n";
                addr.print();
            } catch(const exception& e) {
                LOG_ERROR(string("NetworkAddressMenuItem::createFromVector: ") + e.what());
                cout << "Ошибка: " << e.what() << endl;
            }
        } else {
            LOG_ERROR("NetworkAddressMenuItem::createFromVector: need 5 numbers, got " + 
                     to_string(parts.size()));
            cout << "Ошибка: нужно ввести ровно 5 чисел!\n";
        }
    }

    void createFromHex() {
        cout << "Введите IP в hex (например: C0A80101 для 192.168.1.1): ";
        string ipHexStr;
        getline(cin, ipHexStr);
        
        cout << "Введите порт в hex (например: 1F90 для 8080): ";
        string portHexStr;
        getline(cin, portHexStr);
        
        try {
            unsigned int ipHex = stoul(ipHexStr, nullptr, 16);
            unsigned short portHex = stoul(portHexStr, nullptr, 16);
            
            NetworkAddress addr(ipHex, portHex);
            cout << "Создан сетевой адрес:\n";
            addr.print();
        } catch(const exception& e) {
            LOG_ERROR(string("NetworkAddressMenuItem::createFromHex: ") + e.what());
            cout << "Ошибка: " << e.what() << endl;
        }
    }
};

class MatrixMultiplyMenuItem: public MenuItem{
public:
    MatrixMultiplyMenuItem(DataPool<std::vector<int>>& intPool,
                            DataPool<std::vector<float>>& floatPool,
                            DataPool<std::vector<double>>& doublePool,
                            const string& typeRef)
        :MenuItem("matrix"),
        intPool(intPool), floatPool(floatPool), doublePool(doublePool),
        vectorType(typeRef) {}

    void action() override{
        if(vectorType == "int") {
            auto vec = intPool.get();
            if(vec.empty() || vec.size() != 4) {
                LOG_ERROR("MatrixMultiplyMenuItem::action: int vector size != 4, size=" + 
                         std::to_string(vec.size()));
                std::cout << "Ошибка: вектор должен быть размера 4\n";
                return;
            }
            sendAndReceive<int>(vec);
        }
        else if(vectorType == "float") {
            auto vec = floatPool.get();
            if(vec.empty() || vec.size() != 4) {
                LOG_ERROR("MatrixMultiplyMenuItem::action: float vector size != 4, size=" + 
                         std::to_string(vec.size()));
                std::cout << "Ошибка: вектор должен быть размера 4\n";
                return;
            }
            sendAndReceive<float>(vec);
        }
        else if(vectorType == "double") {
            auto vec = doublePool.get();
            if(vec.empty() || vec.size() != 4) {
                LOG_ERROR("MatrixMultiplyMenuItem::action: double vector size != 4, size=" + 
                         std::to_string(vec.size()));
                std::cout << "Ошибка: вектор должен быть размера 4\n";
                return;
            }
            sendAndReceive<double>(vec);
        }
        else {
            LOG_ERROR("MatrixMultiplyMenuItem::action: type not selected"); 
            std::cout << "Сначала выберите тип вектора (команда type)\n";
        }
    }
private:
    const int PORT = 1100;
    const char* SERVER_IP = "127.0.0.1";

    template<typename T>
    void sendAndReceive(const std::vector<T>& vec)
    {
        struct sockaddr_in sockAddr;
        int res;
        int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

        std::cout << "\n  client socket:\t" << sock << std::endl;

        if (sock == -1) {
            LOG_ERROR(std::string("MatrixMultiplyMenuItem::sendAndReceive: socket creation failed: ") + 
                     strerror(errno));
            std::cout << "Ошибка: невозможно создать сокет\n";
            return;
        }

        memset(&sockAddr, 0, sizeof(sockAddr));
        sockAddr.sin_family = PF_INET;
        sockAddr.sin_port = htons(PORT);

        res = inet_pton(PF_INET, SERVER_IP, &sockAddr.sin_addr);
        if (res < 0) {
            LOG_ERROR(std::string("MatrixMultiplyMenuItem::sendAndReceive: inet_pton failed (invalid address family): ") + 
                     strerror(errno));
            std::cout << "Ошибка: неверный формат IP-адреса\n";
            close(sock);
            return;
        } else if (res == 0) {
            LOG_ERROR("MatrixMultiplyMenuItem::sendAndReceive: inet_pton failed (invalid IP format): " + 
                     std::string(SERVER_IP));
            std::cout << "Ошибка: неверный формат IP-адреса\n";
            close(sock);
            return;
        }

        res = connect(sock, (struct sockaddr*)&sockAddr, sizeof(sockAddr));
        std::cout << "  client connect:\tto "
                  << inet_ntoa(sockAddr.sin_addr)
                  << ":(" << ntohs(sockAddr.sin_port) << ")\n";

        if (res == -1) {
            LOG_ERROR(std::string("MatrixMultiplyMenuItem::sendAndReceive: connect failed to ") + 
                     std::string(SERVER_IP) + ":" + std::to_string(PORT) + 
                     " - " + strerror(errno));
            std::cout << "Ошибка: не удалось подключиться к серверу. Убедитесь, что сервер запущен на порту " << PORT << "\n";
            close(sock);
            return;
        }

        int vecSize = vec.size();
        ssize_t sent = send(sock, &vecSize, sizeof(vecSize), 0);
        if (sent != sizeof(vecSize)) {
            LOG_ERROR("MatrixMultiplyMenuItem::sendAndReceive: failed to send vector size, sent=" + 
                     std::to_string(sent) + ", errno=" + std::to_string(errno));
            std::cout << "Ошибка отправки данных\n";
            close(sock);
            return;
        }

        sent = send(sock, vec.data(), vec.size() * sizeof(T), 0);
        if (sent != static_cast<ssize_t>(vec.size() * sizeof(T))) {
            LOG_ERROR("MatrixMultiplyMenuItem::sendAndReceive: failed to send vector data, sent=" + 
                     std::to_string(sent) + ", expected=" + std::to_string(vec.size() * sizeof(T)));
            std::cout << "Ошибка отправки данных\n";
            close(sock);
            return;
        }

        std::cout << "Отправлено: " << vec.size() << " элементов (" 
                  << vec.size() * sizeof(T) << " байт)\n";

        int newVecSize;
        ssize_t received = recv(sock, &newVecSize, sizeof(newVecSize), 0);
        if (received <= 0) {
            LOG_ERROR("MatrixMultiplyMenuItem::sendAndReceive: failed to receive response size, received=" + 
                     std::to_string(received) + ", errno=" + std::to_string(errno));
            std::cout << "Ошибка приема ответа от сервера\n";
            close(sock);
            return;
        }

        std::cout << "  client received new size:\t" << newVecSize << std::endl;

        std::vector<T> receivedVec(newVecSize);
        int bytesToReceive = newVecSize * sizeof(T);
        int totalReceived = 0;
        char* bufferPtr = reinterpret_cast<char*>(receivedVec.data());

        while (totalReceived < bytesToReceive) {
            int getSize = recv(sock, bufferPtr + totalReceived, bytesToReceive - totalReceived, 0);
            if (getSize <= 0) {
                LOG_ERROR("MatrixMultiplyMenuItem::sendAndReceive: failed to receive data, received=" + 
                         std::to_string(getSize) + ", total=" + std::to_string(totalReceived) + 
                         ", expected=" + std::to_string(bytesToReceive));
                std::cout << "Ошибка приема данных\n";
                close(sock);
                return;
            }
            totalReceived += getSize;
        }

        std::cout << "  received vector: ";
        for (int i = 0; i < newVecSize; i++) {
            std::cout << receivedVec[i];
            if (i < newVecSize - 1) std::cout << ", ";
        }
        std::cout << std::endl;

        if constexpr (std::is_same_v<T, int>) {
            intPool.insert(receivedVec);
            std::cout << "Результат сохранён в пул int\n";
        } else if constexpr (std::is_same_v<T, float>) {
            floatPool.insert(receivedVec);
            std::cout << "Результат сохранён в пул float\n";
        } else if constexpr (std::is_same_v<T, double>) {
            doublePool.insert(receivedVec);
            std::cout << "Результат сохранён в пул double\n";
        }

        shutdown(sock, SHUT_RDWR);
        close(sock);
        std::cout << "  client shutdown\n";
    }

    DataPool<std::vector<int>>& intPool;
    DataPool<std::vector<float>>& floatPool;
    DataPool<std::vector<double>>& doublePool;
    const string& vectorType;
};