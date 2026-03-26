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
    int foundCount = 0;
    int notFoundCount = 0;

    cout << "\n=== Результаты проверки ресурсов ===\n";
    
    for(const auto& path: params){
        if(fs::exists(path)) {
            cout << "  Найден: " << path << "\n";
            foundCount++;
        } else {
            LOG_ERROR("ResourceTest::test: файл не найден: " + path);
            cout << "  Не найден: " << path << "\n";
            allExist = false;
            notFoundCount++;
        }
    }
    
    cout << "Всего файлов: " << params.size() << "\n";
    cout << "Найдено: " << foundCount << "\n";
    cout << "Не найдено: " << notFoundCount << "\n";
    
    if(allExist) {
        cout << "Все ресурсы существуют\n";
    } else {
        cout << "Некоторые ресурсы отсутствуют\n";
    }
    
    return allExist;
}