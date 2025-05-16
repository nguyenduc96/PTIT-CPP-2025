//
// Created by Dell on 5/9/2025.
//

#include "UserController.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "../utils//utils.h"
#include <filesystem>

const static std::string FILENAME = "../users.txt";
const static std::string FILENAME_TEMP = "../users_tmp.txt";
const static std::string FILENAME_BACKUP = "../users_backup.txt";

bool UserController::userExists(const std::string& username) {
    for (int i = 0; i < users.size(); ++i) {
        if (users[i].username1() == username) {
            return true;
        }
    }
    return false;
}

void UserController::displayUser(UserAccount user_account) {
    std::cout << user_account.toString();
}

void UserController::listAllAccount() {
    for (int i = 0; i < users.size(); ++i) {
        displayUser(users[i]);
    }
}


bool UserController::createAccountByAdmin(const UserAccount &user) {
    return createAccount(user);
}

bool UserController::createAccount(UserAccount user) {
    if (userExists(user.username1())) {
        std::cout << "username " << user.username1() << " da ton tai";
        return false;
    }
    if (user.password1().empty()) {
        std::string rawPassword = generateRandomPassword();
        std::cout << "Mat khau tu dong duoc sinh: " << rawPassword << std::endl;
        user.set_password(rawPassword);
        user.set_force_change_password(true);
    } else {
        user.set_force_change_password(false);
    }
    backupFile(FILENAME, FILENAME_BACKUP);

    std::ofstream file(FILENAME, std::ios::app);
    file << user.username1() << "," << user.password1() << ","
            << user.full_name() << "," << user.email1() << ","
            << user.point_balance() << "," << user.is_admin() << ","
            << user.is_active() << "," << user.force_change_password() << "\n";
    users.push_back(user);
    std::cout << "tai khoan tao thanh cong" << std::endl;
    return true;
}

bool UserController::updatePersonalInfo(UserAccount user, const std::string &newEmail) {
    std::ifstream file(FILENAME);
    std::ofstream temp(FILENAME_TEMP);
    std::string line;
    bool changed = false;

    for (int i = 0; i < users.size(); ++i) {
        if (users[i].username1() == user.username1()) {
            users[i].set_email(newEmail);
        }
    }

    while (getline(file, line)) {
        std::stringstream ss(line);


        std::string word;
        std::pmr::vector<std::string> row;
        while (std::getline(ss,word, ',')) {
            row.push_back(word);
        }
        bool isAdmin = (row[4] == "1");
        int balance = std::stoi(row[5]);
        bool isActive = (row[6] == "1");
        bool forceChangePassword = (row[7] == "1");
        std::string email = row[3];

        if (row[0] == user.username1()) {
            email = newEmail;
            changed = true;
        }
        temp << row[0] << "," << row[1] << "," << row[2] << "," << email << "," << balance << ","
             << isAdmin << "," << isActive << "," << forceChangePassword << "\n";
    }

    file.close();
    temp.close();
    std::filesystem::remove(FILENAME);
    std::filesystem::rename(FILENAME_TEMP, FILENAME);

    return changed;
}

UserAccount * UserController::getUserByUsername(const std::string &username) {
    for (int i = 0; i < users.size(); ++i) {
        if (users[i].username1() == username) {
            return &users[i];
        }
    }
    return nullptr;
}

bool UserController::changePassword(const UserAccount &user_account) {
    std::string oldPass, newPass, reNewPass;
    std::cout << "Nhap mat khau cu: "; std::cin.ignore(); std::getline(std::cin, oldPass);
    std::cout << "Nhap mat khau moi: "; std::cin.ignore(); std::getline(std::cin, newPass);
    std::cout << "Nhap mat lai khau moi: "; std::getline(std::cin, reNewPass);
    changePasswordWithUsername(user_account.username1(), oldPass, newPass, reNewPass);
}

bool UserController::changePasswordWithUsername(const std::string& username, const std::string& oldPass, const std::string& newPass, const std::string& reNewPass) {
    if (reNewPass != newPass) {
        std::cout << "Mat khau moi nhap lai khong dung";
        return  false;
    }
    std::ifstream file(FILENAME);
    std::ofstream temp(FILENAME_TEMP);
    std::string line;
    bool changed = false;

    for (int i = 0; i < users.size(); ++i) {
        if (users[i].username1() == username) {
            users[i].set_password(newPass);
            users[i].set_force_change_password(false);
        }
    }

    while (getline(file, line)) {
        std::stringstream ss(line);
        std::string word;
        std::pmr::vector<std::string> row;
        while (std::getline(ss,word, ',')) {
            row.push_back(word);
        }
        bool isAdmin = (row[4] == "1");
        int balance = std::stoi(row[5]);
        bool isActive = (row[6] == "1");
        bool forceChangePassword = (row[7] == "1");
        std::string pass = row[1];
        if (row[0] == username && pass == oldPass) {
            pass = newPass;
            forceChangePassword = false;
            changed = true;
        }
        temp << row[0] << "," << pass << "," << row[2] << "," << row[3] << "," << balance << ","
             << isAdmin << "," << isActive << "," << forceChangePassword << "\n";
    }

    file.close();
    temp.close();
    std::filesystem::remove(FILENAME);
    std::filesystem::rename(FILENAME_TEMP, FILENAME);

    return changed;
}

UserAccount* UserController::login(const std::string& username, const std::string& password) {
    for (auto & user : users) {
        UserAccount* user_account = &user;
        if (user_account->username1() == username && user_account->password1() == password) {
            return user_account;
        }
    }
    return nullptr;
}

void UserController::loadUsersFromFile() {
    std::ifstream file(FILENAME);
    std::ofstream temp(FILENAME_TEMP);
    std::string line;

    while (getline(file, line, '\n')) {
        std::stringstream ss(line);
        std::string word;
        std::pmr::vector<std::string> row;
        while (std::getline(ss,word, ',')) {
            row.push_back(word);
        }
        bool isAdmin = (row[4] == "1");
        int balance = std::stoi(row[5]);
        bool isActive = (row[6] == "1");
        bool forceChangePassword = (row[7] == "1");
        UserAccount user(row[0], row[1], row[2], row[3], isAdmin, balance, isActive, forceChangePassword);
        users.push_back(user);
    }
    file.close();
}


