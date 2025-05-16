//
// Created by Dell on 5/9/2025.
//

#ifndef USERACCOUNT_H
#define USERACCOUNT_H



#pragma once
#include <string>
#include <sstream>

class UserAccount {
private:
    std::string username;
    std::string password;
    std::string fullName;
    std::string email;
    bool isAdmin;
    int pointBalance;
    bool isActive;
    bool forceChangePassword;
    void loadFromStream(std::stringstream& ss);

public:
    UserAccount(){};

    UserAccount(const std::string &username, const std::string &password, const std::string &full_name,
        const std::string &email, bool is_admin, int point_balance, bool is_active, bool force_change_password)
        : username(username),
          password(password),
          fullName(full_name),
          email(email),
          isAdmin(is_admin),
          pointBalance(point_balance),
          isActive(is_active),
          forceChangePassword(force_change_password) {
    }
    [[nodiscard]] std::string username1() const {
        return username;
    }

    [[nodiscard]] std::string password1() const {
        return password;
    }

    [[nodiscard]] std::string full_name() const {
        return fullName;
    }

    [[nodiscard]] std::string email1() const {
        return email;
    }

    [[nodiscard]] bool is_admin() const {
        return isAdmin;
    }

    [[nodiscard]] int point_balance() const {
        return pointBalance;
    }

    [[nodiscard]] bool is_active() const {
        return isActive;
    }

    [[nodiscard]] bool force_change_password() const {
        return forceChangePassword;
    }

    void set_username(const std::string &username) {
        this->username = username;
    }

    void set_password(const std::string &password) {
        this->password = password;
    }

    void set_full_name(const std::string &full_name) {
        fullName = full_name;
    }

    void set_email(const std::string &email) {
        this->email = email;
    }

    void set_is_admin(bool is_admin) {
        isAdmin = is_admin;
    }

    void set_point_balance(int point_balance) {
        pointBalance = point_balance;
    }

    void set_is_active(bool is_active) {
        isActive = is_active;
    }

    void set_force_change_password(bool force_change_password) {
        forceChangePassword = force_change_password;
    }

    std::string toString() const {
        std::stringstream ss;
        ss << "Tên đăng nhập: " << username << "\n"
           << "Họ tên: " << fullName << "\n"
           << "Email: " << email << "\n"
           << "S điểm thưởng: " << pointBalance << "\n"
           << "Vai trò: " << (isAdmin ? "Quản lý" : "Người dùng") << "\n"
           << "Yêu cầu đổi mật khẩu: " << (forceChangePassword ? "Có" : "Không") << "\n";
        return ss.str();
    }
};


#endif //USERACCOUNT_H
