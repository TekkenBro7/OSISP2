#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <algorithm>
#include <nlohmann/json.hpp> // Подключаем библиотеку nlohmann/json

using json = nlohmann::json;

struct ClientInfo {
    int socket;
    std::string name;
};

std::vector<ClientInfo> clients;
std::mutex clients_mutex;

ClientInfo* find_client_by_name(const std::string& name) {
    std::lock_guard<std::mutex> guard(clients_mutex);
    for (auto& client : clients) {
        if (client.name == name) {
            return &client;
        }
    }
    return nullptr;
}

void handle_client(int client_socket) {
    char buffer[1024];
    int bytes_received;
    std::string client_name;

    while (true) {
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received <= 0) {
            std::cout << "Client disconnected." << std::endl;
            break;
        }

        buffer[bytes_received] = '\0';

        json message = json::parse(buffer);

        if (message["type"] == "register") {
            client_name = message["from"];
            {
                std::lock_guard<std::mutex> guard(clients_mutex);
                clients.push_back({client_socket, client_name});
            }
            std::cout << client_name << " has joined the chat." << std::endl;

            json join_message = {
                {"type", "system"},
                {"from", "Server"},
                {"to", "all"},
                {"body", client_name + " has joined the chat."}
            };
            std::string join_message_str = join_message.dump();
            {
                std::lock_guard<std::mutex> guard(clients_mutex);
                for (const auto& client : clients) {
                    if (client.socket != client_socket) {
                        send(client.socket, join_message_str.c_str(), join_message_str.length(), 0);
                    }
                }
            }
        } else if (message["type"] == "message") {
            std::string to = message["to"];
            std::string from = message["from"];
            std::string body = message["body"];

            std::string full_message = "[" + from + "]: " + body;
            std::cout << full_message << std::endl;

            if (to == "all") {
                std::lock_guard<std::mutex> guard(clients_mutex);
                for (const auto& client : clients) {
                    if (client.socket != client_socket) {
                        send(client.socket, buffer, bytes_received, 0);
                    }
                }
            } else {
                ClientInfo* target_client = find_client_by_name(to);
                if (target_client) {
                    send(target_client->socket, buffer, bytes_received, 0);
                } else {
                    std::cerr << "Client '" << to << "' not found." << std::endl;
                }
            }
        }
    }

    close(client_socket);

    {
        std::lock_guard<std::mutex> guard(clients_mutex);
        clients.erase(std::remove_if(clients.begin(), clients.end(),
                                     [client_socket](const ClientInfo& client) {
                                         return client.socket == client_socket;
                                     }),
                      clients.end());
    }

    json leave_message = {
        {"type", "system"},
        {"from", "Server"},
        {"to", "all"},
        {"body", client_name + " has left the chat."}
    };
    std::string leave_message_str = leave_message.dump();
    {
        std::lock_guard<std::mutex> guard(clients_mutex);
        for (const auto& client : clients) {
            send(client.socket, leave_message_str.c_str(), leave_message_str.length(), 0);
        }
    }
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(54000);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Bind failed." << std::endl;
        close(server_socket);
        return 1;
    }

    if (listen(server_socket, SOMAXCONN) == -1) {
        std::cerr << "Listen failed." << std::endl;
        close(server_socket);
        return 1;
    }

    std::cout << "Server is listening on port 54000..." << std::endl;

    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket == -1) {
            std::cerr << "Accept failed." << std::endl;
            continue;
        }

        std::cout << "New client connected." << std::endl;
        std::thread client_thread(handle_client, client_socket);
        client_thread.detach();
    }

    close(server_socket);
    return 0;
}