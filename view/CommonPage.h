//
// Created by Dell on 5/11/2025.
//

#ifndef COMMONPAGE_H
#define COMMONPAGE_H
#include "UserView.h"
#include "../model/UserAccount.h"
#include "../controller/UserController.h"
#include "../controller/WalletController.h"

class CommonPage {
    UserController& userController;
    WalletController& walletController;
    UserView& userView;

public:
    CommonPage(UserController& uc, WalletController& wc, UserView& usv)
        : userController(uc), walletController(wc), userView(usv) {
    }

    void showWelcomeMenu() {
        int choice;
        do {
            std::cout << "\n==== CHÀO MỪNG ĐẾN VỚI HỆ THỐNG ====" << std::endl;
            std::cout << "1. Đăng nhập" << std::endl;
            std::cout << "2. Đăng ký tài khoản mới" << std::endl;
            std::cout << "0. Thoát" << std::endl;
            std::cout << "Chọn: ";
            std::cin >> choice;

            switch (choice) {
                case 1:
                    showLoginMenu();
                    break;
                case 2:
                    registerNewUser();
                    break;
                case 0:
                    std::cout << "Tạm biệt!\n";
                    break;
                default:
                    std::cout << "Lựa chọn không hợp lệ!\n";
            }
        } while (choice != 0);
    }

    void showLoginMenu() {
        std::string username;
        std::string password;
        userView.showLoginForm(username, password);
        UserAccount* user = userController.login(username, password);
        if (user == nullptr) {
            std::cout << "Sai tên tài khoản hoặc mật khẩu ";
        }
        if (user->force_change_password()) {
            std::cout << "Tài khoản của bạn cần cập nhật mật khẩu!\n";
            userController.changePassword(*user);
        }
        if (user->is_admin()) {
            adminMenu(*user);
        } else {
            normalUserMenu(*user);
        }
    }

    void registerNewUser() {
        UserAccount user_account;
        userView.showCreateAccountForm(user_account);
        userController.createAccount(user_account);
    }

private:
    void adminMenu(UserAccount& admin) {
        int choice;
        do {
            std::cout << "\n==== MENU QUẢN LÝ ====" << std::endl;
            std::cout << "1. Xem thông tin cá nhân" << std::endl;
            std::cout << "2. Cập nhật thông tin cá nhân (OTP)" << std::endl;
            std::cout << "3. Tạo tài khoản người dùng" << std::endl;
            std::cout << "4. Xem danh sách tài khoản" << std::endl;
            std::cout << "5. Chỉnh sửa hộ người dùng (OTP gửi cho họ)" << std::endl;
            std::cout << "0. Thoát" << std::endl;
            std::cout << "Chọn: ";
            std::cin >> choice;

            switch (choice) {
                case 1:
                    userController.displayUser(admin);
                    break;
                case 2: {
                    std::string newName, newEmail;
                    std::cout << "Họ tên mới: "; std::cin.ignore(); std::getline(std::cin, newName);
                    std::cout << "Email mới: "; std::getline(std::cin, newEmail);
                    // userController.updatePersonalInfo(admin, newName, newEmail);
                    break;
                }
                case 3: {
                    std::string uname, fname, email;
                    std::cout << "Tên đăng nhập: "; std::cin >> uname;
                    std::cin.ignore();
                    std::cout << "Họ tên: "; std::getline(std::cin, fname);
                    std::cout << "Email: "; std::getline(std::cin, email);
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
                case 5: {
                    std::string uname;
                    std::cout << "Tên tài khoản cần cập nhật: ";
                    std::cin >> uname;
                    UserAccount* target = userController.getUserByUsername(uname);
                    if (!target) {
                        std::cout << "Không tìm thấy tài khoản.\n";
                        break;
                    }
                    std::string newName, newEmail;
                    std::cin.ignore();
                    std::cout << "Tên mới: "; std::getline(std::cin, newName);
                    std::cout << "Email mới: "; std::getline(std::cin, newEmail);
                    // userController.adminUpdateUserWithOTP(*target, newName, newEmail);
                    break;
                }
                case 0:
                    std::cout << "Đăng xuất.\n";
                    break;
                default:
                    std::cout << "Lựa chọn không hợp lệ.\n";
            }
        } while (choice != 0);
    }
    void normalUserMenu(UserAccount& user) {
        int choice;
        do {
            std::cout << "\n==== MENU NGƯỜI DÙNG ====" << std::endl;
            std::cout << "1. Xem thông tin cá nhân" << std::endl;
            std::cout << "2. Cập nhật thông tin (OTP xác thực)" << std::endl;
            std::cout << "3. Đổi mật khẩu" << std::endl;
            std::cout << "4. Xem số dư ví" << std::endl;
            std::cout << "5. Chuyển điểm" << std::endl;
            std::cout << "0. Thoát" << std::endl;
            std::cout << "Chọn: ";
            std::cin >> choice;

            switch (choice) {
                case 1:
                    userController.displayUser(user);
                    break;
                case 2: {
                    std::string newEmail;
                    std::cout << "Email mới: "; std::getline(std::cin, newEmail);
                    userController.updatePersonalInfo(user,  newEmail);
                    break;
                }
                case 3: {
                    userController.changePassword(user);
                    break;
                }
                case 4:
                    std::cout << "Số dư ví: " << walletController.getWalletBalance(user.username1()) << " điểm\n";
                    break;
                case 5: {
                    std::string toUser;
                    int amount;
                    std::cout << "Tài khoản nhận: ";
                    std::cin >> toUser;
                    std::cout << "Số điểm chuyển: ";
                    std::cin >> amount;
                    if (toUser == user.username1()) {
                        std::cout << "Ban khong the chuyen tien cho chinh minh";
                        break;
                    }
                    UserAccount* receiver = userController.getUserByUsername(toUser);
                    if (receiver)
                        walletController.transferPoints(user, *receiver, amount);
                    else
                        std::cout << "Người nhận không tồn tại.\n";
                    break;
                }
                case 0:
                    std::cout << "Đăng xuất.\n";
                    break;
                default:
                    std::cout << "Lựa chọn không hợp lệ.\n";
            }
        } while (choice != 0);
    }
};

#endif //COMMONPAGE_H
