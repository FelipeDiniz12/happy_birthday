// Header for authentication util functions

#include <string>

namespace AuthenticationUtils
{
    enum AUTHENTICATION_ERROR : char
    {
        NO_ERROR=0,
        WRONG_PASSWORD_REPETITION=1,
        ALREADY_EXISTING_USER=2,
        WRONG_PASSWORD=3,
    };

    /**
     * @brief Perform user registration, prompting messages for
     * username and password (twice) and checking if passwords match.
     * @return true if registration works, false if not.
     */
    auto registerUser() -> bool;

    /**
     * @brief Perform user login, prompting messages for
     * username and password and checking if passwords match.
     * @return true if login works, false if not.
     */
    auto loginUser() -> bool;

    /**
     * @brief Transform a string reference to lower case. A good option
     * to padronize string before compare them with expected values.
     */
    void toLowerCase(std::string& str);
}