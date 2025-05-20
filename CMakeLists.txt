cmake_minimum_required(VERSION 3.29)
project(Reward_Wallet)

set(CMAKE_CXX_STANDARD 20)

# Add SQLite source files
add_library(sqlite3 STATIC
    sqlite/sqlite3.c
)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/sqlite)

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
        model/DatabaseManager.cpp
        model/DatabaseManager.h
)

# Link SQLite3
target_link_libraries(Reward_Wallet PRIVATE sqlite3)
