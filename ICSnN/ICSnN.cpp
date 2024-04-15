#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SERVER_PORT 7500
#define SERVER_ADDR "127.0.0.1"
#define BUFFER_SIZE 1024

void SendFile(SOCKET s, const char* filename) {
    FILE* file;
    char buffer[BUFFER_SIZE];
    size_t bytesRead;
    errno_t err;
    // Откроем файл для чтения в бинарном режиме
    err = fopen_s(&file, filename, "rb");
    if (err != 0 || !file) {
        perror("Failed to open file");
        return;
    }
    // Отправляем содержимое файла
    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(s, buffer, bytesRead, 0);
    }

    fclose(file);
}

void ReceiveFile(SOCKET s, const char* filename) {
    FILE* file;
    char buffer[BUFFER_SIZE];
    int bytesReceived;
    errno_t err;

    // Откроем файл для записи в бинарном режиме с использованием fopen_s
    err = fopen_s(&file, filename, "wb");
    if (err != 0 || !file) {
        perror("Failed to open file");
        return;
    }

    // Читаем данные, отправленные сервером, и записываем их в файл
    while ((bytesReceived = recv(s, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytesReceived, file);
    }

    fclose(file);
}

int main(int argc, char* argv[]) {
    WSADATA ws;
    SOCKET s;
    struct sockaddr_in peer;
    char command[1024];

    // Инициализируем Winsock
    WSAStartup(MAKEWORD(2, 2), &ws);

    // Создаём сокет
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) {
        perror("Socket creation failed");
        WSACleanup();
        return 1;
    }

    // Устанавливаем соединение с сервером
    /*peer.sin_family = AF_INET;
    peer.sin_port = htons(SERVER_PORT);
    peer.sin_addr.s_addr = inet_addr(SERVER_ADDR);*/
    if (inet_pton(AF_INET, SERVER_ADDR, &peer.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        closesocket(s);
        WSACleanup();
        return 1;
    }
    char* context = NULL;
    // Консольный интерфейс
    while (1) {
        printf("Commands:\n");
        printf("send <filename> - to send a file to the server\n");
        printf("get <filename> - to get a file from the server\n");
        printf("exit - to exit the program\n");

        memset(command, 0, sizeof(command));
        fgets(command, 100, stdin); // Используйте fgets в реальных программах для защиты от переполнения буфера

        char* cmd = strtok_s(command, " ", &context);
        char* filename = strtok_s(NULL, " ", &context);

        if (strcmp(cmd, "send") == 0 && filename != NULL) {
            SendFile(s, filename);
        }
        else if (strcmp(cmd, "get") == 0 && filename != NULL) {
            ReceiveFile(s, filename);
        }
        else if (strcmp(cmd, "exit") == 0) {
            break;
        }
        else {
            printf("Unknown command or missing filename\n");
        }
    }

    // Закрытие соединения
    closesocket(s);
    WSACleanup();
    return 0;
}