//
// Created by Dell on 5/9/2025.
//

#include "UserView.h"
#include <iostream>
#include "../controller/UserController.h"

void UserView::showCreateAccountForm(UserAccount &user)
{
    std::cout << "=== Tao tai khoan moi ===\n";
    std::string uname;
    bool usernameExists;
    bool isValidUsername;

    do
    {
        std::cout << "\nYeu cau ten dang nhap:\n";
        std::cout << "- Do dai tu 3 den 20 ky tu\n";
        std::cout << "- Khong chua khoang trang\n";
        std::cout << "- Khong chua ky tu dac biet\n";
        std::cout << "Ten dang nhap: ";
        std::cin >> uname;

        // Kiem tra do dai
        if (uname.length() < 3 || uname.length() > 20)
        {
            std::cout << "Ten dang nhap phai co do dai tu 3 den 20 ky tu.\n";
            isValidUsername = false;
            continue;
        }

        // Kiem tra khoang trang
        if (uname.find(' ') != std::string::npos)
        {
            std::cout << "Ten dang nhap khong duoc chua khoang trang.\n";
            isValidUsername = false;
            continue;
        }

        // Kiem tra ky tu dac biet
        std::string specialChars = "!@#$%^&*()_+{}[]|\\:;\"'<>,.?/~`";
        bool hasSpecialChar = false;
        for (char c : uname)
        {
            if (specialChars.find(c) != std::string::npos)
            {
                hasSpecialChar = true;
                break;
            }
        }
        if (hasSpecialChar)
        {
            std::cout << "Ten dang nhap khong duoc chua ky tu dac biet.\n";
            isValidUsername = false;
            continue;
        }

        usernameExists = UserController().userExists(uname);
        if (usernameExists)
        {
            std::cout << "Ten dang nhap da ton tai. Vui long chon ten khac.\n";
            isValidUsername = false;
            continue;
        }

        isValidUsername = true;
    } while (!isValidUsername);

    user.set_username(uname);

    // Nhap mat khau
    std::cout << "\nYeu cau mat khau:\n";
    std::cout << "- Do dai toi thieu 6 ky tu\n";
    std::cout << "- Chua it nhat 1 chu hoa, 1 chu thuong, 1 so\n";
    std::cout << "Mat khau (bo trong neu muon he thong tu sinh): ";
    std::cin.ignore();
    std::string pwd;
    std::getline(std::cin, pwd);

    // Neu khong de trong, kiem tra mat khau
    if (!pwd.empty())
    {
        bool isValidPassword = false;
        while (!isValidPassword)
        {
            // Kiem tra do dai
            if (pwd.length() < 6)
            {
                std::cout << "Mat khau phai co it nhat 6 ky tu.\n";
                std::cout << "Nhap lai mat khau: ";
                std::getline(std::cin, pwd);
                continue;
            }

            // Kiem tra chu hoa, chu thuong, so
            bool hasUpper = false;
            bool hasLower = false;
            bool hasDigit = false;

            for (char c : pwd)
            {
                if (std::isupper(c))
                    hasUpper = true;
                if (std::islower(c))
                    hasLower = true;
                if (std::isdigit(c))
                    hasDigit = true;
            }

            if (!hasUpper || !hasLower || !hasDigit)
            {
                std::cout << "Mat khau phai chua it nhat 1 chu hoa, 1 chu thuong va 1 so.\n";
                std::cout << "Nhap lai mat khau: ";
                std::getline(std::cin, pwd);
                continue;
            }

            // Nhap lai mat khau de xac nhan
            std::cout << "Nhap lai mat khau de xac nhan: ";
            std::string confirmPwd;
            std::getline(std::cin, confirmPwd);

            if (pwd != confirmPwd)
            {
                std::cout << "Mat khau nhap lai khong khop.\n";
                std::cout << "Nhap lai mat khau: ";
                std::getline(std::cin, pwd);
                continue;
            }

            isValidPassword = true;
        }
    }

    user.set_password(pwd);

    // Nhap ho ten
    std::cout << "\nHo ten: ";
    std::string fullname;
    std::getline(std::cin, fullname);
    while (fullname.empty())
    {
        std::cout << "Ho ten khong duoc de trong. Vui long nhap lai: ";
        std::getline(std::cin, fullname);
    }
    user.set_full_name(fullname);

    // Nhap email
    std::cout << "\nEmail: ";
    std::string email;
    std::getline(std::cin, email);
    while (email.empty() || email.find('@') == std::string::npos)
    {
        std::cout << "Email khong hop le. Vui long nhap lai: ";
        std::getline(std::cin, email);
    }
    user.set_email(email);

    user.set_is_admin(false);

    // Thong bao ve so diem khoi tao
    std::cout << "\nTai khoan moi se duoc cap 1000 diem thuong ban dau.\n";
}

void UserView::showMessage(const std::string &message)
{
    std::cout << message << std::endl;
}

void UserView::showLoginForm(std::string &username, std::string &password) {
    std::cout << "===== Dang nhap =====\n";
    std::cout << "Yeu cau ten dang nhap:\n";
    std::cout << "- Do dai tu 3 den 20 ky tu\n";
    std::cout << "- Khong chua khoang trang\n";
    std::cout << "- Khong chua ky tu dac biet\n";

    std::cout << "Ten dang nhap: ";
    std::cin >> username;

    std::cout << "Mat khau: ";
    std::cin >> password;
}
