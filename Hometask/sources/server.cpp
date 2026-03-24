#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <errno.h>
#include <pthread.h>
#include <random>

#include "loger.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

const int MAX_CLIENTS = 4;

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
int active_clients = 0;

struct ClientData {
    int clientSocket;
    int clientId;
};

void printHelp() {
    std::cout << "  Сервер для умножения 4D векторов на диагональные матрицы.\n";
    std::cout << "  Принимает JSON-запросы от клиентов, обрабатывает векторы\n";
    std::cout << "  различных типов (int, float, double) и возвращает результат.\n\n";
    
    std::cout << "ИСПОЛЬЗОВАНИЕ:\n";
    std::cout << "  ./MyServer [ОПЦИИ]\n\n";
    
    std::cout << "ОПЦИИ:\n";
    std::cout << "  -p <port>          Порт для прослушивания (1024-65535)\n";
    std::cout << "  --help             Показать эту справку\n\n";
    
    std::cout << "ПАРАМЕТРЫ:\n";
    std::cout << "  <port>             Номер порта (обязательный параметр)\n";
    
    std::cout << "ПРИМЕРЫ:\n";
    std::cout << "  ./MyServer -p 1100              Запуск на порту 8080\n";
    std::cout << "  ./MyServer --help               Показать справку\n\n";
    std::cout << "  Максимум одновременных подключений: " << MAX_CLIENTS << "\n";
    std::cout << "  Поддерживаемые типы векторов: int, float, double\n";
    std::cout << "  Матрицы генерируются случайным образом\n";
}

template<typename T>
class Matrix{
private:
    T data[4][4];
public:
    Matrix(){
        for(int i = 0; i < 4; i++){
            for(int j = 0; j<4; j++){
                data[i][j] = (i==j) ? T(1): T(0);
            }
        }
    }

    void setValue(int row, int col, T value){
        if(row >= 0 && row < 4 && col >= 0 && col < 4){
            data[row][col] = value;
        }
    }

    std::vector<T> multiply(const std::vector<T>& vec) const{
        if(vec.size() != 4){
            return std::vector<T>();
        }

        std::vector<T> result(4, T(0));
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                result[i] += data[i][j] * vec[j];
            }
        }
        
        return result;
    }
};

