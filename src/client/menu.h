#pragma once

#include <string>

class Menu {
public:
    enum Input { LOGIN = 1, LOGOUT = 2, PRIVATE_MSG = 3, GROUP_MSG = 4, USER_LIST = 5, EXIT = 6 };
    // 显示主菜单
    static void ShowMainMenu();

    // 获取用户输入的选项
    static int GetUserChoice();

    // 获取用户输入的字符串
    static std::string GetUserInput(const std::string& prompt);
};
