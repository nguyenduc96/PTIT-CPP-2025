#include "DatabaseManager.h"
#include <iostream>

using namespace std;

DatabaseManager::DatabaseManager(const string &db_path) : db_path(db_path), db(nullptr)
{
    open_connect();
    init_table();
}

DatabaseManager::~DatabaseManager()
{
    close_connect();
}

bool DatabaseManager::open_connect()
{
    int rc = sqlite3_open(db_path.c_str(), &db);
    if (rc)
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    return true;
}

void DatabaseManager::close_connect()
{
    if (db)
    {
        sqlite3_close(db);
        db = nullptr;
    }
}

void DatabaseManager::init_table()
{
    const char *sql = "CREATE TABLE IF NOT EXISTS users ("
                      "username TEXT PRIMARY KEY,"
                      "password TEXT NOT NULL,"
                      "full_name TEXT NOT NULL,"
                      "email TEXT NOT NULL,"
                      "is_admin INTEGER DEFAULT 0,"
                      "point_balance INTEGER DEFAULT 0,"
                      "force_change_password INTEGER DEFAULT 0);"
                      "CREATE TABLE IF NOT EXISTS wallets ("
                      "username TEXT PRIMARY KEY,"
                      "balance INTEGER DEFAULT 0,"
                      "FOREIGN KEY(username) REFERENCES users(username));";
    exec_sql(sql);
}

bool DatabaseManager::exec_sql(const string &sql)
{
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool DatabaseManager::save_user(const UserAccount &user)
{
    string sql = "INSERT INTO users (username, password, full_name, email, is_admin, point_balance, force_change_password) "
                 "VALUES ('" +
                 user.username1() + "', '" + user.password1() + "', '" +
                 user.fullName1() + "', '" + user.email1() + "', " +
                 std::to_string(user.is_admin()) + ", " + std::to_string(user.pointBalance1()) + ", " +
                 std::to_string(user.force_change_password()) + ");";

    if (!exec_sql(sql))
        return false;

    // Initialize wallet balance
    sql = "INSERT INTO wallets (username, balance) VALUES ('" + user.username1() + "', 0);";
    return exec_sql(sql);
}

bool DatabaseManager::update_user(const UserAccount &user)
{
    string sql = "UPDATE users SET "
                 "password = '" +
                 user.password1() + "', "
                                    "full_name = '" +
                 user.fullName1() + "', "
                                    "email = '" +
                 user.email1() + "', "
                                 "is_admin = " +
                 std::to_string(user.is_admin()) + ", "
                                                   "point_balance = " +
                 std::to_string(user.pointBalance1()) + ", "
                                                        "force_change_password = " +
                 std::to_string(user.force_change_password()) + " "
                                                                "WHERE username = '" +
                 user.username1() + "';";
    return exec_sql(sql);
}

bool DatabaseManager::delete_user(const string &username)
{
    string sql = "DELETE FROM wallets WHERE username = '" + username + "';";
    if (!exec_sql(sql))
        return false;

    sql = "DELETE FROM users WHERE username = '" + username + "';";
    return exec_sql(sql);
}

UserAccount *DatabaseManager::get_user(const string &username)
{
    string sql = "SELECT * FROM users WHERE username = '" + username + "';";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to fetch user: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        UserAccount *user = new UserAccount();
        user->set_username((const char *)sqlite3_column_text(stmt, 0));
        user->set_password((const char *)sqlite3_column_text(stmt, 1));
        user->set_full_name((const char *)sqlite3_column_text(stmt, 2));
        user->set_email((const char *)sqlite3_column_text(stmt, 3));
        user->set_is_admin(sqlite3_column_int(stmt, 4));
        user->set_point_balance(sqlite3_column_int(stmt, 5));
        user->set_force_change_password(sqlite3_column_int(stmt, 6));
        sqlite3_finalize(stmt);
        return user;
    }

    sqlite3_finalize(stmt);
    return nullptr;
}

vector<UserAccount> DatabaseManager::get_alluser()
{
    vector<UserAccount> users;
    string sql = "SELECT * FROM users;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to fetch users: " << sqlite3_errmsg(db) << std::endl;
        return users;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        UserAccount user;
        user.set_username((const char *)sqlite3_column_text(stmt, 0));
        user.set_password((const char *)sqlite3_column_text(stmt, 1));
        user.set_full_name((const char *)sqlite3_column_text(stmt, 2));
        user.set_email((const char *)sqlite3_column_text(stmt, 3));
        user.set_is_admin(sqlite3_column_int(stmt, 4));
        user.set_point_balance(sqlite3_column_int(stmt, 5));
        user.set_force_change_password(sqlite3_column_int(stmt, 6));
        users.push_back(user);
    }

    sqlite3_finalize(stmt);
    return users;
}

bool DatabaseManager::update_wallet_bal(const string &username, int bal)
{
    string sql = "UPDATE wallets SET balance = " + std::to_string(bal) +
                 " WHERE username = '" + username + "';";
    return exec_sql(sql);
}

int DatabaseManager::get_wallet_bal(const string &username)
{
    string sql = "SELECT balance FROM wallets WHERE username = '" + username + "';";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cerr << "Failed to fetch wallet balance: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    int balance = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        balance = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return balance;
}

bool DatabaseManager::transfer_bal(const string &from_user, const string &to_user, int val)
{
    if (val <= 0)
        return false;

    int from_bal = get_wallet_bal(from_user);
    if (from_bal < val)
        return false;

    string sql = "BEGIN TRANSACTION;";
    if (!exec_sql(sql))
        return false;

    sql = "UPDATE wallets SET balance = balance - " + std::to_string(val) +
          " WHERE username = '" + from_user + "';";
    if (!exec_sql(sql))
    {
        exec_sql("ROLLBACK;");
        return false;
    }

    sql = "UPDATE wallets SET balance = balance + " + std::to_string(val) +
          " WHERE username = '" + to_user + "';";
    if (!exec_sql(sql))
    {
        exec_sql("ROLLBACK;");
        return false;
    }

    sql = "COMMIT;";
    return exec_sql(sql);
}