#ifndef WALLETCONTROLLER_H
#define WALLETCONTROLLER_H

#include <iostream>
#include "UserController.h"
#include "../model/UserAccount.h"
#include "../model/DatabaseManager.h"

class WalletController
{
private:
    DatabaseManager &db_manager;

public:
    WalletController(DatabaseManager &db);

    bool verifyOtp(std::string otp);
    bool transferPoints(UserAccount &fromUser, UserAccount &toUser, int amount);
    void setWalletBalance(const std::string &username, int balance);
    int getWalletBalance(const std::string &username);
};

#endif // WALLETCONTROLLER_H