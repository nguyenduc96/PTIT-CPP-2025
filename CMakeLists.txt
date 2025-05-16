cmake_minimum_required(VERSION 3.29)
project(Reward_Wallet)

set(CMAKE_CXX_STANDARD 20)

add_executable(Reward_Wallet main.cpp
        model/UserAccount.cpp
        model/UserAccount.h
        view/UserView.cpp
        view/UserView.h
        controller/UserController.cpp
        controller/UserController.h
        utils/Utils.cpp
        utils/Utils.h
        model/OTPManager.cpp
        model/OTPManager.h
        controller/WalletController.cpp
        controller/WalletController.h
        view/CommonPage.cpp
        view/CommonPage.h
        utils/FileUtils.cpp
        utils/FileUtils.h
)
