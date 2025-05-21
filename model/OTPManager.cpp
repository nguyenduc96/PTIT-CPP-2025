#include "OTPManager.h"
#include <iostream>
#include <random>
#include <chrono>

std::string OTPManager::generateOTP()
{
    std::mt19937 rng(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<int> dist(100000, 999999);
    return std::to_string(dist(rng));
}

void OTPManager::sendOTP(const std::string &otp, const std::string &receiver)
{
    std::cout << "Gia lap gui OTP.\nMa OTP (gui den " << receiver << "): " << otp << std::endl;
}

bool OTPManager::validateOTP(const std::string &userInput, const std::string &otpGenerated)
{
    return userInput == otpGenerated;
}
