#include "chat_client.h"
#include "menu.h"
#include <iostream>
#include <thread>

int main() {
    ChatClient client;

    std::string server_ip = Menu::GetUserInput("Enter server IP: ");
    int server_port = std::stoi(Menu::GetUserInput("Enter server port: "));

    if (!client.Init(server_ip, server_port)) {
        std::cerr << "Failed to connect to server." << "\n";
        return 1;
    }
    client.Start();

    return 0;
}