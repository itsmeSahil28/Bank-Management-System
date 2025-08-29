#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <limits>
#include <algorithm> 
#include <sstream>
#include <cmath> 
using namespace std;
class Transaction {
private:
    string date;
    string type;
    double amount;
    double balanceAfter;

public:
    Transaction(string t, double amt, double bal) {
        type = t;
        amount = amt;
        balanceAfter = bal;
        
        
        time_t now = time(0);
        tm* localTime = localtime(&now);
        char buffer[80];
        strftime(buffer, 80, "%d-%m-%Y %H:%M:%S", localTime);
        date = buffer;
    }
    Transaction(string d, string t, double amt, double bal) {
        date = d;
        type = t;
        amount = amt;
        balanceAfter = bal;
    }
    
    string getDate() const { return date; }
    string getType() const { return type; }
    double getAmount() const { return amount; }
    double getBalanceAfter() const { return balanceAfter; }
    
    string toString() const {
        ostringstream oss;
        oss << date << "," << type << "," << amount << "," << balanceAfter;
        return oss.str();
    }
};
class Account {
private:
    int accountNumber;
    string name;
    string accountType;
    double balance;
    string dateCreated;
    vector<Transaction> transactions;

public:
    Account(int accNum, string n, string type, double bal) {
        accountNumber = accNum;
        name = n;
        accountType = type;
        balance = bal;
        time_t now = time(0);
        tm* localTime = localtime(&now);
        char buffer[80];
        strftime(buffer, 80, "%d-%m-%Y", localTime);
        dateCreated = buffer;
        if (bal > 0) {
            addTransaction("Initial Deposit", bal, bal);
        }
    }
    int getAccountNumber() const { return accountNumber; }
    string getName() const { return name; }
    string getAccountType() const { return accountType; }
    double getBalance() const { return balance; }
    string getDateCreated() const { return dateCreated; }
    const vector<Transaction>& getTransactions() const { return transactions; }
    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            addTransaction("Deposit", amount, balance);
            cout << "\nAmount deposited successfully.\n";
        } else {
            cout << "\nInvalid amount. Please enter a positive value.\n";
        }
    }
    
    bool withdraw(double amount) {
        if (amount <= 0) {
            cout << "\nInvalid amount. Please enter a positive value.\n";
            return false;
        }
        
        if (amount > balance) {
            cout << "\nInsufficient balance.\n";
            return false;
        }
        
        balance -= amount;
        addTransaction("Withdrawal", -amount, balance);
        cout << "\nAmount withdrawn successfully.\n";
        return true;
    }
    
    void displayDetails() const {
        cout << "\n---- Account Details ----\n";
        cout << "Account Number: " << accountNumber << endl;
        cout << "Name: " << name << endl;
        cout << "Account Type: " << accountType << endl;
        cout << "Balance: Rs" << fixed << setprecision(2) << balance << endl;
        cout << "Date Created: " << dateCreated << endl;
        cout << "------------------------\n";
    }
    
    void displayTransactionHistory() const {
        if (transactions.empty()) {
            cout << "\nNo transactions found for this account.\n";
            return;
        }
        
        cout << "\n---- Transaction History for Account #" << accountNumber << " ----\n";
        cout << left << setw(25) << "Date & Time" 
             << setw(15) << "Type" 
             << setw(15) << "Amount" 
             << "Balance After" << endl;
        cout << string(80, '-') << endl;
        
        for (const auto& transaction : transactions) {
            string amountStr;
            if (transaction.getType() == "Withdrawal") {
                amountStr = "-Rs" + to_string(std::abs(transaction.getAmount()));
            } else {
                amountStr = "Rs" + to_string(transaction.getAmount());
            }
            
            // Format amount string to show only 2 decimal places
            size_t pos = amountStr.find('.');
            if (pos != string::npos && amountStr.length() > pos + 3) {
                amountStr = amountStr.substr(0, pos + 3);
            }
            
            cout << left << setw(25) << transaction.getDate()
                 << setw(15) << transaction.getType()
                 << setw(15) << amountStr
                 << "Rs" << fixed << setprecision(2) << transaction.getBalanceAfter() << endl;
        }
    }
    
    void setDateCreated(const string& date) {
        dateCreated = date;
    }
    
    void addTransaction(string type, double amount, double balanceAfter) {
        transactions.push_back(Transaction(type, amount, balanceAfter));
    }
    
    void addStoredTransaction(string date, string type, double amount, double balanceAfter) {
        transactions.push_back(Transaction(date, type, amount, balanceAfter));
    }
    
    string getTransactionsAsString() const {
        string result = "";
        for (const auto& transaction : transactions) {
            result += transaction.toString() + "|";
        }
        // Remove the last '|' if there are transactions
        if (!result.empty()) {
            result.pop_back();
        }
        return result;
    }
};
class BankManagementSystem {
private:
    vector<Account> accounts;
    int nextAccountNumber;
    const string FILENAME = "bank_data.txt";
    const string TRANSACTIONS_FILENAME = "transactions_data.txt";
    void saveToFile() {
        ofstream file(FILENAME);
        if (!file) {
            cout << "\nError opening file for writing!\n";
            return;
        }
        
        for (const auto& account : accounts) {
            file << account.getAccountNumber() << ","
                 << account.getName() << ","
                 << account.getAccountType() << ","
                 << account.getBalance() << ","
                 << account.getDateCreated() << endl;
        }
        
        file.close();
        ofstream transFile(TRANSACTIONS_FILENAME);
        if (!transFile) {
            cout << "\nError opening transactions file for writing!\n";
            return;
        }
        
        for (const auto& account : accounts) {
            string transactions = account.getTransactionsAsString();
            if (!transactions.empty()) {
                transFile << account.getAccountNumber() << ":" << transactions << endl;
            }
        }
        
        transFile.close();
    }
    void loadFromFile() {
        ifstream file(FILENAME);
        if (!file) {
            cout << "\nNo existing data file found. Starting with empty database.\n";
            nextAccountNumber = 10001; 
        }
        accounts.clear();
        string line;
        int maxAccNum = 1000000;
        
        while (getline(file, line)) {
            size_t pos = 0;
            vector<string> tokens;
            
            while ((pos = line.find(",")) != string::npos) {
                tokens.push_back(line.substr(0, pos));
                line.erase(0, pos + 1);
            }
            tokens.push_back(line);
            if (tokens.size() == 5) {
                int accNum = stoi(tokens[0]);
                double balance = stod(tokens[3]);
                
                Account acc(accNum, tokens[1], tokens[2], 0); 
                acc.setDateCreated(tokens[4]);
                if (balance > 0) {
                    acc.deposit(balance);
                }
                
                accounts.push_back(acc);
                
                if (accNum > maxAccNum) {
                    maxAccNum = accNum;
                }
            }
        }
        
        nextAccountNumber = maxAccNum + 1;
        file.close();
        ifstream transFile(TRANSACTIONS_FILENAME);
        if (!transFile) {
            cout << "\nNo existing transaction file found.\n";
            return;
        }
        
        while (getline(transFile, line)) {
            size_t colonPos = line.find(":");
            if (colonPos != string::npos) {
                int accNum = stoi(line.substr(0, colonPos));
                string transData = line.substr(colonPos + 1);
                
                Account* account = findAccount(accNum);
                if (account != nullptr) {
                    size_t pos = 0;
                    string token;
                    
                    while ((pos = transData.find("|")) != string::npos) {
                        token = transData.substr(0, pos);
                        transData.erase(0, pos + 1);
                        vector<string> transTokens;
                        size_t commaPos = 0;
                        string transToken;
                        string tempToken = token;
                        
                        while ((commaPos = tempToken.find(",")) != string::npos) {
                            transTokens.push_back(tempToken.substr(0, commaPos));
                            tempToken.erase(0, commaPos + 1);
                        }
                        transTokens.push_back(tempToken);
                        
                        if (transTokens.size() == 4) {
                            account->addStoredTransaction(
                                transTokens[0],           
                                transTokens[1],           
                                stod(transTokens[2]),     
                                stod(transTokens[3])      
                            );
                        }
                    }
                    if (!transData.empty()) {
                        vector<string> transTokens;
                        size_t commaPos = 0;
                        string tempToken = transData;
                        
                        while ((commaPos = tempToken.find(",")) != string::npos) {
                            transTokens.push_back(tempToken.substr(0, commaPos));
                            tempToken.erase(0, commaPos + 1);
                        }
                        transTokens.push_back(tempToken);
                        
                        if (transTokens.size() == 4) {
                            account->addStoredTransaction(
                                transTokens[0],           
                                transTokens[1],           
                                stod(transTokens[2]),    
                                stod(transTokens[3])      
                            );
                        }
                    }
                }
            }
        }
        
        transFile.close();
    }
    
public:
    BankManagementSystem() {
        loadFromFile();
    }
    
