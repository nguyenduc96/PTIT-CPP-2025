//
// Created by Dell on 5/9/2025.
//

#include "UserAccount.h"
#include <string>
#include <sstream>
#include <iostream>

void UserAccount::loadFromStream(std::stringstream& ss) {
    ss >> username >> password >> fullName >> email >> pointBalance
       >> isAdmin >> isActive >> forceChangePassword;
}
