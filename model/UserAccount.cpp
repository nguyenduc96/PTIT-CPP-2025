//
// Created by Dell on 5/9/2025.
//

#include "UserAccount.h"
#include <string>
#include <sstream>
#include <iostream>

UserAccount::UserAccount() : pointBalance(0), isAdmin(false), forceChangePassword(false) {}

// Getters
std::string UserAccount::username1() const { return username; }
std::string UserAccount::password1() const { return password; }
std::string UserAccount::fullName1() const { return fullName; }
std::string UserAccount::email1() const { return email; }
int UserAccount::pointBalance1() const { return pointBalance; }
bool UserAccount::is_admin() const { return isAdmin; }
bool UserAccount::force_change_password() const { return forceChangePassword; }

// Setters
void UserAccount::set_username(const std::string &uname) { username = uname; }
void UserAccount::set_password(const std::string &pwd) { password = pwd; }
void UserAccount::set_full_name(const std::string &name) { fullName = name; }
void UserAccount::set_email(const std::string &mail) { email = mail; }
void UserAccount::set_point_balance(int points) { pointBalance = points; }
void UserAccount::set_is_admin(bool admin) { isAdmin = admin; }
void UserAccount::set_force_change_password(bool force) { forceChangePassword = force; }

std::string UserAccount::toString() const
{
    std::stringstream ss;
    ss << "Ten dang nhap: " << username << "\n"
       << "Ho ten: " << fullName << "\n"
       << "Email: " << email << "\n"
       << "So diem thuong: " << pointBalance << "\n"
       << "Vai tro: " << (isAdmin ? "Quan ly" : "Nguoi dung") << "\n"
       << "Yeu cau doi mat khau: " << (forceChangePassword ? "Co" : "Khong") << "\n";
    return ss.str();
}
