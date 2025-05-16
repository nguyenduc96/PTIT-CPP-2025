//
// Created by Dell on 5/9/2025.
//

#include "UserView.h"
#include <iostream>

void UserView::showCreateAccountForm(UserAccount& user) {
    std::cout << "=== Tao tai khoan moi ===\n";
    std::cout << "Ten dang nhap: ";
    std::string uname;
    std::cin >> uname;
    user.set_username(uname);
    std::cout << "Mat khau (bo trong neu muon he thong tu sinh): ";
    std::cin.ignore();
    std::string pwd;
    std::getline(std::cin, pwd);
    user.set_password(pwd);
    std::string fullname;
    std::cout << "Ho ten: "; std::getline(std::cin, fullname);
    user.set_full_name(fullname);
    std::string email;
    std::cout << "Email: "; std::getline(std::cin, email);
    user.set_email(email);
    user.set_is_admin(false);
    user.set_point_balance(0);
    user.set_is_active(true);
}

void UserView::showMessage(const std::string& message) {
    std::cout << message << std::endl;
}

void UserView::showLoginForm(std::string& username, std::string& password) {
    std::cout << "===== Dang nhap =====\n";
    std::cout << "Ten dang nhap: ";
    std::cin >> username;
    std::cout << "Mat khau: ";
    std::cin >> password;
}
