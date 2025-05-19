/**
 * @file Password.h
 * @brief Password hashing and verification utilities
 *
 * This file contains utility functions for password hashing
 * and verification using cryptographic hash functions.
 *
 */

#ifndef CRYPTO_PASSWORD_H
#define CRYPTO_PASSWORD_H

#include <string>
#include <random>
#include <chrono>
#include "MD5.h"

namespace crypto
{

    /**
     * @class Password
     * @brief Utilities for password hashing and verification
     */
    class Password
    {
    public:
        /**
         * @brief Hash a password using MD5
         * @param password Plain text password
         * @return Hashed password string
         */
        static std::string hash(const std::string &password)
        {
            return MD5::hash(password);
        }

        /**
         * @brief Verify if a plain text password matches a hashed password
         * @param plainPassword Plain text password to verify
         * @param hashedPassword Stored hashed password
         * @return True if the passwords match, false otherwise
         */
        static bool verify(const std::string &plainPassword, const std::string &hashedPassword)
        {
            return hash(plainPassword) == hashedPassword;
        }

        /**
         * @brief Generate a random password
         * @param length Length of the password to generate (default: 10)
         * @return Randomly generated password
         */
        static std::string generateRandom(size_t length = 10)
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
    };

} // namespace crypto

#endif // CRYPTO_PASSWORD_H