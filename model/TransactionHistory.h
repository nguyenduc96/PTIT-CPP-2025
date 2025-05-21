//
// Created by Dell on 5/21/2025.
//

#ifndef TRANSACTIONHISTORY_H
#define TRANSACTIONHISTORY_H
#include <string>
#include <ctime>

using namespace std;

class TransactionHistory {
private:
    string fromUser;
    string toUser;
    double amount;
    string transactionTime;

public:
    TransactionHistory();
    ~TransactionHistory();
    string toString();

    [[nodiscard]] string from_user() const {
        return fromUser;
    }

    void set_from_user(const string &from_user) {
        fromUser = from_user;
    }

    [[nodiscard]] string to_user() const {
        return toUser;
    }

    void set_to_user(const string &to_user) {
        toUser = to_user;
    }

    [[nodiscard]] double amount1() const {
        return amount;
    }

    void set_amount(double amount) {
        this->amount = amount;
    }

    [[nodiscard]] string transaction_time() const {
        return transactionTime;
    }

    void set_transaction_time(const string &transaction_time) {
        transactionTime = transaction_time;
    }
};

#endif //TRANSACTIONHISTORY_H
