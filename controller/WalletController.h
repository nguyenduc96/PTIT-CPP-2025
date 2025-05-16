//
// Created by Dell on 5/11/2025.
//

#ifndef WALLETCONTROLLER_H
#define WALLETCONTROLLER_H
#include <iostream>
#include <map>

#include "../model/OTPManager.h"
#include "../model/UserAccount.h"

class WalletController {
public:
    bool transferPoints(UserAccount &fromUser, UserAccount &toUser, int amount) {
        if (amount <= 0) {
            std::cout << "Số điểm chuyển phải lớn hơn 0.\n";
            return false;
        }
        if (wallets[fromUser.username1()] < amount) {
            std::cout << "Không đủ điểm trong ví.\n";
            return false;
        }

        std::string otp = OTPManager::generateOTP();
        OTPManager::sendOTP(otp, fromUser.username1());

        std::cout << "Nhập mã OTP để xác nhận chuyển điểm: ";
        std::string userInput;
        std::cin >> userInput;

        if (!OTPManager::validateOTP(userInput, otp)) {
            std::cout << "Xác thực OTP thất bại. Giao dịch bị hủy.\n";
            return false;
        }

        wallets[fromUser.username1()] -= amount;
        wallets[toUser.username1()] += amount;
        std::cout << "Chuyển thành công " << amount << " điểm từ " << fromUser.username1()
                << " đến " << toUser.username1() << ".\n";
        return true;
    }

    void setWalletBalance(const std::string &username, int balance) {
        wallets[username] = balance;
    }

    int getWalletBalance(const std::string &username) {
        return wallets[username];
    }

private:
    std::map<std::string, int> wallets;
};


#endif //WALLETCONTROLLER_H
