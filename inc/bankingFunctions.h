#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sqlite3.h>

#include "Colors.h"
//https://stackoverflow.com/questions/5134891/how-do-i-use-valgrind-to-find-memory-leaks

// This header file contains all of the functions in banking.c
struct BankAccount {
    char * username;
    char * password;
    double checkingAccountBalance;
    double savingsAccountBalance;
    float interest;
    int rowNumber;
} typedef BankAccount;

enum BankCharacteristics {
    SAVINGS,
    CHECKING,
    INTEREST
};

void accountInformationSql(const char * username, sqlite3 * dataBase);
void * updateAccountInformationSql(const char * username, const char * columnName, const double value, sqlite3 * dataBase);
void * updateBankAccount(enum BankCharacteristics characteristics, const double value, sqlite3 * dataBase);
void bankingSql(void);