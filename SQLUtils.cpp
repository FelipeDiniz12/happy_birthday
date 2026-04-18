#include "SQLUtils.h"

#include <iostream>
#include <sqlite3.h>
#include <vector>

namespace
{
/**
 * @brief Executes a SQL query.
 * @param query SQL Query to be executed.
 * @param databasePath The database in which the SQL query will be run.
 * @param listOfElements An optional argument to replace values in the original query.
 * @return Returns an integer related with the query result.
 */
int executeSqlQuery(const std::string& query, const std::string& databasePath,
                    const std::vector<std::string>& listOfElements)
{
    auto db = SQLUtils::getDatabase(databasePath);
    if (!db) {
        // db is a nullptr
        std::cerr << "Database does not exist..." << std::endl;
        return -1;
    }

    sqlite3_stmt* stmt;

    // 1. Prepare the statement
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, NULL) != SQLITE_OK) {
        std::cerr << "Error in statement..." << std::endl;
        std::cerr << "Error: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return -1;
    }

    // 2. Bind the username to the '?' placeholder to prevent SQL injection
    uint index = 1;
    for (const auto& element : listOfElements) {
        sqlite3_bind_text(stmt, index, element.c_str(), -1, SQLITE_STATIC);
        index++;
    }

    std::cout << "Statement is: " << sqlite3_expanded_sql(stmt) << std::endl;

    // 3. Execute the statement
    int executionResult = sqlite3_step(stmt);

    // 4. Finalize to clean up memory
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return executionResult;
}
} // namespace

namespace SQLUtils
{
sqlite3* getDatabase(const std::string& databasePath, const std::string& query)
{
    sqlite3* db;
    if (sqlite3_open(databasePath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Error: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }

    if (query != "") {
        char* errMsg = nullptr;
        if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "SQL Error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
    }

    return db;
}

bool checkQueryCondition(const std::string& databasePath, const std::string& query,
                         const std::vector<std::string>& listOfElements)
{
    auto result = executeSqlQuery(query, databasePath, listOfElements);

    bool found = false;
    if (result == SQLITE_ROW) {
        found = true;
    }

    return found;
}

bool addUserToTable(const std::string& username, const std::string& password,
                    const std::string& databasePath)
{
    auto result = executeSqlQuery("INSERT INTO users (username, password) VALUES (?, ?);",
                                  databasePath, {username, password});

    bool userAdded = true;
    if (result != SQLITE_DONE) {
        std::cerr << "Error inserting user" << std::endl;
        userAdded = false;
    }

    return userAdded;
}
} // namespace SQLUtils