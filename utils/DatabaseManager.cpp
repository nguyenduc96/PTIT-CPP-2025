#include "DatabaseManager.h"
#include <iostream>

DatabaseManager::DatabaseManager(const std::string &dbPath) : dbPath(dbPath), db(nullptr)
{
}

DatabaseManager::~DatabaseManager()
{
    if (db)
    {
        sqlite3_close(db);
    }
}

bool DatabaseManager::initialize()
{
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc)
    {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    if (!createUserTable() || !createWalletTable())
    {
        return false;
    }

    return true;
}

bool DatabaseManager::executeQuery(const std::string &query)
{
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

bool DatabaseManager::executeQueryWithParams(const std::string &query, const std::vector<std::string> &params)
{
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    for (size_t i = 0; i < params.size(); i++)
    {
        sqlite3_bind_text(stmt, i + 1, params[i].c_str(), -1, SQLITE_TRANSIENT);
    }

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE && rc != SQLITE_ROW)
    {
        std::cerr << "Execution failed: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    return true;
}

bool DatabaseManager::createUserTable()
{
    std::string query = "CREATE TABLE IF NOT EXISTS users ("
                        "username TEXT PRIMARY KEY,"
                        "password TEXT NOT NULL,"
                        "full_name TEXT NOT NULL,"
                        "email TEXT NOT NULL,"
                        "is_admin INTEGER NOT NULL,"
                        "force_change_password INTEGER NOT NULL"
                        ");";

    return executeQuery(query);
}

bool DatabaseManager::createWalletTable()
{
    std::string query = "CREATE TABLE IF NOT EXISTS wallets ("
                        "username TEXT PRIMARY KEY,"
                        "balance INTEGER NOT NULL,"
                        "FOREIGN KEY (username) REFERENCES users(username)"
                        ");";

    return executeQuery(query);
}

bool DatabaseManager::insertUser(const UserAccount &user)
{
    std::string query = "INSERT INTO users (username, password, full_name, email, is_admin, force_change_password) "
                        "VALUES (?, ?, ?, ?, ?, ?);";

    std::vector<std::string> params = {
        user.username1(),
        user.password1(),
        user.fullName1(),
        user.email1(),
        user.is_admin() ? "1" : "0",
        user.force_change_password() ? "1" : "0"};

    if (!executeQueryWithParams(query, params))
    {
        return false;
    }

    query = "INSERT INTO wallets (username, balance) VALUES (?, ?);";
    params = {user.username1(), std::to_string(user.pointBalance1())};

    return executeQueryWithParams(query, params);
}

bool DatabaseManager::updateUser(const UserAccount &user)
{
    std::string query = "UPDATE users SET "
                        "password = '" +
                        user.password1() + "', "
                                           "full_name = '" +
                        user.fullName1() + "', "
                                           "email = '" +
                        user.email1() + "', "
                                        "is_admin = " +
                        std::to_string(user.is_admin()) + ", "
                                                          "force_change_password = " +
                        std::to_string(user.force_change_password()) + " "
                                                                       "WHERE username = '" +
                        user.username1() + "';";
    return executeQuery(query);
}

bool DatabaseManager::updateUserPassword(const std::string &username, const std::string &newPassword, bool forceChange)
{
    std::string query = "UPDATE users SET password = ?, force_change_password = ? WHERE username = ?;";
    std::vector<std::string> params = {newPassword, forceChange ? "1" : "0", username};

    return executeQueryWithParams(query, params);
}

bool DatabaseManager::updateUserEmail(const std::string &username, const std::string &newEmail)
{
    std::string query = "UPDATE users SET email = ? WHERE username = ?;";
    std::vector<std::string> params = {newEmail, username};

    return executeQueryWithParams(query, params);
}

bool DatabaseManager::userExists(const std::string &username)
{
    sqlite3_stmt *stmt;
    std::string query = "SELECT 1 FROM users WHERE username = ?;";

    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    bool exists = (rc == SQLITE_ROW);
    sqlite3_finalize(stmt);

    return exists;
}

UserAccount DatabaseManager::getUserByUsername(const std::string &username)
{
    sqlite3_stmt *stmt;
    std::string query = "SELECT username, password, full_name, email, is_admin, force_change_password "
                        "FROM users WHERE username = ?;";

    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return UserAccount();
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);

    UserAccount user;
    if (rc == SQLITE_ROW)
    {
        std::string username = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        std::string password = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        std::string fullName = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        std::string email = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
        bool isAdmin = sqlite3_column_int(stmt, 4) != 0;
        bool forceChangePassword = sqlite3_column_int(stmt, 5) != 0;

        int balance = getWalletBalance(username);

        user.set_username(username);
        user.set_password(password);
        user.set_full_name(fullName);
        user.set_email(email);
        user.set_is_admin(isAdmin);
        user.set_point_balance(balance);
        user.set_force_change_password(forceChangePassword);
    }

    sqlite3_finalize(stmt);
    return user;
}

std::vector<UserAccount> DatabaseManager::getAllUsers()
{
    std::vector<UserAccount> users;
    sqlite3_stmt *stmt;
    std::string query = "SELECT username, password, full_name, email, is_admin, force_change_password FROM users;";

    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return users;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        std::string username = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 0));
        std::string password = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        std::string fullName = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        std::string email = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
        bool isAdmin = sqlite3_column_int(stmt, 4) != 0;
        bool forceChangePassword = sqlite3_column_int(stmt, 5) != 0;

        int balance = getWalletBalance(username);

        UserAccount user;
        user.set_username(username);
        user.set_password(password);
        user.set_full_name(fullName);
        user.set_email(email);
        user.set_is_admin(isAdmin);
        user.set_point_balance(balance);
        user.set_force_change_password(forceChangePassword);
        users.push_back(user);
    }

    sqlite3_finalize(stmt);
    return users;
}

bool DatabaseManager::setWalletBalance(const std::string &username, int balance)
{
    if (!userExists(username))
    {
        return false;
    }

    std::string query;
    if (getWalletBalance(username) >= 0)
    {
        query = "UPDATE wallets SET balance = ? WHERE username = ?;";
    }
    else
    {
        query = "INSERT INTO wallets (balance, username) VALUES (?, ?);";
    }

    std::vector<std::string> params = {std::to_string(balance), username};
    return executeQueryWithParams(query, params);
}

int DatabaseManager::getWalletBalance(const std::string &username)
{
    sqlite3_stmt *stmt;
    std::string query = "SELECT balance FROM wallets WHERE username = ?;";

    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);

    int balance = -1;
    if (rc == SQLITE_ROW)
    {
        balance = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return balance;
}

bool DatabaseManager::transferPoints(const std::string &fromUsername, const std::string &toUsername, int amount)
{
    if (amount <= 0)
    {
        return false;
    }

    int fromBalance = getWalletBalance(fromUsername);
    if (fromBalance < amount)
    {
        return false;
    }

    int toBalance = getWalletBalance(toUsername);
    if (toBalance < 0)
    {
        return false;
    }

    std::string beginTransaction = "BEGIN TRANSACTION;";
    std::string commitTransaction = "COMMIT;";
    std::string rollbackTransaction = "ROLLBACK;";

    if (!executeQuery(beginTransaction))
    {
        return false;
    }

    bool success = setWalletBalance(fromUsername, fromBalance - amount) &&
                   setWalletBalance(toUsername, toBalance + amount);

    if (success)
    {
        return executeQuery(commitTransaction);
    }
    else
    {
        executeQuery(rollbackTransaction);
        return false;
    }
}