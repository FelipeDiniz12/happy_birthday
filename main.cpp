
#include "AuthenticationUtils.h"
#include "SQLUtils.h"

#include <cctype>
#include <cstring>
#include <iostream>
#include <string>

#define LOGIN 0
#define REGISTER 1
#define MAXIMUM_ATTEMPTS 3

int main()
{
    // Create database if it does not exist.
    SQLUtils::getDatabase("users.db",
                          "CREATE TABLE IF NOT EXISTS users(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "username TEXT NOT NULL UNIQUE, password TEXT NOT NULL);");
    std::cout << "========= Welcome to HappyBirthday =========" << std::endl;
    std::string option;
    std::cout << "Login or register? ";
    std::getline(std::cin, option);
    AuthenticationUtils::toLowerCase(option);
    bool mode;
    if (option == "login") {
        mode = LOGIN;
    } else if (option == "register") {
        mode = REGISTER;
    } else {
        std::cout << std::endl;
        std::cerr << "Invalid option. Choose either to login or register" << std::endl;
        return 1;
    }

    if (mode == LOGIN) {
        if (!AuthenticationUtils::loginUser()) {
            return 1;
        }
    } else {
        if (!AuthenticationUtils::registerUser()) {
            return 1;
        }
    }
    std::cout << std::endl;
    std::cout << "Ending program..." << std::endl;
    return 0;
}