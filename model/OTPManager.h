#ifndef OTPMANAGER_H
#define OTPMANAGER_H

#include <string>

class OTPManager
{
public:
    static std::string generateOTP();
    static void sendOTP(const std::string &otp, const std::string &receiver);
    static bool validateOTP(const std::string &userInput, const std::string &otpGenerated);
};

#endif // OTPMANAGER_H
