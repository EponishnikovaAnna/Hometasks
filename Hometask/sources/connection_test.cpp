#include "connection_test.h"
#include <iostream>
#include <regex>
#include <string>
#include <sstream>
#include "loger.h"

using namespace std;

bool ConnectionTest::test(const vector<string>& params){
    if(params.size() < 2){
        cout << "Недостаточно параметров для ConnectionTest\n";
        LOG_ERROR("ConnectionTest::test: недостаточно параметров");
        return false;
    }

    string ip = params[0];
    int port = stoi(params[1]);

    if(port <= 0 || port > 65535) {
        LOG_ERROR("ConnectionTest::test: wrong port");
        cout << "Неверный порт (должен быть от 1 до 65535)\n";
        return false;
    }

    regex ipPattern(R"(^(\d{1,3}\.){3}\d{1,3}$)");
    
    if(!regex_match(ip, ipPattern)) {
        LOG_ERROR("ConnectionTest::test: wrong format of IP");
        cout << "Неверный формат IP\n";
        return false;
    }

    stringstream ss(ip);
    string part;
    vector<int> parts;
    
    while(getline(ss, part, '.')) {
        int num = stoi(part);
        if(num < 0 || num > 255) {
            LOG_ERROR("ConnectionTest::test: IP part out of range: " + part);
            cout << "Каждая часть IP должна быть в диапазоне 0-255\n";
            return false;
        }
        parts.push_back(num);
    }

    if(parts.size() != 4) {
        LOG_ERROR("ConnectionTest::test: invalid number of IP parts");
        cout << "IP должен содержать 4 части\n";
        return false;
    }

    cout << "ConnectionTest успешен\n";
    return true;
}