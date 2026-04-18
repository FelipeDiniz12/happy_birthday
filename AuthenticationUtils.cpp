
#include "AuthenticationUtils.h"
#include "SQLUtils.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <openssl/evp.h>
#include <sqlite3.h>
#include <sstream>
#include <termios.h>
#include <unistd.h>

namespace
{
/**
 * @brief Transforms a given password into a hash to avoid leaking/storing plain
 * text passwords.
 * @param password A string reference for the password to be salted and hashed
 * @param salt A salt string to be applied with the password before hashing it.
 * Its default value is "felipinho".
 * @return Returns a string hash after salting it and encrypting it.
 */
std::string makePasswordHash(std::string& password, const std::string& salt = "felipinho")
{
    std::string saltedPass = password + salt;
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 26;

    EVP_MD_CTX* context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(context, EVP_sha256(), NULL);
    EVP_DigestUpdate(context, saltedPass.c_str(), saltedPass.length());
    EVP_DigestFinal_ex(context, hash, &lengthOfHash);
    EVP_MD_CTX_free(context);
    password.assign(password.length(), '0');
    // Store 'hash' and 'salt' in your database

    std::stringstream ss;
    for (unsigned int i = 0; i < lengthOfHash; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}

/**
 * @brief Perform user login, prompting messages for
 * username and password and checking if passwords match.
 * @return true if login works, false if not.
 */
std::string getPassword(std::string& password)
{
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt); // Save current terminal settings
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;                   // Disable character echoing
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Apply new settings

    std::getline(std::cin, password); // Read input normally
    std::string pwd = makePasswordHash(password);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore original settings

    return pwd;
}

/**
 * @brief Validate if a user exists in user database
 */
bool verifyUsernameDatabase(const std::string& username, const std::string& databasePath)
{
    if (SQLUtils::checkQueryCondition(
            databasePath, "SELECT ID FROM users WHERE username = ? LIMIT 1;", {username})) {
        std::cerr << "Username: \"" << username << "\" already exists...";
        return false;
    }
    return true;
}

/**
 * @brief Adds a username, and its hashed password, to the database.
 * @param username A string with the user name.
 * @param password A string for the hashed passowrd for the given user.
 * @param databasePath The string for the database path.
 */
void addUsernameInDatabase(const std::string& username, const std::string& password,
                           const std::string& databasePath)
{
    if (SQLUtils::addUserToTable(username, password, databasePath)) {
        std::cout << "User: \"" << username << "\" was registered in database." << std::endl;
    }
}
} // namespace

bool AuthenticationUtils::registerUser()
{
    std::cout << "Enter your username: ";
    std::string username;
    std::getline(std::cin, username);
    std::cout << "Enter your password: ";
    std::string password;
    std::string pwd = getPassword(password);
    std::cout << std::endl;
    std::cout << "Enter your password again: ";
    std::string checkPassword;
    std::string checkPwd = getPassword(checkPassword);
    std::cout << std::endl;

    if (pwd != checkPwd) {
        std::cout << std::endl;
        std::cerr << "Passwords differ from each other!" << std::endl;
        return 1;
    }

    if (!::verifyUsernameDatabase(username, "users.db")) {
        return false;
    }

    ::addUsernameInDatabase(username, pwd, "users.db");
    return true;
}

bool AuthenticationUtils::loginUser()
{
    std::cout << "Enter your username: ";
    std::string username;
    std::getline(std::cin, username);
    std::cout << "Enter your password: ";
    std::string password;
    std::string pwd = getPassword(password);
    std::cout << std::endl;

    if (!SQLUtils::checkQueryCondition(
            "users.db", "SELECT ID FROM users WHERE username = ? LIMIT 1;", {username})) {
        std::cerr << "User \"" << username << "\" not found in database" << std::endl;
        return false;
    }

    if (SQLUtils::checkQueryCondition(
            "users.db", "SELECT ID FROM users WHERE username = ? AND password = ? LIMIT 1;",
            {username, password})) {
        std::cout << "Successfull authentication!" << std::endl;
        return true;
    } else {
        std::cout << "Wrong password!" << std::endl;
        return false;
    }
}

void AuthenticationUtils::toLowerCase(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
}