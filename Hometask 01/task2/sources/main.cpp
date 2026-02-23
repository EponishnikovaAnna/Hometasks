#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits> 

using namespace std;

string toLower(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;    
}

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

template<typename T>
void inputVector(vector<T>& vec, const string& typeName) {
    cout << "Введите 4 числа типа " << typeName << ":\n";
    for(int i = 0; i < 4; i++) {
        while (true) {
            cout << i+1 << "-е число: ";
            if (cin >> vec[i]) {
                break;
            } else {
                clearInputBuffer();
            }
        }
    }
    clearInputBuffer();
}

template<typename T>
void printVector(const vector<T>& vec, const string& typeName) {
    cout << "Вектор (" << typeName << "): (";
    for(int i = 0; i < 4; i++) {
        cout << vec[i];
        if(i < 3) cout << ", ";
    }
    cout << ")\n";
}

int main() {
    string programName;
    string vectorType = "";
    vector<int> vectorInt(4);
    vector<float> vectorFloat(4);
    vector<double> vectorDouble(4);
    
    bool hasIntVector = false;
    bool hasFloatVector = false;
    bool hasDoubleVector = false;
    
    bool flag = true;
    
    while(flag) {
        cout << "\nМеню команд:\n";
        cout << "  name - ввод имени программы\n";
        cout << "  type - ввод типа вектора (int, float, double)\n";
        cout << "  vector - ввод четырёхмерного вектора\n";
        cout << "  exit - выход\n";
        cout << "Введите команду: ";
        
        string command;
        getline(cin, command);
        command = toLower(command);
        
        if(command == "name") {
            cout << "Введите имя программы: ";
            getline(cin, programName);
            cout << "Имя программы: \"" << programName << "\" сохранено\n";
            
        } else if(command == "type") {
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
            
        } else if(command == "vector") {
            if(vectorType.empty()) {
                cout << "Ошибка! Сначала введите тип вектора (команда 'type')\n";
                continue;
            }
            
            if(vectorType == "int") {
                inputVector(vectorInt, "int");
                hasIntVector = true;
                printVector(vectorInt, "int"); 
            } else if(vectorType == "float") {
                inputVector(vectorFloat, "float");
                hasFloatVector = true;
                printVector(vectorFloat, "float");
            } else if(vectorType == "double") {
                inputVector(vectorDouble, "double");
                hasDoubleVector = true;
                printVector(vectorDouble, "double");
            }
            cout << "Вектор успешно сохранён!\n";
            
        } else if(command == "exit") {
            cout << "\nЗавершение программы\n";
            flag = false;
            
        } else {
            cout << "Неизвестная команда! Используйте: name, type, vector, exit\n";
        }
    }
    
    return 0;
}
