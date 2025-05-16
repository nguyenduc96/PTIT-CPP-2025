//
// Created by Dell on 5/9/2025.
//

#ifndef USERCONTROLLER_H
#define USERCONTROLLER_H



#pragma once
#include <iostream>

#include "../model/UserAccount.h"
#include "../model/OTPManager.h"

class UserController {
    std::vector<UserAccount> users;
public:
    void loadUsersFromFile();
    void listAllAccount();
    void displayUser(UserAccount user_account);
    bool createAccount(UserAccount user);
    bool createAccountByAdmin(const UserAccount &user);
    bool userExists(const std::string& username);
    bool updatePersonalInfo(UserAccount user, const std::string& newEmail);
    bool changePasswordWithUsername(const std::string& username, const std::string& oldPass, const std::string& newPass, const std::string& reNewPass);
    bool changePassword(const UserAccount &user_account);
    UserAccount* login(const std::string& username, const std::string& password);
    UserAccount* getUserByUsername(const std::string& username);
    bool changePasswordWithOTP(UserAccount& user) {
        std::string otp = OTPManager::generateOTP();
        OTPManager::sendOTP(otp, user.username1());
        std::cout << "Nhập mã OTP: ";
        std::string userInput;
        std::cin >> userInput;

        if (!OTPManager::validateOTP(userInput, otp)) {
            std::cout << "Mã OTP không đúng. Hủy thao tác.\n";
            return false;
        }

        std::string newPassword;
        std::cout << "Nhập mật khẩu mới: ";
        std::cin >> newPassword;

        user.set_force_change_password(false);
        std::cout << "Đổi mật khẩu thành công!\n";
        return true;
    }

private:
    std::string hashPassword(const std::string& raw) {
        std::hash<std::string> hasher;
        return std::to_string(hasher(raw));
    }

};




#endif //USERCONTROLLER_H
