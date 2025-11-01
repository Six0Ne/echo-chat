#include "menu.h"
#include <iostream>
#include <limits>

void Menu::ShowMainMenu() {
    std::cout << "\n====== Chat Menu ======" << "\n";
    std::cout << "1. Login" << "\n";
    std::cout << "2. Logout" << "\n";
    std::cout << "3. Send Private Message" << "\n";
    std::cout << "4. Send Group Message" << "\n";
    std::cout << "5. View Online Users" << "\n";
    std::cout << "6. Exit" << "\n";
    std::cout << "=======================" << "\n";
}

int Menu::GetUserChoice() {
    int choice;
    while (true) {
        std::cout << "Enter your choice: ";
        if (std::cin >> choice) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // 清空缓冲区
            if (choice >= 1 && choice <= 6) {
                return choice;
            } else {
                std::cout << "Invalid choice. Please enter a number between 1 and 6." << "\n";
            }
        } else {
            std::cout << "Invalid input. Please enter a number." << "\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

std::string Menu::GetUserInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}