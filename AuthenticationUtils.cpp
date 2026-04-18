
#include "AuthenticationUtils.h"
#include "SQLUtils.h"

#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <openssl/evp.h>
#include <sqlite3.h>
#include <sstream>
#include <termios.h>
#include <unistd.h>

#define HASH_LENGTH 26

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
auto makePasswordHash(std::string& password, const std::string& salt = "felipinho") -> std::string
{
    std::string saltedPass = password + salt;
    std::array<unsigned char, EVP_MAX_MD_SIZE> hash;
    unsigned int lengthOfHash = HASH_LENGTH;

    EVP_MD_CTX* context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(context, EVP_sha256(), NULL);
    EVP_DigestUpdate(context, saltedPass.c_str(), saltedPass.length());
    EVP_DigestFinal_ex(context, hash.data(), &lengthOfHash);
    EVP_MD_CTX_free(context);
    password.assign(password.length(), '0');
    // Store 'hash' and 'salt' in your database

    std::stringstream strStream;
    for (unsigned int i = 0; i < lengthOfHash; i++) {
        strStream << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return strStream.str();
}

/**
 * @brief Perform user login, prompting messages for
 * username and password and checking if passwords match.
 * @return true if login works, false if not.
 */
auto getPassword(std::string& password) -> std::string
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
auto verifyUsernameDatabase(const std::string& username, const std::string& databasePath) -> bool
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
        std::cout << "User: \"" << username << "\" was registered in database.\n";
    }
}
} // namespace

auto AuthenticationUtils::registerUser(const uint maximumAttempts) -> bool
{
    uint attempt;
    std::string username;

    for (attempt = 1; attempt <= maximumAttempts; ++attempt) {
        std::cout << "Enter your username: ";
        std::getline(std::cin, username);

        if (username == "") {
            std::cout << "Username cannot be empty!\nTry once again...\n";
        } else {
            break;
        }
    }

    if (attempt > maximumAttempts) {
        std::cerr << "Maximum attempts reached. Registration failed.\n";
        return false;
    }

    std::cout << "Enter your password: ";
    std::string password;
    std::string pwd = getPassword(password);

    for (attempt = 1; attempt <= maximumAttempts; ++attempt) {
        std::cout << "\nEnter your password again: ";
        std::string checkPassword;
        std::string checkPwd = getPassword(checkPassword);
        std::cout << "\n";
        if (pwd != checkPwd) {
            std::cout << "\n";
            std::cout << "Passwords differ from each other!\nTry once again...\n";
        } else {
            break;
        }
    }

    if (attempt > maximumAttempts) {
        std::cerr << "Maximum attempts reached. Registration failed.\n";
        return false;
    }

    if (!::verifyUsernameDatabase(username, "users.db")) {
        return false;
    }

    ::addUsernameInDatabase(username, pwd, "users.db");
    return true;
}

auto AuthenticationUtils::loginUser(const uint maximumAttempts) -> bool
{
    uint attempt;
    std::string username;

    for (attempt = 1; attempt <= maximumAttempts; ++attempt) {
        std::cout << "Enter your username: ";
        std::getline(std::cin, username);

        if (username == "") {
            std::cout << "Username cannot be empty!\nTry once again...\n";
        } else {
            break;
        }
    }

    if (attempt > maximumAttempts) {
        std::cerr << "Maximum attempts reached. Login failed.\n";
        return false;
    }

    std::cout << "Enter your password: ";
    std::string password;
    std::string pwd = getPassword(password);
    std::cout << "\n";

    if (!SQLUtils::checkQueryCondition(
            "users.db", "SELECT ID FROM users WHERE username = ? LIMIT 1;", {username})) {
        std::cerr << "User \"" << username << "\" not found in database\n";
        return false;
    }

    bool returnValue = false;
    for (attempt = 1; attempt < maximumAttempts; ++attempt) {
        if (SQLUtils::checkQueryCondition(
                "users.db", "SELECT ID FROM users WHERE username = ? AND password = ? LIMIT 1;",
                {username, pwd})) {
            std::cout << "Successfull authentication!\n";
            returnValue = true;
        } else {
            std::cout << "Wrong password!\nTry once again...\n";
            std::cout << "Enter your password: ";
            std::string password;
            pwd = getPassword(password);
            std::cout << "\n";
        }
        if (returnValue) {
            // Password is correct, break the loop
            break;
        }
    }
    if (attempt >= maximumAttempts) {
        std::cerr << "Maximum attempts reached. Login failed.\n";
    }
    return returnValue;
}

void AuthenticationUtils::toLowerCase(std::string& str)
{
    std::transform(str.begin(), str.end(), str.begin(),
                   [](unsigned char character) { return std::tolower(character); });
}