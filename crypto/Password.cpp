/**
 * @file Password.cpp
 * @brief Implementation of password utilities
 *
 * This file contains the implementation of the Password class methods.
 *
 */

#include "Password.h"
#include <random>
#include <chrono>

namespace crypto
{

    std::string Password::hash(const std::string &password)
    {
        return MD5::hash(password);
    }

    bool Password::verify(const std::string &plainPassword, const std::string &hashedPassword)
    {
        return hash(plainPassword) == hashedPassword;
    }

    std::string Password::generateRandom(size_t length)
    {
        const std::string chars =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "!@#$%^&*()-_=+";

        // Seed the random number generator
        unsigned seed = static_cast<unsigned>(
            std::chrono::system_clock::now().time_since_epoch().count());
        std::mt19937 generator(seed);
        std::uniform_int_distribution<> distribution(0, static_cast<int>(chars.size() - 1));

        std::string password;
        password.reserve(length);

        for (size_t i = 0; i < length; ++i)
        {
            password += chars[distribution(generator)];
        }

        return password;
    }

} // namespace crypto