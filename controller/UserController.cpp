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
        std::string rawPassword = crypto::Password::generateRandom(18);
        std::cout << "Mat khau tu dong duoc sinh: " << rawPassword << std::endl;
        std::cout << "Vui long luu lai mat khau nay. Ban se can doi mat khau khi dang nhap lan dau.\n";
        user.set_password(crypto::Password::hash(rawPassword));
        user.set_force_change_password(true);
    }
    // Nếu người dùng tự nhập mật khẩu, không yêu cầu đổi mật khẩu
    else
    {
        user.set_password(crypto::Password::hash(user.password1()));
        user.set_force_change_password(false);
    }

    // Thiết lập 1000 điểm mặc định
    user.set_point_balance(1000);

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

bool UserController::verifyOtp(std::string otp)
{
    int MAX = 3;
    bool check = false;
    do
    {
        std::cout << "Nhap ma OTP: ";
        std::cin.ignore();
        std::string userInput;
        std::getline(std::cin, userInput);

        if (!OTPManager::validateOTP(userInput, otp))
        {
            MAX--;
            std::cout << "Ma OTP khong dung. Vui long nhap lai. Ban con " << MAX << " lan!\n";
        }
        else
        {
            check = true;
        }
    } while (MAX > 0 && !check);
    return check;
}

bool UserController::updatePersonalInfo(UserAccount user, const std::string &newEmail)
{
    if (newEmail.empty())
    {
        std::cout << "Email khong duoc de trong.\n";
        return false;
    }

    std::string otp = OTPManager::generateOTP();
    OTPManager::sendOTP(otp, user.username1());

    if (!verifyOtp(otp))
        return false;

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

    if (!crypto::Password::verify(oldPass, user->password1()))
    {
        std::cout << "Mat khau cu khong dung.\n";
        delete user;
        return false;
    }

    std::string otp = OTPManager::generateOTP();
    OTPManager::sendOTP(otp, username);

    if (!verifyOtp(otp))
        return false;

    user->set_password(crypto::Password::hash(newPass));
    user->set_force_change_password(false);

    if (db_manager.update_user(*user))
    {
        for (auto &u : users)
        {
            if (u.username1() == username)
            {
                u.set_password(crypto::Password::hash(newPass));
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
    std::cin >> oldPass;

    if (!crypto::Password::verify(oldPass, user_account.password1()))
    {
        std::cout << "Mat khau cu khong dung.\n";
        return false;
    }

    std::cout << "Nhap mat khau moi: ";
    std::cin >> newPass;

    std::cout << "Nhap lai mat khau moi: ";
    std::cin >> reNewPass;

    return changePasswordWithUsername(user_account.username1(), oldPass, newPass, reNewPass);
}

UserAccount *UserController::login(const std::string &username, const std::string &password)
{
    if (username.empty() || password.empty())
    {
        std::cout << "Ten dang nhap va mat khau khong duoc de trong.\n";
        return nullptr;
    }

    // Kiem tra do dai ten dang nhap
    if (username.length() < 3 || username.length() > 20)
    {
        std::cout << "Ten dang nhap phai co do dai tu 3 den 20 ky tu.\n";
        return nullptr;
    }

    // Kiem tra khoang trang
    if (username.find(' ') != std::string::npos)
    {
        std::cout << "Ten dang nhap khong duoc chua khoang trang.\n";
        return nullptr;
    }

    // Kiem tra ky tu dac biet
    std::string specialChars = "!@#$%^&*()_+{}[]|\\:;\"'<>,.?/~`";
    for (char c : username)
    {
        if (specialChars.find(c) != std::string::npos)
        {
            std::cout << "Ten dang nhap khong duoc chua ky tu dac biet.\n";
            return nullptr;
        }
    }

    UserAccount *user = db_manager.get_user(username);
    if (!user)
    {
        std::cout << "Tai khoan khong ton tai.\n";
        return nullptr;
    }

    if (!crypto::Password::verify(password, user->password1()))
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
    std::string newPassword;
    std::cout << "Nhap mat khau moi: ";
    std::getline(std::cin, newPassword);

    if (newPassword.empty())
    {
        std::cout << "Mat khau moi khong duoc de trong.\n";
        return false;
    }

    std::string otp = OTPManager::generateOTP();
    OTPManager::sendOTP(otp, user.username1());

    if (!verifyOtp(otp))
        return false;

    user.set_password(crypto::Password::hash(newPassword));
    user.set_force_change_password(false);

    if (db_manager.update_user(user))
    {
        for (auto &u : users)
        {
            if (u.username1() == user.username1())
            {
                u.set_password(crypto::Password::hash(newPassword));
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
    return crypto::Password::generateRandom(length);
}

bool UserController::adminUpdateUserWithOTP(UserAccount &user, string newName, string newEmail)
{
    if (newEmail.empty())
    {
        std::cout << "Email khong duoc de trong.\n";
        return false;
    }
    if (newName.empty())
    {
        std::cout << "Fullname khong duoc de trong.\n";
        return false;
    }

    std::string otp = OTPManager::generateOTP();
    OTPManager::sendOTP(otp, user.username1());

    if (!verifyOtp(otp))
        return false;

    user.set_email(newEmail);
    user.set_full_name(newName);
    if (db_manager.update_user(user))
    {
        for (auto &u : users)
        {
            if (u.username1() == user.username1())
            {
                u.set_email(newEmail);
                u.set_full_name(newName);
                break;
            }
        }
        std::cout << "Cap nhat thong tin thanh cong!\n";
        return true;
    }
    std::cout << "Khong the cap nhat thong tin. Vui long thu lai sau.\n";
    return false;
}
