#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <string>
#include <nlohmann/json.hpp> // Подключаем библиотеку nlohmann/json

using json = nlohmann::json;

void receive_messages(int socket) {
    char buffer[1024];
    int bytes_received;

    while (true) {
        bytes_received = recv(socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            std::cout << "Server disconnected." << std::endl;
            break;
        }

        buffer[bytes_received] = '\0';

        json message = json::parse(buffer);
        std::cout << "[" << message["from"] << "] " << message["body"] << std::endl;
    }
}

int main() {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(54000);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Connection failed." << std::endl;
        close(client_socket);
        return 1;
    }

    std::cout << "Connected to server." << std::endl;

    std::string name;
    std::cout << "Enter your name: ";
    std::getline(std::cin, name);

    json register_message = {
        {"type", "register"},
        {"from", name},
        {"to", "server"},
        {"body", ""}
    };
    std::string register_message_str = register_message.dump();
    send(client_socket, register_message_str.c_str(), register_message_str.length(), 0);

    std::thread receive_thread(receive_messages, client_socket);
    receive_thread.detach();

    while (true) {
        std::string message_body;
        std::string to_client;

        std::cout << "Enter recipient (or 'all' for broadcast): ";
        std::getline(std::cin, to_client);

        std::cout << "Enter your message: ";
        std::getline(std::cin, message_body);

        if (message_body.empty()) {
            continue;
        }

        json message = {
            {"type", "message"},
            {"from", name},
            {"to", to_client},
            {"body", message_body}
        };

        std::string message_str = message.dump();
        send(client_socket, message_str.c_str(), message_str.length(), 0);
    }

    close(client_socket);
    return 0;
}