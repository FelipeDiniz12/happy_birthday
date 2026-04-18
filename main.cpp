
#include "AuthenticationUtils.h"
#include "SQLUtils.h"

#include <cassert>
#include <cctype>
#include <cstring>
#include <iostream>
#include <string>

#define MAXIMUM_ATTEMPTS 3

namespace
{
enum Mode : char {
    LOGIN = 0,
    REGISTER = 1,
    INVALID = 2,
};
}

auto main() -> int
{
    // Create database if it does not exist.
    SQLUtils::getDatabase("users.db",
                          "CREATE TABLE IF NOT EXISTS users(id INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "username TEXT NOT NULL UNIQUE, password TEXT NOT NULL);");
    std::cout << "========= Welcome to HappyBirthday =========\n";
    std::string option;
    while (true) {
        std::cout << "Login, register or exit? ";
        std::getline(std::cin, option);
        AuthenticationUtils::toLowerCase(option);
        Mode mode;
        if (option == "login") {
            mode = Mode::LOGIN;
        } else if (option == "register") {
            mode = Mode::REGISTER;
        } else if (option == "exit") {
            std::cout << "Exiting program...\n";
            return 0;
        } else {
            mode = Mode::INVALID;
        }

        switch (mode) {
        case Mode::LOGIN:
            AuthenticationUtils::loginUser(MAXIMUM_ATTEMPTS);
            break;
        case Mode::REGISTER:
            AuthenticationUtils::registerUser(MAXIMUM_ATTEMPTS);
            break;
        case Mode::INVALID:
            std::cout << "\n";
            std::cerr << "Invalid option. Choose a valid option.\n";
            break;
        default:
            assert(0 && "Invalid mode");
            break;
        }
    }
    std::cout << "\n";
    std::cout << "Ending program...\n";
    return 0;
}