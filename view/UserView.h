//
// Created by Dell on 5/9/2025.
//

#ifndef USERVIEW_H
#define USERVIEW_H


#pragma once
#include <string>
#include "../model/UserAccount.h"

class UserView {
public:
    static void showCreateAccountForm(UserAccount& user);
    static void showMessage(const std::string& message);
    void showLoginForm(std::string& username, std::string& password);

};


#endif //USERVIEW_H
