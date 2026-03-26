#include "connection_test.h"
#include "ip_utils.h"
#include <iostream>

using namespace std;

bool ConnectionTest::test(const vector<string>& params){
    if(params.size() < 2){
        cout << "Недостаточно параметров для ConnectionTest\n";
        LOG_ERROR("ConnectionTest::test: недостаточно параметров");
        return false;
    }

    string ip = params[0];
    int port = stoi(params[1]);

    try {
        IPUtils::validatePort(port);
    } catch(const exception& e) {
        cout << "Неверный порт: " << e.what() << endl;
        return false;
    }

    if(!IPUtils::isValidIp(ip)) {
        cout << "Неверный формат IP-адреса: " << ip << endl;
        return false;
    }

    cout << "ConnectionTest успешен\n";
    return true;
}