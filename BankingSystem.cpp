#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <ctime>

using namespace std;

struct Transaction {
    string type;      // Deposit, Withdrawal, Transfer In, Transfer Out, Account Opened
    double amount;
    double balanceAfter;
    string time;
};

string currentTime() {
    time_t now = time(0);
    char buf[32];
    strftime(buf, sizeof(buf), "%d-%m-%Y %H:%M:%S", localtime(&now));
    return string(buf);
}

class Account {
    int accountNumber;
    string name;
    string pin;
    double balance;
    vector<Transaction> history;

    void record(const string& type, double amount) {
        Transaction t;
        t.type = type;
        t.amount = amount;
        t.balanceAfter = balance;
        t.time = currentTime();
        history.push_back(t);
    }

public:
    Account(int number, const string& n, const string& p, double initialDeposit)
        : accountNumber(number), name(n), pin(p), balance(initialDeposit) {
        record("Account Opened", initialDeposit);
    }

    int getNumber() const { return accountNumber; }
    string getName() const { return name; }
    double getBalance() const { return balance; }
    bool checkPin(const string& p) const { return pin == p; }

    void deposit(double amount) {
        balance += amount;
        record("Deposit", amount);
    }

    bool withdraw(double amount) {
        if (amount > balance) return false;
        balance -= amount;
        record("Withdrawal", amount);
        return true;
    }

    bool transferOut(double amount) {
        if (amount > balance) return false;
        balance -= amount;
        record("Transfer Out", amount);
        return true;
    }

    void transferIn(double amount) {
        balance += amount;
        record("Transfer In", amount);
    }

    void printHistory() const {
        cout << "\n=== Transaction History (Acc " << accountNumber << ") ===\n";
        cout << left << setw(22) << "Date/Time" << setw(16) << "Type"
             << setw(12) << "Amount" << "Balance\n";
        cout << string(62, '-') << "\n";
        for (size_t i = 0; i < history.size(); ++i) {
            const Transaction& t = history[i];
            cout << left << setw(22) << t.time << setw(16) << t.type
                 << setw(12) << fixed << setprecision(2) << t.amount
                 << t.balanceAfter << "\n";
        }
    }
};

class Bank {
    vector<Account> accounts;
    int nextNumber;

    Account* find(int number) {
        for (size_t i = 0; i < accounts.size(); ++i)
            if (accounts[i].getNumber() == number) return &accounts[i];
        return 0;
    }

public:
    Bank() : nextNumber(1001) {}

    void createAccount() {
        string name, pin;
        double initial;

        cout << "Enter your name: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, name);
        if (name.empty()) { cout << "Name cannot be empty.\n"; return; }

        cout << "Set a 4-digit PIN: ";
        cin >> pin;
        if (pin.length() != 4 || pin.find_first_not_of("0123456789") != string::npos) {
            cout << "PIN must be exactly 4 digits.\n";
            return;
        }

        cout << "Initial deposit: ";
        if (!(cin >> initial) || initial < 0) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid amount.\n";
            return;
        }

        accounts.push_back(Account(nextNumber, name, pin, initial));
        cout << "\nAccount created successfully!\n"
             << "Your account number is: " << nextNumber << "\n";
        nextNumber++;
    }

    Account* login() {
        int number;
        string pin;
        cout << "Account number: ";
        if (!(cin >> number)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input.\n";
            return 0;
        }
        cout << "PIN: ";
        cin >> pin;

        Account* acc = find(number);
        if (!acc || !acc->checkPin(pin)) {
            cout << "Invalid account number or PIN.\n";
            return 0;
        }
        cout << "\nWelcome, " << acc->getName() << "!\n";
        return acc;
    }

    Account* findAccount(int number) { return find(number); }
};

double readAmount(const string& prompt) {
    double x;
    cout << prompt;
    while (!(cin >> x) || x <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter a positive amount. " << prompt;
    }
    return x;
}

int readInt(const string& prompt) {
    int x;
    cout << prompt;
    while (!(cin >> x)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. " << prompt;
    }
    return x;
}

void accountMenu(Bank& bank, Account* acc) {
    int choice;
    do {
        cout << "\n----- ACCOUNT MENU -----\n"
             << "1. Check balance\n"
             << "2. Deposit\n"
             << "3. Withdraw\n"
             << "4. Transfer\n"
             << "5. Transaction history\n"
             << "0. Logout\n";
        choice = readInt("Choose: ");

        switch (choice) {
            case 1:
                cout << "Current balance: " << fixed << setprecision(2)
                     << acc->getBalance() << "\n";
                break;
            case 2: {
                double amt = readAmount("Deposit amount: ");
                acc->deposit(amt);
                cout << "Deposited " << fixed << setprecision(2) << amt
                     << ". New balance: " << acc->getBalance() << "\n";
                break;
            }
            case 3: {
                double amt = readAmount("Withdraw amount: ");
                if (acc->withdraw(amt))
                    cout << "Withdrawn " << fixed << setprecision(2) << amt
                         << ". New balance: " << acc->getBalance() << "\n";
                else
                    cout << "Insufficient balance.\n";
                break;
            }
            case 4: {
                int toNum = readInt("Recipient account number: ");
                Account* to = bank.findAccount(toNum);
                if (!to) { cout << "Recipient account not found.\n"; break; }
                if (to == acc) { cout << "Cannot transfer to your own account.\n"; break; }
                double amt = readAmount("Transfer amount: ");
                if (acc->transferOut(amt)) {
                    to->transferIn(amt);
                    cout << "Transferred " << fixed << setprecision(2) << amt
                         << " to " << to->getName() << ". New balance: "
                         << acc->getBalance() << "\n";
                } else {
                    cout << "Insufficient balance.\n";
                }
                break;
            }
            case 5: acc->printHistory(); break;
            case 0: cout << "Logged out.\n"; break;
            default: cout << "Invalid option.\n";
        }
    } while (choice != 0);
}

int main() {
    Bank bank;
    int choice;

    do {
        cout << "\n===== BANKING SYSTEM =====\n"
             << "1. Create account\n"
             << "2. Login\n"
             << "0. Exit\n";
        choice = readInt("Choose: ");

        switch (choice) {
            case 1: bank.createAccount(); break;
            case 2: {
                Account* acc = bank.login();
                if (acc) accountMenu(bank, acc);
                break;
            }
            case 0: cout << "Thank you for banking with us!\n"; break;
            default: cout << "Invalid option.\n";
        }
    } while (choice != 0);

    return 0;
}