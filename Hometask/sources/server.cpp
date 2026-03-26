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
#include <random>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>

#include "loger.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

constexpr size_t MAX_CLIENTS = 4;
constexpr size_t RECV_BUFFER_SIZE = 2048;

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
    std::cout << "  ./MyServer -p 1100              Запуск на порту 1100\n";
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

class ClientHandler {
private:
    int clientSocket;
    int clientId;
    std::mutex& clientsMutex;
    size_t& activeClients;
    std::condition_variable& cv;
    
public:
    ClientHandler(int sock, int id, std::mutex& mutex, size_t& active, std::condition_variable& cond) 
        : clientSocket(sock), clientId(id), clientsMutex(mutex), activeClients(active), cv(cond) {}
    
    ClientHandler(const ClientHandler&) = delete;
    ClientHandler& operator=(const ClientHandler&) = delete;
    
    ClientHandler(ClientHandler&& other) noexcept 
        : clientSocket(other.clientSocket), clientId(other.clientId),
          clientsMutex(other.clientsMutex), activeClients(other.activeClients), cv(other.cv) {
        other.clientSocket = -1;
    }
    
    ~ClientHandler() {
        if (clientSocket != -1) {
            shutdown(clientSocket, SHUT_RDWR);
            close(clientSocket);
            
            {
                std::lock_guard<std::mutex> lock(clientsMutex);
                activeClients--;
                std::cout << "Активных клиентов: " << activeClients << "/" << MAX_CLIENTS << std::endl;
            }
            cv.notify_one(); 
        }
    }
    
    void process() {
        try {
            char recvBuffer[RECV_BUFFER_SIZE];
            ssize_t received = recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
            
            if (received <= 0) {
                LOG_ERROR("Failed to receive data from client");
                return;
            }

            if (received < sizeof(uint32_t)) {
                LOG_ERROR("Received packet too short");
                return;
            }

            uint32_t jsonLength;
            memcpy(&jsonLength, recvBuffer, sizeof(jsonLength));
            jsonLength = ntohl(jsonLength);
            
            std::cout << "[Client " << clientId << "] Получена длина JSON: " 
                      << jsonLength << " байт" << std::endl;

            if (received < sizeof(uint32_t) + jsonLength) {
                LOG_ERROR("Incomplete JSON data received");
                return;
            }

            std::string jsonStr(recvBuffer + sizeof(uint32_t), jsonLength);
            std::cout << "[Client " << clientId << "] Получен JSON: " << jsonStr << std::endl;
            
            json request = json::parse(jsonStr);

            if (!request.contains("vector") || !request["vector"].is_array() || request["vector"].empty()) {
                sendError("Invalid request: vector field is required");
                return;
            }
            
            if (request["vector"][0].is_number_integer()) {
                processRequest<int>(request);
            } else if (request["vector"][0].is_number_float()) {
                double val = request["vector"][0].get<double>();
                float fval = static_cast<float>(val);
                
                if (static_cast<double>(fval) == val) {
                    processRequest<float>(request);
                } else {
                    processRequest<double>(request);
                }
            } else {
                sendError("Unsupported vector type");
            }
            
        } catch(const std::exception& e) {
            std::cerr << "[Client " << clientId << "] Error: " << e.what() << std::endl;
            sendError(std::string("Internal error: ") + e.what());
        }
    }
    
private:
    template<typename T>
    void processRequest(const json& request) {
        try {
            std::cout << "[Client " << clientId << "] Обнаружен тип: " << getTypeName<T>() << std::endl;
            
            std::vector<T> receivedVec = request["vector"].get<std::vector<T>>();
            
            if (receivedVec.size() != 4) {
                sendError("Vector size must be 4");
                return;
            }
            
            if (!validateWComponent(receivedVec)) {
                sendError("Invalid vector: w-component cannot be zero");
                return;
            }

            Matrix<T> matrix;
            fillMatrixWithRandomValues(matrix);
            
            std::vector<T> resultVec = matrix.multiply(receivedVec);
            sendSuccess(resultVec);
            
        } catch (const std::exception& e) {
            std::cerr << "[Client " << clientId << "] Process error: " << e.what() << std::endl;
            sendError(std::string("Processing error: ") + e.what());
        }
    }

