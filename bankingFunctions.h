#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
// #include <sqlite3.h>

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

char * tempUsername;

void banking(void);
BankAccount * updateCSV(int updateSavingsChecking, int depositWithdrawValue, BankAccount * tempAccount);
BankAccount * accountInformation(char * username);
