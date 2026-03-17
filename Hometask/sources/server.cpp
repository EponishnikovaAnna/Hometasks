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

#include "loger.h"

const short port = 1100;

template<typename T>
class Matrix{
private:
    T data[4][4];
public:
    Matrix(){
        LOG_ERROR("Matrix created for type: " + std::string(typeid(T).name()));
        for(int i = 0; i < 4; i++){
            for(int j = 0; j<4; j++){
                data[i][j] = (i==j) ? T(1): T(0);
            }
        }
    }

    void setValue(int row, int col, T value){
        if(row >= 0 && row < 4 && col >= 0 && col < 4){
            data[row][col] = value;
            LOG_ERROR("Matrix::setValue: set data[" + std::to_string(row) + "][" + 
                     std::to_string(col) + "] = " + std::to_string(value));
        } else {
            LOG_ERROR("Matrix::setValue: invalid indices [" + std::to_string(row) + "][" + 
                     std::to_string(col) + "]");
        }
    }

    std::vector<T> multiply(const std::vector<T>& vec) const{
        if(vec.size() != 4){
            LOG_ERROR("Matrix::multiply: vector size is " + std::to_string(vec.size()) + 
                     ", expected 4");
            std::cerr << "Ошибка: вектор должен быть размера 4\n";
            return vec;
        }

        std::vector<T> result(4, T(0));
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                result[i] += data[i][j] * vec[j];
            }
        }
        
        LOG_ERROR("Matrix::multiply: multiplication completed");
        return result;
    }
};

template<typename T>
bool processClient(int clientSocket)
{
    LOG_ERROR("processClient started for socket: " + std::to_string(clientSocket) + 
              ", type: " + std::string(typeid(T).name()));
    
    int vecSize;
    auto recvSize = recv(clientSocket, &vecSize, sizeof(vecSize), 0);
    
    if(recvSize <= 0) {
        LOG_ERROR(std::string("processClient: failed to receive vector size, recvSize=" + 
                 std::to_string(recvSize) + ", errno=" + std::to_string(errno) + 
                 " (" + strerror(errno) + ")"));
        std::cout << "Ошибка приема размера вектора\n";
        return false;
    }
    
    if(vecSize <= 0 || vecSize > 1000) {  // Защита от слишком больших векторов
        LOG_ERROR("processClient: invalid vector size: " + std::to_string(vecSize));
        std::cout << "Ошибка: некорректный размер вектора\n";
        return false;
    }
    
    std::cout << "Получен размер вектора: " << vecSize << std::endl;
    LOG_ERROR("processClient: received vector size = " + std::to_string(vecSize));
    
    std::vector<T> receivedVec(vecSize);
    int bytesToReceive = vecSize * sizeof(T);
    int totalReceived = 0;
    char* bufferPtr = reinterpret_cast<char*>(receivedVec.data());
    
    std::cout << "  " << "waiting for " << bytesToReceive << " bytes of data..." << std::endl;
    LOG_ERROR("processClient: expecting " + std::to_string(bytesToReceive) + " bytes");
    
    while (totalReceived < bytesToReceive) {
        auto getSize = recv(clientSocket, bufferPtr + totalReceived,
                            bytesToReceive - totalReceived, 0);
        
        if (getSize <= 0) {
            LOG_ERROR(std::string("processClient: failed to receive data, getSize=" + 
                     std::to_string(getSize) + ", errno=" + std::to_string(errno) + 
                     " (" + strerror(errno) + ")"));
            std::cout << "  " << "Ошибка приема данных" << std::endl;
            return false;
        }
        
        totalReceived += getSize;
        std::cout << "  " << "received " << totalReceived << "/" << bytesToReceive << " bytes" << std::endl;
        LOG_ERROR("processClient: received " + std::to_string(totalReceived) + 
                 "/" + std::to_string(bytesToReceive) + " bytes");
    }

    std::cout << "  " << "received vector: ";
    for (int i = 0; i < vecSize; i++) {
        std::cout << receivedVec[i];
        if (i < vecSize - 1) std::cout << ", ";
    }
    std::cout << std::endl;

    LOG_ERROR("processClient: creating matrix for multiplication");
    Matrix<T> matrix;

    for (int i = 0; i < vecSize; i++) {
        matrix.setValue(i, i, T(2));
    }

    LOG_ERROR("processClient: performing multiplication");
    std::vector<T> resultVec = matrix.multiply(receivedVec);
    
    std::cout << "  " << "processed vector: ";
    for (int i = 0; i < resultVec.size(); i++) {  
        std::cout << resultVec[i];                  
        if (i < resultVec.size() - 1) std::cout << ", ";
    }
    std::cout << std::endl;

    LOG_ERROR("processClient: sending result, size=" + std::to_string(resultVec.size()));

    int newVecSize = resultVec.size();
    auto sendSize = send(clientSocket, &newVecSize, sizeof(newVecSize), 0);
    std::cout << "  " << "sent size:\t" << sendSize << " bytes" << std::endl;
    
    if(sendSize != sizeof(newVecSize)) {
        LOG_ERROR("processClient: failed to send result size, sent=" + 
                 std::to_string(sendSize) + ", errno=" + std::to_string(errno));
        return false;
    }

    std::cout << "  " << "sending " << resultVec.size() * sizeof(T) << " bytes of data..." << std::endl;  
    auto sendData = send(clientSocket, resultVec.data(), resultVec.size() * sizeof(T), 0);  
    std::cout << "  " << "sent data:\t" << sendData << " bytes" << std::endl;
    
    if(sendData != static_cast<ssize_t>(resultVec.size() * sizeof(T))) {
        LOG_ERROR("processClient: failed to send result data, sent=" + 
                 std::to_string(sendData) + ", expected=" + 
                 std::to_string(resultVec.size() * sizeof(T)));
        return false;
    }
    
    LOG_ERROR("processClient: successfully completed for socket: " + std::to_string(clientSocket));
    return true;
}

int main()
{
    LOG_ERROR("Server application started on port " + std::to_string(port));
    
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
    
    while(true) {  
        int client = accept(sock, nullptr, nullptr);
        
        if (client < 0) {
            LOG_ERROR(std::string("Accept failed: ") + strerror(errno));
            perror("Ошибка: принятия");
            continue;
        }
        std::cout << "  " << "server accept:\tto " << client << std::endl;
        LOG_ERROR("New client connected, socket: " + std::to_string(client));
        
        if(processClient<int>(client)) {
            std::cout << "Обработка завершена успешно\n";
            LOG_ERROR("Client " + std::to_string(client) + " processed successfully");
        } else {
            std::cout << "Ошибка обработки\n";
            LOG_ERROR("Client " + std::to_string(client) + " processing failed");
        }
        
        shutdown(client, SHUT_RDWR);
        std::cout << "  " << "client connection closed" << std::endl;
        close(client);
        LOG_ERROR("Client " + std::to_string(client) + " connection closed");
    }
    
    close(sock);
    std::cout << "  " << "server close" << std::endl;
    std::cout << std::endl;
    LOG_ERROR("Server shutting down");
    
    return 0;
}