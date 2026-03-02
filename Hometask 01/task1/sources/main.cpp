#include <iostream>
#include <string>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <limits> 

using namespace std;


struct CommandLineArgs
{
    string ip_address;
    int port = 0;
    string role;
    int index = -1;
    string library;
};

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

bool parseArgument(int argc, char* argv[], CommandLineArgs& args)
{
    for(int i = 1; i < argc; i++)
    {
        string flag = argv[i];

        if(flag == "-a" && (i+1) < argc)
        {
            args.ip_address = argv[++i];
        }
        else if(flag == "-p" && (i+1) < argc)
        {
            args.port = atoi(argv[++i]);
        }
        else if(flag == "-r" && (i+1) < argc)
        {
            args.role = argv[++i];
        }
        else if(flag == "-i" && (i+1) < argc)
        {
            args.index = atoi(argv[++i]);
        }
        else if(flag == "-L" && (i+1) < argc)
        {
            args.library = argv[++i];
        }
    }

    if(args.ip_address.empty() || args.port == 0 || 
       args.role.empty() || args.index == -1 || args.library.empty()) {
        return false;
    }

    return true;    
}

int main(int argc, char* argv[])
{
    CommandLineArgs args;

    if(parseArgument(argc, argv, args))
    {
        cout << "Arguments:\n"
             << "ip_address: " << args.ip_address << "\n"
             << "port: " << args.port << "\n"
             << "role: " << args.role << "\n"
             << "index: " << args.index << "\n"
             << "library: " << args.library << endl;
    }
    else
    {
        cout << "Usage: " << argv[0] 
             << " -a <ip> -p <port> -r <role> -i <index> -L <library>\n";
        return 1;
    }
    
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
