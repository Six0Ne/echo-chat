#pragma once

#include <string>

class Menu {
public:
    // 显示主菜单
    static void ShowMainMenu();

    // 获取用户输入的选项
    static int GetUserChoice();

    // 获取用户输入的字符串
    static std::string GetUserInput(const std::string& prompt);
};
