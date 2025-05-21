//
// Created by Dell on 5/21/2025.
//

#include "TransactionHistory.h"
#include <sstream>

TransactionHistory::TransactionHistory() {
}

TransactionHistory::~TransactionHistory() {
}

string TransactionHistory::toString() {
    std::stringstream ss;
    ss << "Nguoi gui: " << fromUser << " Nguoi nhan: " << toUser << " So tien: " << amount << " Thoi gian: " << transactionTime << "\n";
    return ss.str();
}
