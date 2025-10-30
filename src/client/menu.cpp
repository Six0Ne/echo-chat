#include "menu.h"
#include <iostream>
#include <limits>

void Menu::ShowMainMenu() {
    std::cout << "\n====== Chat Menu ======" << std::endl;
    std::cout << "1. Login" << std::endl;
    std::cout << "2. Logout" << std::endl;
    std::cout << "3. Send Private Message" << std::endl;
    std::cout << "4. Send Group Message" << std::endl;
    std::cout << "5. View Online Users" << std::endl;
    std::cout << "6. Exit" << std::endl;
    std::cout << "=======================" << std::endl;
}

int Menu::GetUserChoice() {
    int choice;
    while (true) {
        std::cout << "Enter your choice: ";
        if (std::cin >> choice) {
            if (choice >= 1 && choice <= 6) {
                return choice;
            } else {
                std::cout << "Invalid choice. Please enter a number between 1 and 6." << std::endl;
            }
        } else {
            std::cout << "Invalid input. Please enter a number." << std::endl;
            std::cin.clear();                                                    // 清除错误状态
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // 丢弃无效输入
        }
    }
}

std::string Menu::GetUserInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt;
    std::cin.clear();  // 清除错误状态
    std::cin.sync();   // 同步输入缓冲区
    std::getline(std::cin, input);
    return input;
}