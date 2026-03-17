#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <limits>

#include "app_settings.h"
#include "menu.cpp"
#include "menu_item.h"
#include "data_pool.h"
#include "connection_test.h"
#include "resource_test.h"
#include "loger.h"

template<typename T>
bool tryReadValue(std::istream& in, T& value) {
    try {
        if (in >> value) return true;
        in.clear();
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return false;
    } catch (const std::exception& e) {
        LOG_ERROR(std::string("tryReadValue failed: ") + e.what());
        std::cerr << "Ошибка при чтении значения\n";
        return false;
    }
}

template<typename T>
bool validateWComponent(const std::vector<T>& vec) {
    try {
        return vec[3] != 0;
    } catch (const std::out_of_range& e) {
        LOG_ERROR(std::string("validateWComponent failed: ") + e.what() + 
                  " - vector size: " + std::to_string(vec.size()));
        std::cerr << "Ошибка: вектор должен содержать минимум 4 элемента\n";
        return false;
    }
}

class ConnectionTestMenuItem : public MenuItem {
public:
    ConnectionTestMenuItem(ConnectionTest& testRef, const AppSettings& settings)
        : MenuItem("connection"), test(testRef), appSettings(settings) {}

    void action() override {
        try {
            std::vector<std::string> params;
            std::string ip, portStr;

            ip = appSettings.getIp();
            portStr = std::to_string(appSettings.getPort());

            params.push_back(ip);
            params.push_back(portStr);
            test.test(params);
        } catch (const std::exception& e) {
            LOG_ERROR(std::string("ConnectionTest failed: ") + e.what());
            std::cerr << "Ошибка: не удалось выполнить тест подключения\n";
        } catch (...) {
            LOG_ERROR("ConnectionTest failed with unknown error");
            std::cerr << "Ошибка: не удалось выполнить тест подключения\n";
        }
    }

private:
    ConnectionTest& test;
    const AppSettings& appSettings;
};

class ResourceTestMenuItem : public MenuItem {
public:
    ResourceTestMenuItem(ResourceTest& testRef)
        : MenuItem("resource"), test(testRef) {}

    void action() override {
        try {
            std::vector<std::string> files;
            std::string path;
            std::cout << "Введите пути к файлам через Enter (пустая строка для конца):\n";
            while(true) {
                std::getline(std::cin, path);
                if(path.empty()) break;
                files.push_back(path);
            }
            test.test(files);
        } catch (const std::exception& e) {
            LOG_ERROR(std::string("ResourceTest failed: ") + e.what());
            std::cerr << "Ошибка: не удалось выполнить тест ресурсов\n";
        } catch (...) {
            LOG_ERROR("ResourceTest failed with unknown error");
            std::cerr << "Ошибка: не удалось выполнить тест ресурсов\n";
        }
    }

private:
    ResourceTest& test;
};

int main(int argc, char* argv[])
{
    try
    {
        LOG_ERROR("Application started with " + std::to_string(argc) + " arguments");
        
        AppSettings settings(argc, argv);

        std::cout << "IP: " << settings.getIp() << "\n";
        std::cout << "Port: " << settings.getPort() << "\n";
        std::cout << "Role: " << settings.getRole() << "\n";
        std::cout << "Index: " << settings.getIndex() << "\n";
        std::cout << "Library: " << settings.getLibrary() << "\n";
        std::cout << "Имя пользователя: " << settings.getUsername() << "\n\n";

        DataPool<std::vector<int>> intPool(3);
        DataPool<std::vector<float>> floatPool(3);
        DataPool<std::vector<double>> doublePool(3);

        ConnectionTest connectionTest;
        ResourceTest resourceTest;

        std::string programName = "default";
        std::string vectorType;

        Menu menu;
        bool running = true;

        menu.addItem(std::make_unique<NameMenuItem>(programName));
        menu.addItem(std::make_unique<TypeMenuItem>(vectorType));
        menu.addItem(std::make_unique<VectorMenuItem>(intPool, floatPool, doublePool, vectorType));

        menu.addItem(std::make_unique<ConnectionTestMenuItem>(connectionTest, settings));
        
        menu.addItem(std::make_unique<ResourceTestMenuItem>(resourceTest));
        menu.addItem(std::make_unique<ExitMenuItem>("exit", running));
        menu.addItem(std::make_unique<ExitMenuItem>("quit", running));
        menu.addItem(std::make_unique<NetworkAddressMenuItem>());
        menu.addItem(std::make_unique<MatrixMultiplyMenuItem>(intPool, floatPool, doublePool, vectorType, settings));

        while(running)
        {
            try {
                std::cout << "\n=== Меню ===\n";
                std::cout << "  name - задать имя программы\n";
                std::cout << "  type - выбрать тип вектора (int/float/double)\n";
                std::cout << "  vector - ввести вектор (использует выбранный тип)\n";
                std::cout << "  connection - тест подключения\n";
                std::cout << "  resource - тест ресурсов\n";
                std::cout << "  network - ввести сетевой адрес\n";
                std::cout << "  quit/exit - выход\n";
                std::cout << "  matrix - умножить вектор на матрицу (требуется сервер)\n";
                
                std::cout << "Введите команду: ";
                
                std::string cmd;
                std::getline(std::cin >> std::ws, cmd);
                
                menu.execute(cmd);
                
            } catch (const std::exception& e) {
                LOG_ERROR(std::string("Main menu loop error: ") + e.what());
                std::cerr << "Ошибка: проблема при выполнении команды\n";
            } catch (...) {
                LOG_ERROR("Main menu loop unknown error");
                std::cerr << "Ошибка: проблема при выполнении команды\n";
            }
        }
        
        LOG_ERROR("Application finished normally");
        
    }
    catch(const std::exception& ex)
    {
        LOG_ERROR(std::string("Fatal error in main: ") + ex.what());
        std::cerr << "Критическая ошибка: " << ex.what() << "\n";
        return 1;
    }
    catch(...)
    {
        LOG_ERROR("Unknown fatal error in main");
        std::cerr << "Произошла неизвестная критическая ошибка\n";
        return 1;
    }

    return 0;
}