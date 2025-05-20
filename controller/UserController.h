//
// Created by Dell on 5/9/2025.
//

#ifndef USERCONTROLLER_H
#define USERCONTROLLER_H

#pragma once
#include <iostream>
#include <vector>
#include <random>
#include "../model/UserAccount.h"
#include "../model/OTPManager.h"
#include "../model/DatabaseManager.h"
#include "../crypto/Password.h"
using namespace std;

class UserController
{
private:
    DatabaseManager db_manager;
    std::vector<UserAccount> users;
    std::string generateRandomPassword(int length = 10);

public:
    UserController();
    void loadUsersFromFile();
    void listAllAccount();
    void displayUser(UserAccount user_account);
    bool createAccount(UserAccount user);
    bool createAccountByAdmin(const UserAccount &user);
    bool userExists(const std::string &username);

    bool verifyOtp(std::string otp);

    bool updatePersonalInfo(UserAccount user, const std::string &newEmail);
    bool changePasswordWithUsername(const std::string &username, const std::string &oldPass,
                                    const std::string &newPass, const std::string &reNewPass);
    bool changePassword(const UserAccount &user_account);
    UserAccount *login(const std::string &username, const std::string &password);
    UserAccount *getUserByUsername(const std::string &username);
    bool changePasswordWithOTP(UserAccount &user);
    bool adminUpdateUserWithOTP(UserAccount &user, string newName, string newEmail);
};

#endif // USERCONTROLLER_H
