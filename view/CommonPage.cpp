#include "CommonPage.h"
#include <iostream>

CommonPage::CommonPage(UserController &uc, WalletController &wc, UserView &usv)
    : userController(uc), walletController(wc), userView(usv) {}

void CommonPage::showWelcomeMenu()
{
    int choice;
    std::string input;
    do
    {
        std::cout << "\n==== CHAO MUNG DEN HE THONG ====" << std::endl;
        std::cout << "1. Dang nhap" << std::endl;
        std::cout << "2. Dang ky" << std::endl;
        std::cout << "0. Thoat" << std::endl;
        std::cout << "Chon: ";
        std::getline(std::cin, input);
        choice = std::stoi(input);

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

void CommonPage::showLoginMenu()
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

        UserAccount *user = userController.login(username, password);
        if (user == nullptr)
        {
            std::cout << "Mat khau sai. Vui long kiem tra lai.\n";
            attempts++;
            continue;
        }

        if (user->force_change_password())
        {
            std::cout << "\nTai khoan cua ban can cap nhat mat khau!\n";
            std::cout << "Vui long doi mat khau de tiep tuc.\n";
            bool passwordChanged = false;
            while (!passwordChanged)
            {
                if (userController.changePassword(*user))
                {
                    passwordChanged = true;
                    std::cout << "Doi mat khau thanh cong! Ban co the tiep tuc su dung he thong.\n";
                }
                else
                {
                    std::cout << "Doi mat khau khong thanh cong. Ban co muon thu lai? (y/n): ";
                    char choice;
                    std::string input;
                    std::getline(std::cin, input);
                    choice = input[0];
                    if (choice != 'y' && choice != 'Y')
                    {
                        std::cout << "Ban phai doi mat khau de tiep tuc su dung he thong.\n";
                        delete user;
                        return;
                    }
                }
            }
        }

        if (user->is_admin())
            adminMenu(*user);
        else
            normalUserMenu(*user);

        delete user;
        return;
    }

    std::cout << "Ban da nhap sai qua 3 lan. Quay lai menu chinh.\n";
}

void CommonPage::registerNewUser()
{
    UserAccount user_account;
    userView.showCreateAccountForm(user_account);
    userController.createAccount(user_account);
}

void CommonPage::adminMenu(UserAccount &admin)
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
        std::cout << "6. Xem lich su giao dich" << std::endl;
        std::cout << "0. Thoat" << std::endl;
        std::cout << "Chon: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        switch (choice)
        {
        case 1:
            userController.displayUser(admin);
            break;
        case 2:
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::string newName, newEmail;
            std::cout << "Ho ten moi: ";
            std::getline(std::cin, newName);
            std::cout << "Email moi: ";
            std::getline(std::cin, newEmail);
            userController.adminUpdateUserWithOTP(admin, newName, newEmail);
            break;
        }
        case 3:
        {
            std::string uname, fname, email;
            std::cout << "Ten dang nhap: ";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin >> uname;
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
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Ten moi: ";
            std::getline(std::cin, newName);
            std::cout << "Email moi: ";
            std::getline(std::cin, newEmail);
            userController.adminUpdateUserWithOTP(*target, newName, newEmail);
            break;
        }
        case 6: {
            std::string uname;
            std::cout << "Ten tai khoan can xem lich su: ";
            std::cin >> uname;
            userController.showTransactionHistory(uname);
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

void CommonPage::normalUserMenu(UserAccount &user)
{
    int choice;
    std::string input;
    do
    {
        std::cout << "\n==== MENU NGUOI DUNG ====" << std::endl;
        std::cout << "1. Xem thong tin ca nhan" << std::endl;
        std::cout << "2. Cap nhat thong tin (OTP xac thuc)" << std::endl;
        std::cout << "3. Doi mat khau" << std::endl;
        std::cout << "4. Xem so du vi" << std::endl;
        std::cout << "5. Chuyen diem" << std::endl;
        std::cout << "6. Xem lich su giao dich" << std::endl;
        std::cout << "0. Thoat" << std::endl;
        std::cout << "Chon: ";
        std::getline(std::cin, input);
        choice = std::stoi(input);

        switch (choice)
        {
        case 1:
            userController.displayUser(user);
            break;
        case 2:
        {
            std::string newName, newEmail;
            std::cout << "Ho ten moi: ";
            std::getline(std::cin, newName);
            std::cout << "Email moi: ";
            std::getline(std::cin, newEmail);
            userController.updatePersonalInfo(user, newName, newEmail);
            break;
        }
        case 3:
        {
            userController.changePassword(user);
            break;
        }
        case 4:
            std::cout << "So du vi: " << walletController.getWalletBalance(user.username1()) << " diem\n";
            break;
        case 5:
        {
            std::string toUser;
            int amount;
            std::cout << "Tai khoan nhan: ";
            std::getline(std::cin, toUser);
            std::cout << "So diem chuyen: ";
            std::getline(std::cin, input);
            amount = std::stoi(input);
            if (toUser == user.username1())
            {
                std::cout << "Ban khong the chuyen tien cho chinh minh";
                break;
            }
            UserAccount *receiver = userController.getUserByUsername(toUser);
            if (receiver)
            {
                walletController.transferPoints(user, *receiver, amount);
                int latestBalance = walletController.getWalletBalance(user.username1());
                user.set_point_balance(latestBalance);
            }
            else
                std::cout << "Nguoi nhan khong ton tai.\n";
            break;
        }
        case 6: {
            userController.showTransactionHistory(user.username1());
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
