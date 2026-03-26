#include "menu_item.h"
#include "network_address.h"
#include "loger.h"
#include <algorithm>
#include <sstream>
#include <cstring>

using namespace std;

MenuItem::MenuItem(const string& name) : commandName(name) {}

string MenuItem::getCommandName() const { 
    return commandName; 
}

NameMenuItem::NameMenuItem(string& nameRef) 
    : MenuItem("name"), programName(nameRef) {}

void NameMenuItem::action() {
    cout << "Введите имя программы: ";
    getline(cin, programName);
    cout << "Имя программы: \"" << programName << "\" сохранено\n";
}

TypeMenuItem::TypeMenuItem(string& typeRef) 
    : MenuItem("type"), vectorType(typeRef) {}

void TypeMenuItem::action() {
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

string TypeMenuItem::toLower(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

VectorMenuItem::VectorMenuItem(DataPools& poolsRef, const string& typeRef)
    : MenuItem("vector"), pools(poolsRef), vectorType(typeRef) {}

void VectorMenuItem::action() {
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

void VectorMenuItem::actionInt() {
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

    pools.intPool.insert(vec);

    if(!pools.intPool.isLastValid()) { 
        LOG_ERROR("VectorMenuItem::actionInt: четверый компонент нулевой");
        cout << "Ошибка: 4-й компонент не может быть нулевым!\n";
    } else {
        cout << "Вектор int успешно сохранён в пул!\n";
    }
}

void VectorMenuItem::actionFloat() {
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

    pools.floatPool.insert(vec);

    if(!pools.floatPool.isLastValid()) { 
        LOG_ERROR("VectorMenuItem::actionFLoat: четверый компонент нулевой");
        cout << "Ошибка: 4-й компонент не может быть нулевым!\n";
    } else {
        cout << "Вектор float успешно сохранён в пул!\n";
    }
}

void VectorMenuItem::actionDouble() {
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

    pools.doublePool.insert(vec);

    if(!pools.doublePool.isLastValid()) { 
        LOG_ERROR("VectorMenuItem::actionDouble: четверый компонент нулевой");
        cout << "Ошибка: 4-й компонент не может быть нулевым!\n";
    } else {
        cout << "Вектор double успешно сохранён в пул!\n";
    }
}

ExitMenuItem::ExitMenuItem(const string& cmd, bool& flag) 
    : MenuItem(cmd), running(flag) {}

void ExitMenuItem::action() {
    running = false;
    cout << "Завершение программы\n";
}

NetworkAddressMenuItem::NetworkAddressMenuItem() 
    : MenuItem("network") {}

void NetworkAddressMenuItem::action() {
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

void NetworkAddressMenuItem::createFromString() {
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

void NetworkAddressMenuItem::createFromVector() {
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

void NetworkAddressMenuItem::createFromHex() {
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

MatrixMultiplyMenuItem::MatrixMultiplyMenuItem(DataPools& poolsRef,
                                               const string& typeRef,
                                               const AppSettings& settings)
    : MenuItem("matrix"), pools(poolsRef), vectorType(typeRef), appSettings(settings) {}

void MatrixMultiplyMenuItem::action() {
    if(vectorType == "int") {
        auto vec = pools.intPool.get();
        if(vec.empty() || vec.size() != 4) {
            LOG_ERROR("MatrixMultiplyMenuItem::action: int vector size != 4, size=" + 
                     std::to_string(vec.size()));
            std::cout << "Ошибка: вектор должен быть размера 4\n";
            return;
        }
        sendAndReceive<int>(vec);
    }
    else if(vectorType == "float") {
        auto vec = pools.floatPool.get();
        if(vec.empty() || vec.size() != 4) {
            LOG_ERROR("MatrixMultiplyMenuItem::action: float vector size != 4, size=" + 
                     std::to_string(vec.size()));
            std::cout << "Ошибка: вектор должен быть размера 4\n";
            return;
        }
        sendAndReceive<float>(vec);
    }
    else if(vectorType == "double") {
        auto vec = pools.doublePool.get();
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