    template<typename T>
    std::string getTypeName() const {
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, float>) return "float";
        else if constexpr (std::is_same_v<T, double>) return "double";
        else return "unknown";
    }

    template<typename T>
    void fillMatrixWithRandomValues(Matrix<T>& matrix) {
        std::random_device rd;
        std::mt19937 gen(rd());
        
        if constexpr (std::is_same_v<T, int>) {
            std::uniform_int_distribution<int> dist(1, 100);
            for (int i = 0; i < 4; i++) {
                matrix.setValue(i, i, dist(gen));
            }
        } else if constexpr (std::is_same_v<T, float>) {
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);
            for (int i = 0; i < 4; i++) {
                matrix.setValue(i, i, dist(gen));
            }
        } else if constexpr (std::is_same_v<T, double>) {
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            for (int i = 0; i < 4; i++) {
                matrix.setValue(i, i, dist(gen));
            }
        }
    }

    template<typename T>
    bool validateWComponent(const std::vector<T>& vec) {
        if (vec.size() < 4) return false;
        
        T w = vec[3];
        if constexpr (std::is_floating_point_v<T>) {
            return std::abs(w) > std::numeric_limits<T>::epsilon();
        } else {
            return w != T(0);
        }
    }

    template<typename T>
    void sendSuccess(const std::vector<T>& resultVec) {
        json response;
        response["result"] = resultVec;
        response["status"] = "success";
        
        sendJsonResponse(response);
    }

    void sendError(const std::string& errorMsg) {
        json errorResponse;
        errorResponse["error"] = errorMsg;
        errorResponse["status"] = "error";
        
        sendJsonResponse(errorResponse);
    }

    void sendJsonResponse(const json& response) {
        std::string responseStr = response.dump();
        uint32_t responseLength = htonl(responseStr.length());

        std::vector<char> responseBuffer(sizeof(responseLength) + responseStr.length());
        memcpy(responseBuffer.data(), &responseLength, sizeof(responseLength));
        memcpy(responseBuffer.data() + sizeof(responseLength), responseStr.c_str(), responseStr.length());
        
        send(clientSocket, responseBuffer.data(), responseBuffer.size(), 0);
    }
};

class ServerSocket {
private:
    int sock;
    
public:
    ServerSocket(short port) : sock(-1) {
        sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        
        if (sock == -1) {
            LOG_ERROR(std::string("Failed to create socket: ") + strerror(errno));
            throw std::runtime_error("Failed to create socket");
        }
        
        std::cout << std::endl;
        std::cout << "  " << "server socket:\t" << sock << std::endl;
        LOG_ERROR("Socket created: " + std::to_string(sock));

        int optval = 1;
        if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
            LOG_ERROR(std::string("Failed to set socket options: ") + strerror(errno));
        }
        
        struct sockaddr_in sockAddr;
        memset(&sockAddr, 0, sizeof(sockAddr));
        sockAddr.sin_family = PF_INET;
        sockAddr.sin_port = htons(port);
        sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        
        int result = bind(sock, (struct sockaddr*)&sockAddr, sizeof(sockAddr));
        
        if (result == -1) {
            LOG_ERROR(std::string("Bind failed: ") + strerror(errno));
            throw std::runtime_error("Bind failed");
        }
        std::cout << "  " << "server bind:\t" << result << std::endl;
        LOG_ERROR("Bind successful");
        
        result = listen(sock, 10);
        
