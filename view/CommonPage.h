//
// Created by Dell on 5/11/2025.
//

#ifndef COMMONPAGE_H
#define COMMONPAGE_H
#include "UserView.h"
#include "../model/UserAccount.h"
#include "../controller/UserController.h"
#include "../controller/WalletController.h"

class CommonPage
{
    UserController &userController;
    WalletController &walletController;
    UserView &userView;

public:
    CommonPage(UserController &uc, WalletController &wc, UserView &usv)
        : userController(uc), walletController(wc), userView(usv)
    {
    }

    void showWelcomeMenu()
    {
        int choice;
        do
        {
            std::cout << "\n==== CHAO MUNG DEN HE THONG ====" << std::endl;
            std::cout << "1. Dang nhap" << std::endl;
            std::cout << "2. Dang ky" << std::endl;
            std::cout << "0. Thoat" << std::endl;
            std::cout << "Chon: ";
            std::cin >> choice;

            switch (choice)
            {
            case 1:
                showLoginMenu();
                break;
            case 2:
                registerNewUser();
                break;
            case 0:
                std::cout << "Tam biet!\n";
                break;
            default:
                std::cout << "Lua chon khong hop le!\n";
            }
        } while (choice != 0);
    }

    void showLoginMenu()
    {
        int attempts = 0;
        const int MAX_ATTEMPTS = 3;
        std::string username;
        std::string password;

        while (attempts < MAX_ATTEMPTS)
        {
            userView.showLoginForm(username, password);
            if (!userController.userExists(username))
            {
                std::cout << "Tai khoan khong ton tai. Vui long kiem tra lai.\n";
                attempts++;
                continue;
            }

            UserAccount* user = userController.login(username, password);
            if (user == nullptr)
            {
                std::cout << "Mat khau sai. Vui long kiem tra lai.\n";
                attempts++;
                continue;
            }

            // Đăng nhập thành công
            if (user->force_change_password())
            {
                std::cout << "Tai khoan cua ban can cap nhat mat khau!\n";
                userController.changePassword(*user);
            }
            if (user->is_admin())
            {
                adminMenu(*user);
            }
            else
            {
                normalUserMenu(*user);
            }
            return;
        }

        std::cout << "Ban da nhap sai qua 3 lan. Quay lai menu chinh.\n";
    }

    void registerNewUser()
    {
        UserAccount user_account;
        userView.showCreateAccountForm(user_account);
        userController.createAccount(user_account);
    }

private:
    void adminMenu(UserAccount &admin)
    {
        int choice;
        do
        {
            std::cout << "\n==== MENU QUAN LY ====" << std::endl;
            std::cout << "1. Xem thong tin ca nhan" << std::endl;
            std::cout << "2. Cap nhat thong tin ca nhan (OTP)" << std::endl;
            std::cout << "3. Tao tai khoan nguoi dung" << std::endl;
            std::cout << "4. Xem danh sach tai khoan" << std::endl;
            std::cout << "5. Chinh sua ho nguoi dung (OTP gui cho ho)" << std::endl;
            std::cout << "0. Thoat" << std::endl;
            std::cout << "Chon: ";
            std::cin >> choice;

            switch (choice)
            {
            case 1:
                userController.displayUser(admin);
                break;
            case 2:
            {
                std::cin.ignore();
                std::string newName, newEmail;
                std::cout << "Ho ten moi: ";
                std::getline(std::cin, newName);
                std::cin.ignore();
                std::cout << "Email moi: ";
                std::getline(std::cin, newEmail);
                userController.adminUpdateUserWithOTP(admin, newName, newEmail);
                break;
            }
            case 3:
            {
                std::string uname, fname, email;
                std::cout << "Ten dang nhap: ";
                std::cin >> uname;
                std::cin.ignore();
                std::cout << "Ho ten: ";
                std::getline(std::cin, fname);
                std::cout << "Email: ";
                std::getline(std::cin, email);
                UserAccount user_account;
                user_account.set_username(uname);
                user_account.set_email(email);
                user_account.set_full_name(fname);
                userController.createAccountByAdmin(user_account);
                walletController.setWalletBalance(uname, 0);
                break;
            }
            case 4:
                userController.listAllAccount();
                break;
            case 5:
            {
                std::string uname;
                std::cout << "Ten tai khoan can cap nhat: ";
                std::cin >> uname;
                UserAccount *target = userController.getUserByUsername(uname);
                if (!target)
                {
                    std::cout << "Khong tim thay tai khoan.\n";
                    break;
                }
                std::string newName, newEmail;
                std::cin.ignore();
                std::cout << "Ten moi: ";
                std::getline(std::cin, newName);
                std::cout << "Email moi: ";
                std::getline(std::cin, newEmail);
                userController.adminUpdateUserWithOTP(*target, newName, newEmail);
                break;
            }
            case 0:
                std::cout << "Dang xuat.\n";
                break;
            default:
                std::cout << "Lua chon khong hop le.\n";
            }
        } while (choice != 0);
    }
    void normalUserMenu(UserAccount &user)
    {
        int choice;
        do
        {
            std::cout << "\n==== MENU NGUOI DUNG ====" << std::endl;
            std::cout << "1. Xem thong tin ca nhan" << std::endl;
            std::cout << "2. Cap nhat thong tin (OTP xac thuc)" << std::endl;
            std::cout << "3. Doi mat khau" << std::endl;
            std::cout << "4. Xem so du vi" << std::endl;
            std::cout << "5. Chuyen diem" << std::endl;
            std::cout << "0. Thoat" << std::endl;
            std::cout << "Chon: ";
            std::cin >> choice;

            switch (choice)
            {
            case 1:
                userController.displayUser(user);
                break;
            case 2:
            {
                std::cin.ignore();
                std::string newEmail;
                std::cout << "Email moi: ";
                std::getline(std::cin, newEmail);
                userController.updatePersonalInfo(user, newEmail);
                break;
            }
            case 3:
            {
                std::cin.ignore();
                userController.changePassword(user);
                break;
            }
            case 4:
                std::cout << "So du vi: " << walletController.getWalletBalance(user.username1()) << " diem\n";
                break;
            case 5:
            {
                std::cin.ignore();
                std::string toUser;
                int amount;
                std::cout << "Tai khoan nhan: ";
                std::cin >> toUser;
                std::cout << "So diem chuyen: ";
                std::cin >> amount;
                if (toUser == user.username1())
                {
                    std::cout << "Ban khong the chuyen tien cho chinh minh";
                    break;
                }
                UserAccount *receiver = userController.getUserByUsername(toUser);
                if (receiver)
                    walletController.transferPoints(user, *receiver, amount);
                else
                    std::cout << "Nguoi nhan khong ton tai.\n";
                break;
            }
            case 0:
                std::cout << "Dang xuat.\n";
                break;
            default:
                std::cout << "Lua chon khong hop le.\n";
            }
        } while (choice != 0);
    }
};

#endif // COMMONPAGE_H
