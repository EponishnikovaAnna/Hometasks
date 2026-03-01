#include "connection_test.h"
#include <iostream>
#include <regex>
#include <string>

using namespace std;

bool ConnectionTest::test(const vector<string>& params){
    if(params.size() < 2){
        cout << "Недостаточно параметров для ConnectionTest\n";
        return false;
    }

    string ip = params[0];
    int port = stoi(params[1]);

    regex ipPattern(R"((\d{1,3}\.){3}\d{1,3})");

    if(!regex_match(ip, ipPattern))
    {
        cout << "Неверный формат IP\n";
        return false;
    }

    if(port <= 0)
    {
        cout << "Неверный порт\n";
        return false;
    }

    cout << "ConnectionTest успешен\n";
    return true;
}