        if (result == -1) {
            LOG_ERROR(std::string("Listen failed: ") + strerror(errno));
            throw std::runtime_error("Listen failed");
        }
        std::cout << "  " << "server listen:\t" << result << std::endl;
        LOG_ERROR("Listen successful, waiting for connections...");
    }
    
    ~ServerSocket() {
        if (sock != -1) {
            close(sock);
            std::cout << "  " << "server close" << std::endl;
        }
    }

    ServerSocket(const ServerSocket&) = delete;
    ServerSocket& operator=(const ServerSocket&) = delete;

    ServerSocket(ServerSocket&& other) noexcept : sock(other.sock) {
        other.sock = -1;
    }
    
    int accept() {
        int client = ::accept(sock, nullptr, nullptr);
        if (client < 0) {
            LOG_ERROR(std::string("Accept failed: ") + strerror(errno));
        }
        return client;
    }
    
    int getSocket() const { return sock; }
};

class Server {
private:
    short port;
    std::unique_ptr<ServerSocket> serverSocket;
    std::mutex clientsMutex;
    std::condition_variable cv;
    size_t activeClients = 0;
    int nextClientId = 0;
    bool stopFlag = false;
    
public:
    explicit Server(short p) : port(p) {}
    
    void stop() {
        {
            std::lock_guard<std::mutex> lock(clientsMutex);
            stopFlag = true;
        }
        cv.notify_all();
    }
    
    void run() {
        serverSocket = std::make_unique<ServerSocket>(port);
        std::cout << "Ожидание подключений...\n" << std::endl;
        
        while(!stopFlag) {
            int client = -1;

            {
                std::unique_lock<std::mutex> lock(clientsMutex);
                cv.wait(lock, [this]() {
                    return stopFlag || activeClients < MAX_CLIENTS;
                });
                
                if (stopFlag) {
                    break;  
                }
            }

            client = serverSocket->accept();
            if (client < 0) {
                continue;
            }

            int currentClientId;
            {
                std::lock_guard<std::mutex> lock(clientsMutex);

                if (activeClients >= MAX_CLIENTS) {
                    std::cout << "Лимит подключений достигнут, отклоняем" << std::endl;
                    close(client);
                    continue;
                }
                
                activeClients++;
                nextClientId++;
                currentClientId = nextClientId;
            }
            
            std::cout << "\n=== Новое подключение ===" << std::endl;
            std::cout << "Клиент ID: " << currentClientId << std::endl;
            std::cout << "Сокет: " << client << std::endl;
            
            {
                std::lock_guard<std::mutex> lock(clientsMutex);
                std::cout << "Активных клиентов: " << activeClients << "/" << MAX_CLIENTS << std::endl;
            }
            
            // Запускаем обработчик
            auto handler = std::make_unique<ClientHandler>(
                client, currentClientId, clientsMutex, activeClients, cv
            );
            
            std::thread clientThread([handler = std::move(handler)]() mutable {
                handler->process();
            });
            clientThread.detach();
            
            std::cout << "Поток для клиента " << currentClientId << " создан" << std::endl;
        }
        
        std::cout << "Сервер остановлен" << std::endl;
    }
};

int main(int argc, char* argv[])
{
    short port = 0;

    for(int i = 1; i < argc; i++)
    {
        std::string flag = argv[i];

        if(flag == "--help")
        {
            printHelp();
            return 0;
        }
        else if(flag == "-p" && (i+1) < argc)
        {
            port = atoi(argv[++i]);
            
            if(port <= 0 || port > 65535) {
                LOG_ERROR("Неверный порт: " + std::to_string(port));
                return 1;
            }
        }
    }
    
    if (port == 0) {
        std::cerr << "Ошибка: не указан порт!\n";
        printHelp();
        return 1;
    }
    
    LOG_ERROR("Server application started on port " + std::to_string(port));
    std::cout << "Максимум одновременных подключений: " << MAX_CLIENTS << std::endl;
    
    try {
        Server server(port);
        server.run();
    } catch(const std::exception& e) {
        LOG_ERROR(std::string("Server error: ") + e.what());
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    LOG_ERROR("Server shutting down");
    return 0;
}