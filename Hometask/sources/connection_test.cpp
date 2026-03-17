#include "connection_test.h"
#include <iostream>
#include <regex>
#include <string>
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

    regex ipPattern(R"((\d{1,3}\.){3}\d{1,3})");

    if(!regex_match(ip, ipPattern))
    {
        LOG_ERROR("ConnectionTest::test: wrong format of IP");
        cout << "Неверный формат IP\n";
        return false;
    }

    if(port <= 0)
    {
        LOG_ERROR("ConnectionTest::test: wrong port");
        cout << "Неверный порт\n";
        return false;
    }

    cout << "ConnectionTest успешен\n";
    return true;
}