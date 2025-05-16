//
// Created by Dell on 5/9/2025.
//

#ifndef UTILS_H
#define UTILS_H

#include <string>

std::string sha256(const std::string& input);
void backupFile(const std::string& sourcePath, const std::string& backupPath);
std::string generateRandomPassword(int length = 10);


#endif //UTILS_H
