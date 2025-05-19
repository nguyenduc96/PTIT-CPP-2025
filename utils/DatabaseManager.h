#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <string>
#include <vector>
#include "../sqlite/sqlite3.h"
#include "../model/UserAccount.h"

class DatabaseManager
{
private:
    sqlite3 *db;
    std::string dbPath;

public:
    DatabaseManager(const std::string &dbPath = "../reward_wallet.db");
    ~DatabaseManager();

    bool initialize();
    bool executeQuery(const std::string &query);
    bool executeQueryWithParams(const std::string &query, const std::vector<std::string> &params);

    bool createUserTable();
    bool insertUser(const UserAccount &user);
    bool updateUser(const UserAccount &user);
    bool updateUserPassword(const std::string &username, const std::string &newPassword, bool forceChange);
    bool updateUserEmail(const std::string &username, const std::string &newEmail);
    bool userExists(const std::string &username);
    UserAccount getUserByUsername(const std::string &username);
    std::vector<UserAccount> getAllUsers();

    bool createWalletTable();
    bool setWalletBalance(const std::string &username, int balance);
    int getWalletBalance(const std::string &username);
    bool transferPoints(const std::string &fromUsername, const std::string &toUsername, int amount);
};

#endif