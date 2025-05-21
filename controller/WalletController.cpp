#include "WalletController.h"
#include "../model/OTPManager.h"

WalletController::WalletController(DatabaseManager &db) : db_manager(db) {}

bool WalletController::verifyOtp(std::string otp)
{
    int MAX = 3;
    bool check = false;
    do
    {
        std::cout << "Nhap ma OTP de xac nhan chuyen diem: ";
        std::string userInput;
        std::getline(std::cin, userInput);

        if (!OTPManager::validateOTP(userInput, otp))
        {
            MAX--;
            std::cout << "Ma OTP khong dung. Vui long nhap lai. Ban con " << MAX << " lan!\n";
        }
        else
        {
            check = true;
        }
    } while (MAX > 0 && !check);
    return check;
}

bool WalletController::transferPoints(UserAccount &fromUser, UserAccount &toUser, int amount)
{
    if (amount <= 0)
    {
        std::cout << "So diem chuyen phai lon hon 0.\n";
        return false;
    }

    if (fromUser.username1() == toUser.username1())
    {
        std::cout << "Khong the chuyen diem cho chinh minh.\n";
        return false;
    }

    if (db_manager.get_wallet_balance(fromUser.username1()) < amount)
    {
        std::cout << "Khong du diem trong vi.\n";
        return false;
    }

    std::string otp = OTPManager::generateOTP();
    OTPManager::sendOTP(otp, fromUser.username1());

    if (!verifyOtp(otp))
        return false;

    if (db_manager.transfer_points(fromUser.username1(), toUser.username1(), amount))
    {
        std::cout << "Chuyen thanh cong " << amount << " diem tu " << fromUser.username1()
                  << " den " << toUser.username1() << ".\n";
        return true;
    }

    return false;
}

void WalletController::setWalletBalance(const std::string &username, int balance)
{
    db_manager.update_wallet_balance(username, balance);
}

int WalletController::getWalletBalance(const std::string &username)
{
    return db_manager.get_wallet_balance(username);
}
