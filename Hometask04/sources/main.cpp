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

template<typename T>
bool tryReadValue(std::istream& in, T& value) {
    if (in >> value) return true;
    in.clear();
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return false;
}

template<typename T>
bool validateWComponent(const std::vector<T>& vec) {
    return vec[3] != 0;
}

class ConnectionTestMenuItem : public MenuItem {
public:
    ConnectionTestMenuItem(ConnectionTest& testRef, const AppSettings& settings)
        : MenuItem("connection"), test(testRef), appSettings(settings) {}

    void action() override {
        std::vector<std::string> params;
        std::string ip, portStr;

        ip = appSettings.getIp();
        portStr = std::to_string(appSettings.getPort());
        

        params.push_back(ip);
        params.push_back(portStr);
        test.test(params);
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
        std::vector<std::string> files;
        std::string path;
        std::cout << "Введите пути к файлам через Enter (пустая строка для конца):\n";
        while(true) {
            std::getline(std::cin, path);
            if(path.empty()) break;
            files.push_back(path);
        }
        test.test(files);
    }

private:
    ResourceTest& test;
};

int main(int argc, char* argv[])
{
    try
    {
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
        menu.addItem(std::make_unique<ExitMenuItem>(running));
        menu.addItem(std::make_unique<NetworkAddressMenuItem>());

        while(running)
        {
            std::cout << "  name - задать имя программы\n";
            std::cout << "  type - выбрать тип вектора (int/float/double)\n";
            std::cout << "  vector - ввести вектор (использует выбранный тип)\n";
            std::cout << "  connection - тест подключения\n";
            std::cout << "  resource - тест ресурсов\n";
            std::cout << "  network - ввести сетевой адрес\n";
            std::cout << "  exit - выход\n";
            
            std::cout << "Введите команду: ";
            
            std::string cmd;
            std::getline(std::cin, cmd);
            menu.execute(cmd);
        }
    }
    catch(const std::exception& ex)
    {
        std::cerr << "Ошибка: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
