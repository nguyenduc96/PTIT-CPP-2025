//
// Created by Dell on 5/9/2025.
//

#include "Utils.h"
#include <sstream>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <random>


void backupFile(const std::string& sourcePath, const std::string& backupPath) {
    try {
        std::filesystem::copy_file(
            sourcePath,
            backupPath,
            std::filesystem::copy_options::overwrite_existing
        );
    } catch (std::exception& e) {
        std::cerr << "Loi khi sao luu tap tin: " << e.what() << "\n";
    }
}

std::string generateRandomPassword(int length) {
    const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<> dist(0, chars.size() - 1);

    std::string password;
    for (int i = 0; i < length; ++i) {
        password += chars[dist(engine)];
    }
    return password;
}
