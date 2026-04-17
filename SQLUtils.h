
// Headers
#include <string>
#include <vector>

// Forward declarations
class sqlite3;

namespace SQLUtils
{
    /**
     * @brief Either creates or returns a database pointer. If wanted, a command can be run after
     * collecting the database.
     * @param databasePath The database path user wants to use.
     * @param commandToRun Command to be run after opening/creating the database.
     * @return Returns a pointer of sqlite3 for the database requrested.
     */
    sqlite3* getDatabase(const std::string& databasePath, const std::string& commandToRun = "");

    /**
     * @brief Check if a given query in a database returns a valid ID or not.
     * @param databasePath The database path where the query is to be done.
     * @param query The query to be done.
     * @param listOfElements The user can set a list of elements to be replaced in the original
     * query.
     * @return Returns true if query is successful, otherwise, false.
     */
    bool checkQueryCondition(const std::string& databasePath, const std::string& query, const std::vector<std::string>& listOfElements = {});

    /**
     * @brief Add a user to the database.
     * @param username The string of the user to be added to the database.
     * @param password The hashed password to the added along with the user.
     * @return Returns true if the user was added to the table, otherwise, false.
     */
    bool addUserToTable(const std::string& username, const std::string& password, const std::string& databasePath = "users.db");
}