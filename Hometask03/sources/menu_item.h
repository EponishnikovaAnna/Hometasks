#pragma once
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>
#include <typeinfo>
#include <limits>
#include "data_pool.h"

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
        cout << "Введите 4 целых числа:\n";
        for(int i = 0; i < 4; i++) {
            cout << "Компонент " << i+1 << ": ";
            while(!tryReadValue(cin, vec[i])) {
                cout << "Ошибка! Введите целое число: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }

        if(!validateWComponent(vec)) {
            cout << "Ошибка: 4-й компонент не может быть нулевым!\n";
        } else {
            intPool.insert(vec);
            cout << "Вектор int успешно сохранён в пул!\n";
        }
    }
    
    void actionFloat() {
        vector<float> vec(4);
        cout << "Введите 4 числа (float):\n";
        for(int i = 0; i < 4; i++) {
            cout << "Компонент " << i+1 << ": ";
            while(!tryReadValue(cin, vec[i])) {
                cout << "Ошибка! Введите число (например, 3.14): ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }

        if(!validateWComponent(vec)) {
            cout << "Ошибка: 4-й компонент не может быть нулевым!\n";
        } else {
            floatPool.insert(vec);
            cout << "Вектор float успешно сохранён в пул!\n";
        }
    }
    
    void actionDouble() {
        vector<double> vec(4);
        cout << "Введите 4 числа (double):\n";
        for(int i = 0; i < 4; i++) {
            cout << "Компонент " << i+1 << ": ";
            while(!tryReadValue(cin, vec[i])) {
                cout << "Ошибка! Введите число: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }

        if(!validateWComponent(vec)) {
            cout << "Ошибка: 4-й компонент не может быть нулевым!\n";
        } else {
            doublePool.insert(vec);
            cout << "Вектор double успешно сохранён в пул!\n";
        }
    }
};

class ExitMenuItem : public MenuItem {
public:
    ExitMenuItem(bool& flag) : MenuItem("exit"), running(flag) {}
    void action() override {
        running = false;
        cout << "Завершение программы\n";
    }
private:
    bool& running;
};
