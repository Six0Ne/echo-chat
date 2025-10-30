#include "chat_client.h"
#include "menu.h"
#include <iostream>

int main() {
    ChatClient client;

    std::string server_ip = Menu::GetUserInput("Enter server IP: ");
    int server_port = std::stoi(Menu::GetUserInput("Enter server port: "));

    if (!client.Connect(server_ip, server_port)) {
        std::cerr << "Failed to connect to server." << std::endl;
        return 1;
    }

    bool running = true;
    while (running) {
        Menu::ShowMainMenu();
        int choice = Menu::GetUserChoice();

        switch (choice) {
        case 1: {  // Login
            std::string username = Menu::GetUserInput("Enter username: ");
            std::string password = Menu::GetUserInput("Enter password: ");
            client.Login(username, password);
            break;
        }
        case 2: {  // Logout
            client.Logout();
            break;
        }
        case 3: {  // Send Private Message
            std::string target_user = Menu::GetUserInput("Enter target username: ");
            std::string message = Menu::GetUserInput("Enter message: ");
            client.SendPrivateMessage(target_user, message);
            break;
        }
        case 4: {  // Send Group Message
            std::string message = Menu::GetUserInput("Enter message: ");
            client.SendGroupMessage(message);
            break;
        }
        case 5: {  // View Online Users
            client.RequestUserList();
            break;
        }
        case 6: {  // Exit
            running = false;
            client.Logout();
            break;
        }
        default:
            std::cerr << "Invalid choice. Please try again." << std::endl;
        }
    }

    return 0;
}