bool processClient(int clientSocket, int clientId)
{
    try {
        char recvBuffer[2048];

        ssize_t received = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
        
        if (received <= 0) {
            LOG_ERROR("Failed to receive data from client");
            return false;
        }

        if (received < sizeof(uint32_t)) {
            LOG_ERROR("Received packet too short");
            return false;
        }

        uint32_t jsonLength;
        memcpy(&jsonLength, recvBuffer, sizeof(jsonLength));
        jsonLength = ntohl(jsonLength);
        
        std::cout << "[Client " << clientId << "] Получена длина JSON: " 
                  << jsonLength << " байт" << std::endl;

        if (received < sizeof(uint32_t) + jsonLength) {
            LOG_ERROR("Incomplete JSON data received");
            std::cout << "[Client " << clientId << "] Ожидалось " << jsonLength 
                      << " байт, получено " << (received - sizeof(uint32_t)) << std::endl;
            return false;
        }

        std::string jsonStr(recvBuffer + sizeof(uint32_t), jsonLength);
        std::cout << "[Client " << clientId << "] Получен JSON: " << jsonStr << std::endl;
        
        json request = json::parse(jsonStr);

        std::random_device rd;
        std::mt19937 gen(rd());

        if(request["vector"].is_array() && !request["vector"].empty()) {
            if(request["vector"][0].is_number_integer()) {
                std::cout << "[Client " << clientId << "] Обнаружен тип: int" << std::endl;
                std::vector<int> receivedVec = request["vector"].get<std::vector<int>>();
                
                if(receivedVec.size() != 4) {
                    json errorResponse;
                    errorResponse["error"] = "Vector size must be 4";
                    std::string errorStr = errorResponse.dump();
                    uint32_t errorLen = htonl(errorStr.length());

                    std::vector<char> responseBuffer(sizeof(errorLen) + errorStr.length());
                    memcpy(responseBuffer.data(), &errorLen, sizeof(errorLen));
                    memcpy(responseBuffer.data() + sizeof(errorLen), errorStr.c_str(), errorStr.length());
                    
                    send(clientSocket, responseBuffer.data(), responseBuffer.size(), 0);
                    return false;
                }

                Matrix<int> matrix;
                std::uniform_int_distribution<int> dist(1, 100); 
                for(int i = 0; i < 4; i++){
                    matrix.setValue(i, i, dist(gen));
                }
                std::vector<int> resultVec = matrix.multiply(receivedVec);
                
                json response;
                response["result"] = resultVec;
                response["status"] = "success";
                
                std::string responseStr = response.dump();
                uint32_t responseLength = htonl(responseStr.length());

                std::vector<char> responseBuffer(sizeof(responseLength) + responseStr.length());
                memcpy(responseBuffer.data(), &responseLength, sizeof(responseLength));
                memcpy(responseBuffer.data() + sizeof(responseLength), responseStr.c_str(), responseStr.length());
                
                send(clientSocket, responseBuffer.data(), responseBuffer.size(), 0);
                return true;
                
            } else if(request["vector"][0].is_number_float()) {
                double val = request["vector"][0].get<double>();
                float fval = static_cast<float>(val);
                
                if(static_cast<double>(fval) == val) {
                    std::cout << "[Client " << clientId << "] Обнаружен тип: float" << std::endl;
                    std::vector<float> receivedVec = request["vector"].get<std::vector<float>>();
                    
                    if(receivedVec.size() != 4) {
                        json errorResponse;
                        errorResponse["error"] = "Vector size must be 4";
                        std::string errorStr = errorResponse.dump();
                        uint32_t errorLen = htonl(errorStr.length());
                        
                        std::vector<char> responseBuffer(sizeof(errorLen) + errorStr.length());
                        memcpy(responseBuffer.data(), &errorLen, sizeof(errorLen));
                        memcpy(responseBuffer.data() + sizeof(errorLen), errorStr.c_str(), errorStr.length());
                        
                        send(clientSocket, responseBuffer.data(), responseBuffer.size(), 0);
                        return false;
                    }

                    Matrix<float> matrix;
                    std::uniform_real_distribution<float> dist_f(0.0f, 1.0f);
                    for(int i = 0; i < 4; i++){
                        matrix.setValue(i, i, dist_f(gen));
                    }
                    std::vector<float> resultVec = matrix.multiply(receivedVec);
                    
                    json response;
                    response["result"] = resultVec;
                    response["status"] = "success";
                    
                    std::string responseStr = response.dump();
                    uint32_t responseLength = htonl(responseStr.length());
                    
                    std::vector<char> responseBuffer(sizeof(responseLength) + responseStr.length());
                    memcpy(responseBuffer.data(), &responseLength, sizeof(responseLength));
                    memcpy(responseBuffer.data() + sizeof(responseLength), responseStr.c_str(), responseStr.length());
                    
                    send(clientSocket, responseBuffer.data(), responseBuffer.size(), 0);
                    return true;
                } else {
                    std::cout << "[Client " << clientId << "] Обнаружен тип: double" << std::endl;
                    std::vector<double> receivedVec = request["vector"].get<std::vector<double>>();
                    
                    if(receivedVec.size() != 4) {
                        json errorResponse;
                        errorResponse["error"] = "Vector size must be 4";
                        std::string errorStr = errorResponse.dump();
                        uint32_t errorLen = htonl(errorStr.length());
                        
                        std::vector<char> responseBuffer(sizeof(errorLen) + errorStr.length());
                        memcpy(responseBuffer.data(), &errorLen, sizeof(errorLen));
                        memcpy(responseBuffer.data() + sizeof(errorLen), errorStr.c_str(), errorStr.length());
                        
                        send(clientSocket, responseBuffer.data(), responseBuffer.size(), 0);
                        return false;
                    }

                    Matrix<double> matrix;
                    std::uniform_real_distribution<double> dist_d(0.0, 1.0);
                    for(int i = 0; i < 4; i++){
                        matrix.setValue(i, i, dist_d(gen));
                    }
                    std::vector<double> resultVec = matrix.multiply(receivedVec);
                    
                    json response;
                    response["result"] = resultVec;
                    response["status"] = "success";
                    
                    std::string responseStr = response.dump();
                    uint32_t responseLength = htonl(responseStr.length());
                    
                    std::vector<char> responseBuffer(sizeof(responseLength) + responseStr.length());
                    memcpy(responseBuffer.data(), &responseLength, sizeof(responseLength));
                    memcpy(responseBuffer.data() + sizeof(responseLength), responseStr.c_str(), responseStr.length());
                    
                    send(clientSocket, responseBuffer.data(), responseBuffer.size(), 0);
                    return true;
                }
            }
        }
        
        return false;
        
    } catch(const std::exception& e) {
        std::cerr << "[Client " << clientId << "] Error: " << e.what() << std::endl;
        return false;
    }
}

