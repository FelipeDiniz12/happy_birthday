#include "SQLUtils.h"

#include <iostream>
#include <sqlite3.h>
#include <vector>

namespace
{
/**
 * @brief Executes a SQL query.
 * @param databasePath The database in which the SQL query will be run.
 * @param query A tuple containing the SQL Query to be executed and a list of elements to replace
 * placeholders.
 * @return Returns an integer related with the query result.
 */
auto executeSqlQuery(const std::string& databasePath,
                     std::tuple<std::string, std::vector<std::string>> query) -> int
{
    auto* database = SQLUtils::getDatabase(databasePath);
    if (database == nullptr) {
        // database is a nullptr
        std::cerr << "Database does not exist...\n";
        return -1;
    }

    sqlite3_stmt* stmt;

    // 1. Prepare the statement
    if (sqlite3_prepare_v2(database, std::get<0>(query).c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error in statement...\n";
        std::cerr << "Error: " << sqlite3_errmsg(database) << "\n";
        sqlite3_close(database);
        return -1;
    }

    // 2. Bind the username to the '?' placeholder to prevent SQL injection
    int index = 1;
    for (const auto& element : std::get<1>(query)) {
        sqlite3_bind_text(stmt, index, element.c_str(), -1, SQLITE_STATIC);
        index++;
    }

    // 3. Execute the statement
    int executionResult = sqlite3_step(stmt);

    // 4. Finalize to clean up memory
    sqlite3_finalize(stmt);
    sqlite3_close(database);
    return executionResult;
}
} // namespace

namespace SQLUtils
{
auto getDatabase(const std::string& databasePath, const std::string& query) -> sqlite3*
{
    sqlite3* database;
    if (sqlite3_open(databasePath.c_str(), &database) != SQLITE_OK) {
        std::cerr << "Error: " << sqlite3_errmsg(database) << "\n";
        return nullptr;
    }

    if (query != "") {
        char* errMsg = nullptr;
        if (sqlite3_exec(database, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "SQL Error: " << errMsg << "\n";
            sqlite3_free(errMsg);
        }
    }

    return database;
}

auto checkQueryCondition(const std::string& databasePath, const std::string& query,
                         const std::vector<std::string>& listOfElements) -> bool
{
    auto result = executeSqlQuery(databasePath, std::make_tuple(query, listOfElements));

    bool found = false;
    if (result == SQLITE_ROW) {
        found = true;
    }

    return found;
}

auto addUserToTable(const std::string& username, const std::string& password,
                    const std::string& databasePath) -> bool
{
    auto result = executeSqlQuery(
        databasePath, std::make_tuple("INSERT INTO users (username, password) VALUES (?, ?);",
                                      std::vector<std::string>{username, password}));

    bool userAdded = true;
    if (result != SQLITE_DONE) {
        std::cerr << "Error inserting user\n";
        userAdded = false;
    }

    return userAdded;
}
} // namespace SQLUtils