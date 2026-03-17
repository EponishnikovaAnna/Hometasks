#include "resource_test.h"
#include <iostream>
#include <filesystem>
#include "loger.h"

namespace fs = std::filesystem;
using namespace std;

bool ResourceTest::test(const vector<string>& params){
    if(params.empty()){
        LOG_ERROR("ResourceTest::нет файлов для проверки");
        cout << "Нет файлов для проверки\n";
        return false;
    }

    bool allExist = true;

    for(const auto& path: params){
        if(!fs::exists(path)){
            LOG_ERROR("ConnectionTest::test: файл не найден");
            cout << "Файл не найден: " << path << "\n";
            allExist = false;
        }
    }

    if(allExist)
        cout << "Все ресурсы существуют\n";

    return allExist;
}