    ~BankManagementSystem() {
        saveToFile();
    }
    
    void createAccount() {
        string name, accountType;
        double initialDeposit;
        
        cout << "\n---- Create New Account ----\n";
        cout << "Enter Full Name: ";
        cin.ignore();
        getline(cin, name);  
        cout << "Select Account Type (Savings/Current): ";
        getline(cin, accountType);
        for (auto& c : accountType) {
            c = tolower(c);
        }
        
        while (accountType != "savings" && accountType != "current") {
            cout << "Invalid account type. Please enter 'Savings' or 'Current': ";
            getline(cin, accountType);
            for (auto& c : accountType) {
                c = tolower(c);
            }
        }
        accountType[0] = toupper(accountType[0]);
        
        cout << "Enter Initial Deposit Amount: $";
        while (!(cin >> initialDeposit) || initialDeposit < 0) {
            cout << "Invalid amount. Please enter a positive value: $";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        Account newAccount(nextAccountNumber, name, accountType, initialDeposit);
        accounts.push_back(newAccount);
        
        cout << "\nAccount created successfully!\n";
        cout << "Your Account Number is: " << nextAccountNumber << endl;
        
        nextAccountNumber++;
        saveToFile();
    }
    
    Account* findAccount(int accountNumber) {
        for (auto& account : accounts) {
            if (account.getAccountNumber() == accountNumber) {
                return &account;
            }
        }
        return nullptr;
    }
    
    void depositMoney() {
        int accountNumber;
        double amount;
        
        cout << "\n---- Deposit Money ----\n";
        cout << "Enter Account Number: ";
        while (!(cin >> accountNumber)) {
            cout << "Invalid input. Please enter a valid account number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        Account* account = findAccount(accountNumber);
        if (account == nullptr) {
            cout << "\nAccount not found!\n";
            return;
        }
        
        cout << "Enter Deposit Amount: $";
        while (!(cin >> amount) || amount <= 0) {
            cout << "Invalid amount. Please enter a positive value: $";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        account->deposit(amount);
        saveToFile();
    }
    
    void withdrawMoney() {
        int accountNumber;
        double amount;
        
        cout << "\n---- Withdraw Money ----\n";
        cout << "Enter Account Number: ";
        while (!(cin >> accountNumber)) {
            cout << "Invalid input. Please enter a valid account number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        Account* account = findAccount(accountNumber);
        if (account == nullptr) {
            cout << "\nAccount not found!\n";
            return;
        }
        
        cout << "Enter Withdrawal Amount: Rs";
        while (!(cin >> amount) || amount <= 0) {
            cout << "Invalid amount. Please enter a positive value: $";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        if (account->withdraw(amount)) {
            saveToFile();
        }
    }
    
    void checkBalance() {
        int accountNumber;
        
        cout << "\n---- Check Balance ----\n";
        cout << "Enter Account Number: ";
        while (!(cin >> accountNumber)) {
            cout << "Invalid input. Please enter a valid account number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        Account* account = findAccount(accountNumber);
        if (account == nullptr) {
            cout << "\nAccount not found!\n";
            return;
        }
        
        cout << "\nCurrent Balance: Rs" << fixed << setprecision(2) << account->getBalance() << endl;
    }
    
    void displayAccountDetails() {
        int accountNumber;
        
        cout << "\n---- Account Details ----\n";
        cout << "Enter Account Number: ";
        while (!(cin >> accountNumber)) {
            cout << "Invalid input. Please enter a valid account number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        Account* account = findAccount(accountNumber);
        if (account == nullptr) {
            cout << "\nAccount not found!\n";
            return;
        }
        
        account->displayDetails();
    }
    
    void displayAllAccounts() {
        if (accounts.empty()) {
            cout << "\nNo accounts found in the system.\n";
            return;
        }
        
        cout << "\n---- All Accounts ----\n";
        cout << left << setw(15) << "Acc. Number" 
             << setw(25) << "Name" 
             << setw(15) << "Type" 
             << setw(15) << "Balance" 
             << "Date Created" << endl;
        cout << string(80, '-') << endl;
        
        for (const auto& account : accounts) {
            cout << left << setw(15) << account.getAccountNumber()
                 << setw(25) << account.getName()
                 << setw(15) << account.getAccountType()
                 << "$" << setw(14) << fixed << setprecision(2) << account.getBalance()
                 << account.getDateCreated() << endl;
        }
    }
    
    void searchAccountsByName() {
        string searchName;
        
        cout << "\n---- Search Accounts by Name ----\n";
        cout << "Enter Name to Search: ";
        cin.ignore();
        getline(cin, searchName);
        
        // Convert search name to lowercase for case-insensitive search
        string searchNameLower = searchName;
        transform(searchNameLower.begin(), searchNameLower.end(), searchNameLower.begin(), ::tolower);
        
        vector<Account*> foundAccounts;
        
        for (auto& account : accounts) {
            string accountNameLower = account.getName();
            transform(accountNameLower.begin(), accountNameLower.end(), accountNameLower.begin(), ::tolower);
            
            // If the search term is found in the account name
            if (accountNameLower.find(searchNameLower) != string::npos) {
                foundAccounts.push_back(&account);
            }
        }
        
        if (foundAccounts.empty()) {
            cout << "\nNo accounts found with name '" << searchName << "'.\n";
            return;
        }
        
        cout << "\n---- Found " << foundAccounts.size() << " Account(s) ----\n";
        cout << left << setw(15) << "Acc. Number" 
             << setw(25) << "Name" 
             << setw(15) << "Type" 
             << setw(15) << "Balance" 
             << "Date Created" << endl;
        cout << string(80, '-') << endl;
        
        for (const auto& account : foundAccounts) {
            cout << left << setw(15) << account->getAccountNumber()
                 << setw(25) << account->getName()
                 << setw(15) << account->getAccountType()
                 << "$" << setw(14) << fixed << setprecision(2) << account->getBalance()
                 << account->getDateCreated() << endl;
        }
    }
    
    void viewTransactionHistory() {
        int accountNumber;
        
        cout << "\n---- Transaction History ----\n";
        cout << "Enter Account Number: ";
        while (!(cin >> accountNumber)) {
            cout << "Invalid input. Please enter a valid account number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        Account* account = findAccount(accountNumber);
        if (account == nullptr) {
            cout << "\nAccount not found!\n";
            return;
        }
        
        account->displayTransactionHistory();
    }
};

// Main function
int main() {
    BankManagementSystem bankSystem;
    int choice;
    
    while (true) {
        // Display menu
        cout << "\n==== BANK MANAGEMENT SYSTEM ====\n";
        cout << "1. Create New Account\n";
        cout << "2. Deposit Money\n";
        cout << "3. Withdraw Money\n";
        cout << "4. Check Balance\n";
        cout << "5. Display Account Details\n";
        cout << "6. Display All Accounts\n";
        cout << "7. Search Accounts by Name\n";
        cout << "8. View Transaction History\n";
        cout << "9. Exit\n";
        cout << "Enter your choice (1-9): ";
        
        // Input validation
        while (!(cin >> choice) || choice < 1 || choice > 9) {
            cout << "Invalid choice. Please enter a number between 1 and 9: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
        switch (choice) {
            case 1:
                bankSystem.createAccount();
                break;
            case 2:
                bankSystem.depositMoney();
                break;
            case 3:
                bankSystem.withdrawMoney();
                break;
            case 4:
                bankSystem.checkBalance();
                break;
            case 5:
                bankSystem.displayAccountDetails();
                break;
            case 6:
                bankSystem.displayAllAccounts();
                break;
            case 7:
                bankSystem.searchAccountsByName();
                break;
            case 8:
                bankSystem.viewTransactionHistory();
                break;
            case 9:
                cout << "\nThank you for using the Bank Management System!\n";
                return 0;
        }
    }
    
    return 0;
} 