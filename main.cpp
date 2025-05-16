#include <iostream>
#include "Model/UserAccount.h"
#include "View/UserView.h"
#include "Controller/UserController.h"
#include "view/CommonPage.h"

int main() {
    UserView user_view;
    UserController user_controller;
    WalletController wallet_controller;
    user_controller.loadUsersFromFile();
    CommonPage common_page(user_controller, wallet_controller, user_view);
    common_page.showWelcomeMenu();
    return 0;
}
