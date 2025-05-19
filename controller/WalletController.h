//
// Created by Dell on 5/11/2025.
//

#ifndef WALLETCONTROLLER_H
#define WALLETCONTROLLER_H
#include <iostream>
#include <map>

#include "../model/OTPManager.h"
#include "../model/UserAccount.h"
#include "../model/DatabaseManager.h"

class WalletController
{
private:
    DatabaseManager &db_manager;

public:
    WalletController(DatabaseManager &db) : db_manager(db) {}

    bool transferPoints(UserAccount &fromUser, UserAccount &toUser, int amount)
    {
        if (amount <= 0)
        {
            std::cout << "So diem chuyen phai lon hon 0.\n";
            return false;
        }
        if (db_manager.get_wallet_balance(fromUser.username1()) < amount)
        {
            std::cout << "Khong du diem trong vi.\n";
            return false;
        }

        std::string otp = OTPManager::generateOTP();
        OTPManager::sendOTP(otp, fromUser.username1());

        std::cout << "Nhap ma OTP de xac nhan chuyen diem: ";
        std::string userInput;
        std::cin >> userInput;

        if (!OTPManager::validateOTP(userInput, otp))
        {
            std::cout << "Xac thuc OTP that bai. Giao dich bi huy.\n";
            return false;
        }

        if (db_manager.transfer_points(fromUser.username1(), toUser.username1(), amount))
        {
            std::cout << "Chuyen thanh cong " << amount << " diem tu " << fromUser.username1()
                      << " den " << toUser.username1() << ".\n";
            return true;
        }
        return false;
    }

    void setWalletBalance(const std::string &username, int balance)
    {
        db_manager.update_wallet_balance(username, balance);
    }

    int getWalletBalance(const std::string &username)
    {
        return db_manager.get_wallet_balance(username);
    }
};

#endif // WALLETCONTROLLER_H