void* handleClient(void* arg) {
    ClientData* data = (ClientData*)arg;
    int clientSocket = data->clientSocket;
    int clientId = data->clientId;
    
    std::cout << "=== Поток для клиента " << clientId << " запущен ===" << std::endl;
    
    if(processClient(clientSocket, clientId)) {
        std::cout << "[Client " << clientId << "] Обработка завершена успешно\n";
    } else {
        std::cout << "[Client " << clientId << "] Ошибка обработки\n";
    }
    
    shutdown(clientSocket, SHUT_RDWR);
    close(clientSocket);
    
    pthread_mutex_lock(&clients_mutex);
    active_clients--;
    std::cout << "Активных клиентов: " << active_clients << "/" << MAX_CLIENTS << std::endl;
    pthread_mutex_unlock(&clients_mutex);
    
    delete data;
    return nullptr;
}

int main(int argc, char* argv[])
{
    short port = 0;

    for(int i = 1; i < argc; i++)
    {
        string flag = argv[i];

        if(flag == "--help")
        {
            printHelp();
            exit(0);
        }
        else if(flag == "-p" && (i+1) < argc)
        {
            port = atoi(argv[++i]);
            
            if(port <= 0 || port > 65535) {
                LOG_ERROR("Неверный порт: " + std::to_string(port));
                exit(0);
            }
        }
    }

    LOG_ERROR("Server application started on port " + std::to_string(port));
    std::cout << "Максимум одновременных подключений: " << MAX_CLIENTS << std::endl;
    
    struct sockaddr_in sockAddr;
    
    int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if (-1 == sock) {
        LOG_ERROR(std::string("Failed to create socket: ") + strerror(errno));
        perror("ошибка при создании сокета");
        exit(EXIT_FAILURE);
    }
    
    std::cout << std::endl;
    std::cout << "  " << "server socket:\t" << sock << std::endl;
    LOG_ERROR("Socket created: " + std::to_string(sock));

    int optval = 1;
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        LOG_ERROR(std::string("Failed to set socket options: ") + strerror(errno));
    }
    
    memset(&sockAddr, 0, sizeof(sockAddr));
    
    sockAddr.sin_family = PF_INET;
    sockAddr.sin_port = htons(port);
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int result = bind(sock, (struct sockaddr*)&sockAddr, sizeof(sockAddr));
    
    if (-1 == result) {
        LOG_ERROR(std::string("Bind failed: ") + strerror(errno));
        perror("Ошибка: связывания");
        close(sock);
        exit(EXIT_FAILURE);
    }
    std::cout << "  " << "server bind:\t" << result << std::endl;
    LOG_ERROR("Bind successful");
    
    result = listen(sock, 10);
    
    if (-1 == result) {
        LOG_ERROR(std::string("Listen failed: ") + strerror(errno));
        perror("Ошибка: прослушивания");
        close(sock);
        exit(EXIT_FAILURE);
    }
    std::cout << "  " << "server listen:\t" << result << std::endl;
    LOG_ERROR("Listen successful, waiting for connections...");
    std::cout << "Ожидание подключений...\n" << std::endl;
    
    int clientId = 0;
    
    while(true) {  
        pthread_mutex_lock(&clients_mutex);
        if(active_clients >= MAX_CLIENTS) {
            pthread_mutex_unlock(&clients_mutex);
            usleep(1000000);
            continue;
        }
        pthread_mutex_unlock(&clients_mutex);
        
        int client = accept(sock, nullptr, nullptr);
        
        if (client < 0) {
            LOG_ERROR(std::string("Accept failed: ") + strerror(errno));
            perror("Ошибка: принятия");
            continue;
        }
        
        pthread_mutex_lock(&clients_mutex);
        active_clients++;
        clientId++;
        int currentClientId = clientId;
        pthread_mutex_unlock(&clients_mutex);
        
        std::cout << "\n=== Новое подключение ===" << std::endl;
        std::cout << "Клиент ID: " << currentClientId << std::endl;
        std::cout << "Сокет: " << client << std::endl;
        std::cout << "Активных клиентов: " << active_clients << "/" << MAX_CLIENTS << std::endl;
        
        ClientData* data = new ClientData;
        data->clientSocket = client;
        data->clientId = currentClientId;

        pthread_t thread;
        if(pthread_create(&thread, nullptr, handleClient, data) != 0) {
            LOG_ERROR("Failed to create thread");
            std::cerr << "Ошибка создания потока" << std::endl;
            delete data;
            close(client);
            
            pthread_mutex_lock(&clients_mutex);
            active_clients--;
            pthread_mutex_unlock(&clients_mutex);
        } else {
            pthread_detach(thread);
            std::cout << "Поток для клиента " << currentClientId << " создан" << std::endl;
        }
    }
    
    close(sock);
    std::cout << "  " << "server close" << std::endl;
    std::cout << std::endl;
    LOG_ERROR("Server shutting down");
    
    return 0;
}
