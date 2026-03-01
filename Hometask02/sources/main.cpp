#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits> 
#include <cstdlib>
#include "vector_utils.h"
#include "loger.h"

using namespace std;

const int MAX_UNKNOWN_COMMANDS = 5;

struct CommandLineArgs
{
    string ip_address;
    int port = 0;
    string role;
    int index = -1;
    string library;
};

string filterPrintable(const string& input);

bool parseArgument(int argc, char* argv[], CommandLineArgs& args)
{
    if (argc < 2) {
        LOG_ERROR("Отсутствуют аргументы командной строки");
        return false;
    }
    
    for(int i = 1; i < argc; i++)
    {
        string flag = argv[i];

        if(flag == "-a" && (i+1) < argc)
        {
            args.ip_address = argv[++i];
        }
        else if(flag == "-p" && (i+1) < argc)
        {
            char* endptr;
            args.port = static_cast<int>(strtol(argv[++i], &endptr, 10));
            if (*endptr != '\0' || args.port <= 0) {
                LOG_ERROR(string("Некорректное значение порта: ") + argv[i]);
                return false;
            }
        }
        else if(flag == "-r" && (i+1) < argc)
        {
            args.role = argv[++i];
        }
        else if(flag == "-i" && (i+1) < argc)
        {
            char* endptr;
            args.index = static_cast<int>(strtol(argv[++i], &endptr, 10));
            if (*endptr != '\0') {
                LOG_ERROR(string("Некорректное значение индекса: ") + argv[i]);
                return false;
            }
        }
        else if(flag == "-L" && (i+1) < argc)
        {
            args.library = argv[++i];
        }
        else
        {
            LOG_ERROR(string("Неизвестный флаг: ") + flag);
        }
    }

    if(args.ip_address.empty()) {
        LOG_ERROR("Отсутствует IP-адрес (флаг -a)");
        return false;
    }
    if(args.port == 0) {
        LOG_ERROR("Отсутствует или некорректный порт (флаг -p)");
        return false;
    }
    if(args.role.empty()) {
        LOG_ERROR("Отсутствует роль (флаг -r)");
        return false;
    }
    if(args.index == -1) {
        LOG_ERROR("Отсутствует индекс (флаг -i)");
        return false;
    }
    if(args.library.empty()) {
        LOG_ERROR("Отсутствует библиотека (флаг -L)");
        return false;
    }

    return true;    
}

string toLower(string str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;    
}

void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int main(int argc, char* argv[])
{
    CommandLineArgs args;

    if(!parseArgument(argc, argv, args))
    {
        cout << "Usage: " << argv[0] 
             << " -a <ip> -p <port> -r <role> -i <index> -L <library>\n";
        return 1;
    }
    
    cout << "Arguments:\n"
         << "ip_address: " << args.ip_address << "\n"
         << "port: " << args.port << "\n"
         << "role: " << args.role << "\n"
         << "index: " << args.index << "\n"
         << "library: " << args.library << endl;

    string programName;
    string vectorType;

    vector<int> vectorInt(VECTOR_SIZE);
    vector<float> vectorFloat(VECTOR_SIZE);
    vector<double> vectorDouble(VECTOR_SIZE);
    
    bool flag = true;
    int unknownCommandCount = 0;
    string command;
    
    while(flag) {
        cout << "\nМеню команд:\n";
        cout << "  name - ввод имени программы\n";
        cout << "  type - ввод типа вектора (int, float, double)\n";
        cout << "  vector - ввод четырёхмерного вектора\n";
        cout << "  exit - выход\n";
        cout << "Введите команду: ";
        
        command.clear();
        if (!getline(cin, command)) {
            LOG_ERROR("Ошибка чтения команды");
            break;
        }
        
        string originalCommand = command;
        command = filterPrintable(command);
        command = toLower(command);
        
        if(command == "name") {
            cout << "Введите имя программы: ";
            getline(cin, programName);
            if (programName.empty()) {
                LOG_ERROR("Введено пустое имя программы");
                cout << "Предупреждение: имя программы пустое\n";
            }
            cout << "Имя программы: \"" << programName << "\" сохранено\n";
            
        } else if(command == "type") {
            cout << "Введите тип вектора (int, float, double): ";
            getline(cin, vectorType);
            vectorType = toLower(vectorType);

            while(vectorType != "int" && vectorType != "float" && vectorType != "double") {
                LOG_ERROR(string("Некорректный тип вектора: ") + vectorType);
                cout << "Ошибка! Допустимые типы: int, float, double\n";
                cout << "Повторите ввод: ";
                getline(cin, vectorType);
                vectorType = toLower(vectorType);
            }
            cout << "Тип вектора: " << vectorType << "\n";
            
        } else if(command == "vector") {
            if(vectorType.empty()) {
                LOG_ERROR("Попытка ввода вектора без выбранного типа");
                cout << "Ошибка! Сначала введите тип вектора (команда 'type')\n";
                continue;
            }
            
            if(vectorType == "int") {
                inputVector(cin, vectorInt, "int");
                printVector(vectorInt, "int");
                if (!validateWComponent(vectorInt)) {
                    LOG_ERROR("w-компонента вектора int равна нулю");
                    cout << "Предупреждение: w-компонента равна нулю!\n";
                }
            } else if(vectorType == "float") {
                inputVector(cin, vectorFloat, "float");
                printVector(vectorFloat, "float");
                if (!validateWComponent(vectorFloat)) {
                    LOG_ERROR("w-компонента вектора float равна нулю");
                    cout << "Предупреждение: w-компонента равна нулю!\n";
                }
            } else if(vectorType == "double") {
                inputVector(cin, vectorDouble, "double");
                printVector(vectorDouble, "double");
                if (!validateWComponent(vectorDouble)) {
                    LOG_ERROR("w-компонента вектора double равна нулю");
                    cout << "Предупреждение: w-компонента равна нулю!\n";
                }
            }
            cout << "Вектор успешно сохранён!\n";
            
        } else if(command == "exit") {
            cout << "\nЗавершение программы\n";
            flag = false;
            
        } else if(command.empty()) {
            continue;
        } else {
            unknownCommandCount++;
            LOG_ERROR(string("Неизвестная команда: ") + originalCommand);
            cout << "Неизвестная команда! Используйте: name, type, vector, exit\n";
            
            if (unknownCommandCount >= MAX_UNKNOWN_COMMANDS) {
                LOG_ERROR("Превышено количество некорректных команд");
                cout << "Слишком много некорректных команд. Программа завершена.\n";
                flag = false;
            }
        }
    }

    return 0;    
}
