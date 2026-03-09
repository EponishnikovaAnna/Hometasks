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

using namespace std;

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
                cout << "Ошибка: " << e.what() << endl;
            }
        } else {
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
            cout << "Ошибка: " << e.what() << endl;
        }
    }
};
