#include "DatabaseManager.h"
#include <iostream>
#include "../crypto/Password.h"

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
        std::cerr << "Khong the mo database: " << sqlite3_errmsg(db) << std::endl;
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
                      "force_change_password INTEGER DEFAULT 0);"
                      "CREATE TABLE IF NOT EXISTS wallets ("
                      "username TEXT PRIMARY KEY,"
                      "balance INTEGER DEFAULT 1000,"
                      "FOREIGN KEY(username) REFERENCES users(username));";
    exec_sql(sql);

    // Create default admin account if it doesn't exist
    string checkAdmin = "SELECT 1 FROM users WHERE username = 'ptit2025';";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, checkAdmin.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK)
    {
        if (sqlite3_step(stmt) != SQLITE_ROW)
        {
            // Hash the admin password
            string hashedPassword = crypto::Password::hash("ptit2025");

            // Admin account doesn't exist, create it
            string adminSql = "INSERT INTO users (username, password, full_name, email, is_admin, force_change_password) "
                              "VALUES ('ptit2025', '" +
                              hashedPassword + "', 'Admin', 'admin@ptit.edu.vn', 1, 0);";
            exec_sql(adminSql);

            // Initialize admin wallet
            string walletSql = "INSERT INTO wallets (username, balance) VALUES ('ptit2025', 1000);";
            exec_sql(walletSql);
        }
        sqlite3_finalize(stmt);
    }

    // Create default user account if it doesn't exist
    string checkUser = "SELECT 1 FROM users WHERE username = 'user2025';";
    rc = sqlite3_prepare_v2(db, checkUser.c_str(), -1, &stmt, nullptr);
    if (rc == SQLITE_OK)
    {
        if (sqlite3_step(stmt) != SQLITE_ROW)
        {
            // Hash the user password
            string hashedPassword = crypto::Password::hash("user2025");

            // User account doesn't exist, create it
            string userSql = "INSERT INTO users (username, password, full_name, email, is_admin, force_change_password) "
                             "VALUES ('user2025', '" +
                             hashedPassword + "', 'User', 'user@ptit.edu.vn', 0, 0);";
            exec_sql(userSql);

            // Initialize user wallet
            string walletSql = "INSERT INTO wallets (username, balance) VALUES ('user2025', 1000);";
            exec_sql(walletSql);
        }
        sqlite3_finalize(stmt);
    }
}

bool DatabaseManager::exec_sql(const string &sql)
{
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Loi SQL: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool DatabaseManager::save_user(const UserAccount &user)
{
    string sql = "INSERT INTO users (username, password, full_name, email, is_admin, force_change_password) "
                 "VALUES ('" +
                 user.username1() + "', '" + user.password1() + "', '" +
                 user.fullName1() + "', '" + user.email1() + "', " +
                 std::to_string(user.is_admin()) + ", " +
                 std::to_string(user.force_change_password()) + ");";

    if (!exec_sql(sql))
        return false;

    // Initialize wallet balance
    sql = "INSERT INTO wallets (username, balance) VALUES ('" + user.username1() + "', " + std::to_string(user.pointBalance1()) + ");";
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
    string sql = "SELECT username, password, full_name, email, is_admin, force_change_password FROM users WHERE username = '" + username + "';";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cerr << "Khong the lay thong tin nguoi dung: " << sqlite3_errmsg(db) << std::endl;
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
        user->set_force_change_password(sqlite3_column_int(stmt, 5));

        // Lấy số dư từ bảng wallets
        string walletSql = "SELECT balance FROM wallets WHERE username = '" + username + "';";
        sqlite3_stmt *walletStmt;
        rc = sqlite3_prepare_v2(db, walletSql.c_str(), -1, &walletStmt, nullptr);
        if (rc == SQLITE_OK && sqlite3_step(walletStmt) == SQLITE_ROW)
        {
            user->set_point_balance(sqlite3_column_int(walletStmt, 0));
        }
        sqlite3_finalize(walletStmt);

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
        std::cerr << "Khong the lay danh sach nguoi dung: " << sqlite3_errmsg(db) << std::endl;
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

bool DatabaseManager::update_wallet_balance(const string &username, int balance)
{
    string sql = "UPDATE wallets SET balance = " + std::to_string(balance) +
                 " WHERE username = '" + username + "';";
    return exec_sql(sql);
}

int DatabaseManager::get_wallet_balance(const string &username)
{
    string sql = "SELECT balance FROM wallets WHERE username = '" + username + "';";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    if (rc != SQLITE_OK)
    {
        std::cerr << "Khong the lay so du vi: " << sqlite3_errmsg(db) << std::endl;
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

bool DatabaseManager::transfer_points(const string &from_user, const string &to_user, int amount)
{
    if (amount <= 0)
        return false;

    int from_balance = get_wallet_balance(from_user);
    if (from_balance < amount)
        return false;

    string sql = "BEGIN TRANSACTION;";
    if (!exec_sql(sql))
        return false;

    sql = "UPDATE wallets SET balance = balance - " + std::to_string(amount) +
          " WHERE username = '" + from_user + "';";
    if (!exec_sql(sql))
    {
        exec_sql("ROLLBACK;");
        return false;
    }

    sql = "UPDATE wallets SET balance = balance + " + std::to_string(amount) +
          " WHERE username = '" + to_user + "';";
    if (!exec_sql(sql))
    {
        exec_sql("ROLLBACK;");
        return false;
    }

    sql = "COMMIT;";
    return exec_sql(sql);
}