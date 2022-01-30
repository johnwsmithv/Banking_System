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

// Creating a FILE object to open the userInfo.txt
FILE * userInfoFile;
FILE * tempFile;

static BankAccount * globalBankAccount;

char * tempUsername;

void banking(void);
BankAccount * updateCSV(int updateSavingsChecking, int depositWithdrawValue, BankAccount * tempAccount);
BankAccount * accountInformation(char * username);

static int sqlite3Callback(void * unused, int count, char **data, char **columns);
void *accountInformationSql(char * username, sqlite3 * dataBase);
void * updateAccountInformationSql(char * username, char * column, int value, sqlite3 * dataBase);
void * updateBankAccount(bool updateSavings, int value, sqlite3 * dataBase);
void bankingSql(void);