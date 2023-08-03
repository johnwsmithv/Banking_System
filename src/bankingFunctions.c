#include "bankingFunctions.h"

#ifndef PROJECT_DIR
    #define PROJECT_DIR ""
#endif

static BankAccount * globalBankAccount;

// Little Trim function from StackOverflow
void trim(char * s) {
    char * p = s;
    int l = strlen(p);

    while(isspace(p[l - 1])) p[--l] = 0;
    while(* p && isspace(* p)) ++p, --l;

    memmove(s, p, l + 1);
}

/**
 * @brief Helper function to check if a pointer has been set to NULL; if it is not, then free it
 * 
 * @param obj The object which we want freed!
 */
void destoryObj(void ** obj) {
    if(*obj != NULL) {
        free(*obj);
        *obj = NULL;
    }
}

/**
 * @brief The callback funciton that the sqlite3_exec function is going to use
 * to help out parsing out arguments
 * 
 * @param unused 
 * @param count The number of columns in the result set
 * @param data The Rows Data
 * @param columns The column names
 * @return int 
 */
static int sqlite3Callback(void * unused, int count, char **data, char **columns) {
    // Since the size of the table is fixed, we can simply add all of the data into the loop
    const int lenUsername = strlen(data[0]);
    globalBankAccount->username = (char *)malloc((lenUsername + 1) * sizeof(char));
    memset(globalBankAccount->username, '\0', lenUsername + 1);
    
    globalBankAccount->password = (char *)malloc((strlen(data[1]) + 1) * sizeof(char));
    memset(globalBankAccount->username, '\0', lenUsername + 1);

    strncpy(globalBankAccount->username, data[0], lenUsername);
    strncpy(globalBankAccount->password, data[1], strlen(data[1]));

    char * end;
    globalBankAccount->savingsAccountBalance = strtod(data[2], &end);
    globalBankAccount->checkingAccountBalance = strtod(data[3], &end);
    globalBankAccount->interest = strtof(data[4], &end);      

    return 0;
}

/**
 * @brief This functions searches for the username that the User types on the terminal; it will return an error if the username is not found
 * 
 * @param username The username that the user enters into the terminal
 * @param dataBase The database pointer so we can do the sqlite3 queries
 * @return void* 
 */
void accountInformationSql(const char * username, sqlite3 * dataBase) {

    char * query = sqlite3_mprintf("SELECT * FROM USER_INFO where USERNAME = '%s'", username);
    char * getAccountInfoErr = 0;
    sqlite3_exec(dataBase, query, sqlite3Callback, NULL, &getAccountInfoErr);
    // TODO: Maybe handle error?
} 

/**
 * @brief A function which simple updates the user info based on the infromation that the user entered
 * 
 * @param username The username of the user which is going to have their information updated
 * @param columnName The name of the column being updated; CHECKINGS, SAVINGS, etc
 * @param value The value that is going to be used to update the Column in the database
 * @param dataBase The database pointer
 * @return void* 
 */
void * updateAccountInformationSql(const char * username, const char * columnName, const double value, sqlite3 * dataBase) {
    const char * query = sqlite3_mprintf("UPDATE USER_INFO SET %s = '%lf' WHERE USERNAME = '%s'", columnName, value, username);
    char * errMsg;
    sqlite3_exec(dataBase, query, NULL, NULL, &errMsg);
    return 0;
}

/**
 * @brief This is going to update the global bank account
 * 
 * @param characteristics If this is 0 then we are going to update the savings account; if it is 1 then we are going to update the checking account
 * @param value The value that the user has entered to either deposit (+) or withdraw (-)
 * @return void* 
 */
void * updateBankAccount(enum BankCharacteristics characteristics,  const double value, sqlite3 * dataBase) {
    if(characteristics == SAVINGS){
        if(globalBankAccount->savingsAccountBalance + value < 0){
            printf("You cannot withdraw more than you have in your savings account.\n");
        } else {
            globalBankAccount->savingsAccountBalance += value;
            printf("Your new savings account balance is $%.2f.\n", globalBankAccount->savingsAccountBalance);
            updateAccountInformationSql(globalBankAccount->username, "SAVINGS_ACCOUNT_BALANCE", globalBankAccount->savingsAccountBalance, dataBase);
        }
    } else if (characteristics == CHECKING) {
        if(globalBankAccount->checkingAccountBalance + value < 0){
            printf("You cannot withdraw more than you have in your checking account.\n");
        } else {
            globalBankAccount->checkingAccountBalance += value;
            printf("Your new checking account balance is $%.2f.\n", globalBankAccount->checkingAccountBalance);
            updateAccountInformationSql(globalBankAccount->username, "CHECKING_ACCOUNT_BALANCE", globalBankAccount->checkingAccountBalance, dataBase);
        }
    }

    return 0;
}

