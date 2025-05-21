#ifndef COMMONPAGE_H
#define COMMONPAGE_H

#include "UserView.h"
#include "../model/UserAccount.h"
#include "../controller/UserController.h"
#include "../controller/WalletController.h"

class CommonPage
{
    UserController &userController;
    WalletController &walletController;
    UserView &userView;

public:
    CommonPage(UserController &uc, WalletController &wc, UserView &usv);

    void showWelcomeMenu();
    void showLoginMenu();
    void registerNewUser();

private:
    void adminMenu(UserAccount &admin);
    void normalUserMenu(UserAccount &user);
};

#endif // COMMONPAGE_H
