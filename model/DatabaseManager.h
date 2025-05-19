#pragma once
#include "../sqlite/sqlite3.h"
#include <vector>
#include "../model/UserAccount.h"

using namespace std;

class DatabaseManager
{
private:
    sqlite3 *db;
    string db_path;

    bool exec_sql(const string &sql);
    void init_table();

public:
    DatabaseManager(const string &db_path = "db.db");
    ~DatabaseManager();

    bool open_connect();
    void close_connect();

    bool save_user(const UserAccount &user);
    bool update_user(const UserAccount &user);
    bool delete_user(const string &username);

    UserAccount *get_user(const string &username);
    vector<UserAccount> get_alluser();

    bool update_wallet_bal(const string &username, int bal);
    int get_wallet_bal(const string &username);
    bool transfer_bal(const string &fron_user, const string &to_user, int val);
};