/**
 * @brief This function goes through and creates a bank account for the user
 * 
 * @param dataBase The database pointer to be able to execute db commands
 * @return true 
 * @return false 
 */
bool createAccount(sqlite3 * dataBase) {
    // Buffers for the user to type in the their username and password
    char usernameInput[20];
    char passwordInput[20];

    bool userNameExists = false;
    bool loggedIn = false;

    while(!userNameExists){
        printf(BOLD(CYN("Username:\n")));
        // The user is going to enter a username on the terminal, and it will be attached to this buffer
        scanf("%19s", usernameInput);
        accountInformationSql(usernameInput, dataBase);
        if(globalBankAccount->username == NULL){
            // Time to add stuff to the CSV
            globalBankAccount->username = (char *)malloc((strlen(usernameInput)) * sizeof(char));
            strncpy(globalBankAccount->username, usernameInput, strlen(usernameInput));

            // Since the account does not exist in the DB, we are prompting the user to enter a password
            printf(BOLD(CYN("Password:\n")));
            scanf("%19s", passwordInput);
            globalBankAccount->password = (char *)malloc((strlen(passwordInput)) * sizeof(char));
            strncpy(globalBankAccount->password, passwordInput, strlen(passwordInput));

            globalBankAccount->savingsAccountBalance = 0.0;
            globalBankAccount->checkingAccountBalance = 0.0;
            // Interest is the amount applied to the savings account at the end of every month
            globalBankAccount->interest = 0.05 / 100.0;

            char * insertUserQuery = sqlite3_mprintf("INSERT INTO USER_INFO VALUES('%s', '%s', '%d', '%d', '%f')", globalBankAccount->username, globalBankAccount->password, globalBankAccount->savingsAccountBalance, globalBankAccount->checkingAccountBalance, globalBankAccount->interest);
            sqlite3_exec(dataBase, insertUserQuery, NULL, NULL, NULL);
            printf(BLU("Your account has been created.\n"));
            userNameExists = true;
            loggedIn = true;
        } else {
            printf("%s already exists. Try another username or login.\n", usernameInput);
        }
    }

    return loggedIn;
}

/**
 * @brief Attempts to log into an account the user enters.
 * 
 * @param dataBase The database pointer to be able to execute db commands
 * @return true 
 * @return false 
 */
bool loginToAccount(sqlite3 * dataBase) {
    // Buffers for the user to type in the their username and password
    char usernameInput[20];
    char passwordInput[20];

    bool userNameExists = false;
    bool loggedIn = false;
    bool passwordInputCorrect = false;

    while(!userNameExists){
        printf(BOLD(CYN("Login:\nUsername:\n")));
        scanf("%19s", usernameInput);
        // We want to Query the SQL DB for this username
        accountInformationSql(usernameInput, dataBase);
        if(globalBankAccount->username != NULL) {
            userNameExists = true;
            int passwordAttempts = 0;
            while(!passwordInputCorrect){
                printf(BOLD(CYN("Password:\n")));
                scanf("%19s", passwordInput);
                if(!strncmp(passwordInput, globalBankAccount->password, strlen(passwordInput))) {
                    // Print out a few different options
                    printf(BLU("You are now logged in.\n"));
                    passwordInputCorrect = true;
                    loggedIn = true;
                } else {
                    passwordAttempts++;
                    if(passwordAttempts == 3) {
                        printf(BOLD(RED("Your account is now locked for 15 minutes.\n")));
                        exit(1);
                    } else {
                        printf(RED("That was not the correct password. Try again.\n"));
                    }
                }
            }
        } else {
            printf(RED("This username does not exist.\n"));
        }
    }

    return loggedIn;
}

/**
 * @brief The function that is running this Bank Account Program
 */
