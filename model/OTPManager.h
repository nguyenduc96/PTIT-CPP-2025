//
// Created by Dell on 5/10/2025.
//

#ifndef OTPMANAGER_H
#define OTPMANAGER_H
#include <string>

#pragma once
#include <chrono>
#include <iostream>
#include <random>

class OTPManager
{
public:
    static std::string generateOTP()
    {
        std::mt19937 rng(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
        std::uniform_int_distribution<int> dist(100000, 999999);
        return std::to_string(dist(rng));
    }

    static void sendOTP(const std::string &otp, const std::string &receiver)
    {
        std::cout << "Ma OTP (gui den " << receiver << "): " << otp << std::endl;
    }

    static bool validateOTP(const std::string &userInput, const std::string &otpGenerated)
    {
        return userInput == otpGenerated;
    }
};

#endif
