//
// Created by Dell on 5/9/2025.
//

#ifndef USERACCOUNT_H
#define USERACCOUNT_H

#pragma once
#include <string>
#include <sstream>

class UserAccount
{
private:
    std::string username;
    std::string password;
    std::string fullName;
    std::string email;
    int pointBalance;
    bool isAdmin;
    bool forceChangePassword;

public:
    UserAccount();

    // Getters
    std::string username1() const;
    std::string password1() const;
    std::string fullName1() const;
    std::string email1() const;
    int pointBalance1() const;
    bool is_admin() const;
    bool force_change_password() const;

    // Setters
    void set_username(const std::string &uname);
    void set_password(const std::string &pwd);
    void set_full_name(const std::string &name);
    void set_email(const std::string &mail);
    void set_point_balance(int points);
    void set_is_admin(bool admin);
    void set_force_change_password(bool force);

    std::string toString() const;
};

#endif // USERACCOUNT_H
