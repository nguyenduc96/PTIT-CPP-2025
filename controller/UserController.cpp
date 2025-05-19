//
// Created by Dell on 5/9/2025.
//

#include "UserController.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "../utils/utils.h"
#include <filesystem>

UserController::UserController() : db_manager("reward_wallet.db")
{
    loadUsersFromFile();
}

void UserController::loadUsersFromFile()
{
    users = db_manager.get_alluser();
}

void UserController::listAllAccount()
{
    if (users.empty())
    {
        std::cout << "Khong co tai khoan nao trong he thong.\n";
        return;
    }
    for (const auto &user : users)
    {
        displayUser(user);
    }
}

void UserController::displayUser(UserAccount user_account)
{
    std::cout << user_account.toString();
}

bool UserController::createAccount(UserAccount user)
{
    if (db_manager.get_user(user.username1()) != nullptr)
    {
        std::cout << "Ten dang nhap '" << user.username1() << "' da ton tai.\n";
        return false;
    }

    // Nếu mật khẩu trống, sinh mật khẩu tự động và yêu cầu đổi mật khẩu
    if (user.password1().empty())
    {
        std::string rawPassword = generateRandomPassword(18);
        std::cout << "Mat khau tu dong duoc sinh: " << rawPassword << std::endl;
        user.set_password(rawPassword);
        user.set_force_change_password(true);
    }
    // Nếu người dùng tự nhập mật khẩu, không yêu cầu đổi mật khẩu
    else
    {
        user.set_force_change_password(false);
    }

    if (db_manager.save_user(user))
    {
        users.push_back(user);
        std::cout << "Tao tai khoan thanh cong!\n";
        return true;
    }
    std::cout << "Khong the tao tai khoan. Vui long thu lai sau.\n";
    return false;
}

bool UserController::createAccountByAdmin(const UserAccount &user)
{
    return createAccount(user);
}

bool UserController::userExists(const std::string &username)
{
    return db_manager.get_user(username) != nullptr;
}

bool UserController::updatePersonalInfo(UserAccount user, const std::string &newEmail)
{
    if (newEmail.empty())
    {
        std::cout << "Email khong duoc de trong.\n";
        return false;
    }

    user.set_email(newEmail);
    if (db_manager.update_user(user))
    {
        for (auto &u : users)
        {
            if (u.username1() == user.username1())
            {
                u.set_email(newEmail);
                break;
            }
        }
        std::cout << "Cap nhat thong tin thanh cong!\n";
        return true;
    }
    std::cout << "Khong the cap nhat thong tin. Vui long thu lai sau.\n";
    return false;
}

bool UserController::changePasswordWithUsername(const std::string &username, const std::string &oldPass,
                                                const std::string &newPass, const std::string &reNewPass)
{
    if (newPass.empty())
    {
        std::cout << "Mat khau moi khong duoc de trong.\n";
        return false;
    }

    if (reNewPass != newPass)
    {
        std::cout << "Mat khau moi nhap lai khong dung.\n";
        return false;
    }

    UserAccount *user = db_manager.get_user(username);
    if (!user)
    {
        std::cout << "Khong tim thay tai khoan.\n";
        return false;
    }

    if (user->password1() != oldPass)
    {
        std::cout << "Mat khau cu khong dung.\n";
        delete user;
        return false;
    }

    user->set_password(newPass);
    user->set_force_change_password(false);

    if (db_manager.update_user(*user))
    {
        for (auto &u : users)
        {
            if (u.username1() == username)
            {
                u.set_password(newPass);
                u.set_force_change_password(false);
                break;
            }
        }
        std::cout << "Doi mat khau thanh cong!\n";
        delete user;
        return true;
    }
    std::cout << "Khong the doi mat khau. Vui long thu lai sau.\n";
    delete user;
    return false;
}

bool UserController::changePassword(const UserAccount &user_account)
{
    std::string oldPass, newPass, reNewPass;

    std::cout << "Nhap mat khau cu: ";
    std::getline(std::cin, oldPass);

    std::cout << "Nhap mat khau moi: ";
    std::getline(std::cin, newPass);

    std::cout << "Nhap lai mat khau moi: ";
    std::getline(std::cin, reNewPass);

    return changePasswordWithUsername(user_account.username1(), oldPass, newPass, reNewPass);
}

UserAccount *UserController::login(const std::string &username, const std::string &password)
{
    if (username.empty() || password.empty())
    {
        std::cout << "Ten dang nhap va mat khau khong duoc de trong.\n";
        return nullptr;
    }

    UserAccount *user = db_manager.get_user(username);
    if (!user)
    {
        std::cout << "Tai khoan khong ton tai.\n";
        return nullptr;
    }

    if (user->password1() != password)
    {
        std::cout << "Mat khau khong dung.\n";
        delete user;
        return nullptr;
    }

    return user;
}

UserAccount *UserController::getUserByUsername(const std::string &username)
{
    if (username.empty())
    {
        std::cout << "Ten dang nhap khong duoc de trong.\n";
        return nullptr;
    }
    return db_manager.get_user(username);
}

bool UserController::changePasswordWithOTP(UserAccount &user)
{
    std::string otp = OTPManager::generateOTP();
    OTPManager::sendOTP(otp, user.username1());

    std::cout << "Nhap ma OTP: ";
    std::string userInput;
    std::getline(std::cin, userInput);

    if (!OTPManager::validateOTP(userInput, otp))
    {
        std::cout << "Ma OTP khong dung. Huy thao tac.\n";
        return false;
    }

    std::string newPassword;
    std::cout << "Nhap mat khau moi: ";
    std::getline(std::cin, newPassword);

    if (newPassword.empty())
    {
        std::cout << "Mat khau moi khong duoc de trong.\n";
        return false;
    }

    user.set_password(newPassword);
    user.set_force_change_password(false);

    if (db_manager.update_user(user))
    {
        for (auto &u : users)
        {
            if (u.username1() == user.username1())
            {
                u.set_password(newPassword);
                u.set_force_change_password(false);
                break;
            }
        }
        std::cout << "Doi mat khau thanh cong!\n";
        return true;
    }
    std::cout << "Khong the doi mat khau. Vui long thu lai sau.\n";
    return false;
}

std::string UserController::generateRandomPassword(int length)
{
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<> dist(0, chars.size() - 1);

    std::string password;
    for (int i = 0; i < length; ++i)
    {
        password += chars[dist(engine)];
    }
    return password;
}
