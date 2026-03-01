#include "resource_test.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

bool ResourceTest::test(const vector<string>& params){
    if(params.empty()){
        cout << "Нет файлов для проверки\n";
        return false;
    }

    bool allExist = true;

    for(const auto& path: params){
        if(!fs::exists(path)){
            cout << "Файл не найден: " << path << "\n";
            allExist = false;
        }
    }

    if(allExist)
        cout << "Все ресурсы существуют\n";

    return allExist;
}