void bankingSql(void) {
    sqlite3 * dataBase = NULL;

    char * fileName = "userInfo.db";

    const int lenProjDir  = strlen(PROJECT_DIR);
    const int fileNameLen = strlen(fileName);

    char pathAndFileName[lenProjDir + fileNameLen + 2];

    sprintf(pathAndFileName, "%s/%s", PROJECT_DIR, fileName);

    sqlite3_open(pathAndFileName, &dataBase);

    // We need to now create the table in the database, assuming it doesn't already exist.
    char * errMsg;
    const char * createTable = "CREATE TABLE IF NOT EXISTS USER_INFO("
                               "USERNAME TEXT PRIMARY KEY NOT NULL,"
                               "PASSWORD TEXT NOT NULL,"
                               "SAVINGS_ACCOUNT_BALANCE DOUBLE NOT NULL,"
                               "CHECKING_ACCOUNT_BALANCE DOUBLE NOT NULL,"
                               "INTEREST FLOAT NOT NULL);";
    sqlite3_exec(dataBase, createTable, NULL, NULL, &errMsg);

    globalBankAccount = (BankAccount *)malloc(sizeof(BankAccount));
    globalBankAccount->username = NULL;
    globalBankAccount->password = NULL;
    int userOptions = 0;

    // Different booleans used throughout the code
    bool userInputIncorrect = false;
    bool loggedIn = false;
    bool amountPositive = false;

    double amount = 0.0;
    
    // Want to start the app up with a greeting
    printf(BOLD(MAG("Welcome to the bank. How may I help you?")));
    // Print options
    printf("\nOptions:\n1: Create an Account\n2: Login to an Existing Account\n3. Exit\n");

    // Constantly check and see if the user is inputting a correct input
    while(!userInputIncorrect) {
        scanf("%d", &userOptions);
        if(userOptions == 1 || userOptions == 2){
            userInputIncorrect = true;
        } else if (userOptions == 3) {
            printf(YEL("Thank you for using the Bank App. Have a wonderful day!\n"));
            exit(1);
        } else {
            printf("That is not a correct input. Input 1 or 2.\n");
        }
    }

    // The user wants to create an account
    if(userOptions == 1){
        loggedIn = createAccount(dataBase);
    } else if(userOptions == 2){ // The user wants to LOGIN
        loggedIn = loginToAccount(dataBase);
    }

    // Now that the user is logged in, they need to be able to do some things
    int loggedInInput = 0;
    
    printf("1: View Savings Account Balance\n2: View Checking Account Balance\n3: Deposit Money into Savings Account\n4: Deposit Money into Checking Account\n5: Withdraw Money from Savings Account\n6: Withdraw Money from Checking Account\n7: Logout\n");
    while(loggedIn){
        
        scanf("%d", &loggedInInput);
        // printf("The input was %d\n", loggedInInput);
        switch (loggedInInput)
        {
        case 1:
            printf("Savings Account Balance: $%.2f\n", globalBankAccount->savingsAccountBalance);
            break;
        case 2:
            printf("Checking Account Balance: $%.2f\n", globalBankAccount->checkingAccountBalance);
            break;
        case 3:
            printf("How much do you want to deposit?\n");
            while(!amountPositive){
                scanf("%lf", &amount);
                if(amount <= 0){
                    printf("You cannot deposit an amount less than or equal to 0.\n");
                } else {
                    amountPositive = true;
                }
            }
            updateBankAccount(SAVINGS, amount, dataBase);
            break;
        case 4:
            printf("How much do you want to deposit?\n");
            while(!amountPositive){
                scanf("%lf", &amount);
                if(amount <= 0){
                    printf("You cannot deposit an amount less than or equal to 0.\n");
                } else {
                    amountPositive = true;
                }
            }
            updateBankAccount(CHECKING, amount, dataBase);
            break;
        case 5:
            printf("How much do you want to withdraw?\n");
            scanf("%lf", &amount);
            if(amount > 0){
                amount *= -1;
            } 
            updateBankAccount(SAVINGS, amount, dataBase);
            break;
        case 6:
            printf("How much do you want to withdraw?\n");
            scanf("%lf", &amount);
            if(amount > 0){
                amount *= -1;
            }
            updateBankAccount(CHECKING, amount, dataBase);
            break;        
        case 7:
            printf("You are now logged out.\n");
            loggedIn = false;
            break;
        default:
            printf("That was not a valid option.\n");
            loggedInInput = -1;
            break;
        }
    }

    destoryObj((void **)&globalBankAccount->username);
    destoryObj((void **)&globalBankAccount->password);
    destoryObj((void **)&globalBankAccount);

    sqlite3_close(dataBase);
    
    exit(EXIT_SUCCESS);
}