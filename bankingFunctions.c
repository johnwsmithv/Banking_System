#include "bankingFunctions.h"

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
    // printf("We made it in the callback function!\n");
    // printf("Username Chars %c, %c, %c, %c, %c\n", data[0][0], data[0][1], data[0][2], data[0][3], data[0][4]);
    globalBankAccount->username = (char *)malloc((strlen(data[0]) + 1) * sizeof(char));
    globalBankAccount->password = (char *)malloc((strlen(data[1]) + 1) * sizeof(char));
    strcpy(globalBankAccount->username, data[0]);
    strcpy(globalBankAccount->password, data[1]);
    // globalBankAccount->username = data[0];
    // globalBankAccount->password = data[1];
    char * end;
    globalBankAccount->savingsAccountBalance = strtod(data[2], &end);
    globalBankAccount->checkingAccountBalance = strtod(data[3], &end);
    globalBankAccount->interest = strtof(data[4], &end);      
    // printf("Callback: Username: %s | Password: (%s)| Savings: %f | Checking: %f | Interest: %f\n", globalBankAccount->username, globalBankAccount->password, globalBankAccount->savingsAccountBalance, globalBankAccount->checkingAccountBalance, globalBankAccount->interest);            

    return 0;
}

/**
 * @brief This functions searches for the username that the User types on the terminal; it will return an error if the username is not found
 * 
 * @param username The username that the user enters into the terminal
 * @param dataBase The database pointer so we can do the sqlite3 queries
 * @return void* 
 */
void *accountInformationSql(const char * username, sqlite3 * dataBase) {

    char * query = sqlite3_mprintf("SELECT * FROM USER_INFO where USERNAME = '%s'", username);
    char * getAccountInfoErr = 0;
    sqlite3_exec(dataBase, query, sqlite3Callback, NULL, &getAccountInfoErr);
    if(getAccountInfoErr != NULL) {
        printf("Your account information was not found.\n");
        globalBankAccount->username = 0;
    }
    // printf("Account Information: Username: %s | Password: (%s)| Savings: %f | Checking: %f | Interest: %f\n", globalBankAccount->username, globalBankAccount->password, globalBankAccount->savingsAccountBalance, globalBankAccount->checkingAccountBalance, globalBankAccount->interest);

    return 0;
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
void * updateAccountInformationSql(const char * username, const char * columnName, const int value, sqlite3 * dataBase) {
    const char * query = sqlite3_mprintf("UPDATE USER_INFO SET %s = '%d' WHERE USERNAME = '%s'", columnName, value, username);
    sqlite3_exec(dataBase, query, NULL, NULL, NULL);
    return 0;
}

/**
 * @brief This is going to update the global bank account
 * 
 * @param updateSavings If this is 0 then we are going to update the savings account; if it is 1 then we are going to update the checking account
 * @param value The value that the user has entered to either deposit (+) or withdraw (-)
 * @return void* 
 */
void * updateBankAccount(const bool updateSavings, const int value, sqlite3 * dataBase) {
    if(updateSavings == 0){
        if(globalBankAccount->savingsAccountBalance + value < 0){
            printf("You cannot withdraw more than you have in your savings account.\n");
        } else {
            globalBankAccount->savingsAccountBalance += value;
            printf("Your new savings account balance is $%.2f.\n", globalBankAccount->savingsAccountBalance);
            updateAccountInformationSql(globalBankAccount->username, "SAVINGS", globalBankAccount->savingsAccountBalance, dataBase);
        }
    } else if (updateSavings == 1) {
        if(globalBankAccount->checkingAccountBalance + value < 0){
            printf("You cannot withdraw more than you have in your checking account.\n");
        } else {
            globalBankAccount->checkingAccountBalance += value;
            printf("Your new checking account balance is $%.2f.\n", globalBankAccount->checkingAccountBalance);
            updateAccountInformationSql(globalBankAccount->username, "CHECKING", globalBankAccount->checkingAccountBalance, dataBase);
        }
    }

    return 0;
}

bool createAccount(sqlite3 * dataBase) {
    // Buffers for the user to type in the their username and password
    char usernameInput[20];
    char passwordInput[20];

    bool userNameExists = false;
    bool loggedIn = false;

    while(!userNameExists){
        printf(BOLD(CYN("Username:\n")));
        // The user is going to enter a username on the terminal, and it will be attached to this buffer
        scanf("%20s", usernameInput);
        accountInformationSql(usernameInput, dataBase);
        if(globalBankAccount->username == 0){
            // Time to add stuff to the CSV
            globalBankAccount->username = (char *)malloc((strlen(usernameInput) + 1) * sizeof(char));
            globalBankAccount->username = usernameInput;
            // Since the account does not exist in the DB, we are prompting the user to enter a password
            printf(BOLD(CYN("Password:\n")));
            scanf("%20s", passwordInput);
            globalBankAccount->password = (char *)malloc((strlen(passwordInput) + 1) * sizeof(char));
            globalBankAccount->password = passwordInput;
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

bool loginToAccount(sqlite3 * dataBase) {
    // Buffers for the user to type in the their username and password
    char usernameInput[20];
    char passwordInput[20];

    bool userNameExists = false;
    bool loggedIn = false;
    bool passwordInputCorrect = false;

    while(!userNameExists){
        printf(BOLD(CYN("Login:\nUsername:\n")));
        scanf("%s", usernameInput);
        // We want to Query the SQL DB for this username
        accountInformationSql(usernameInput, dataBase);
        //printf("Account Values: %s, %s, %f, %f, %f\n", globalBankAccount->username, globalBankAccount->password, globalBankAccount->savingsAccountBalance, globalBankAccount->checkingAccountBalance, globalBankAccount->interest);
        if(globalBankAccount->username != NULL){
            // Time to add stuff to the CSV
            userNameExists = true;
            int passwordAttempts = 0;
            while(!passwordInputCorrect){
                printf(BOLD(CYN("Password:\n")));
                scanf("%s", passwordInput);
                if(!strcmp(passwordInput, globalBankAccount->password)){
                    // Print out a few different options
                    printf(BLU("You are now logged in.\n"));
                    passwordInputCorrect = true;
                    loggedIn = true;
                } else {
                    passwordAttempts++;
                    if(passwordAttempts == 3){
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
 * 
 */
void bankingSql(void) {
    sqlite3 * dataBase = NULL;
    sqlite3_open("userInfo.db", &dataBase);
    globalBankAccount = (BankAccount *)malloc(sizeof(BankAccount));
    int userOptions;

    // Different booleans used throughout the code
    bool userInputIncorrect = false;
    bool loggedIn = false;
    bool amountPositive = false;

    double amount;
    
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
        }else {
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
    int loggedInInput;
    
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
            updateBankAccount(0, amount, dataBase);
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
            updateBankAccount(1, amount, dataBase);
            break;
        case 5:
            printf("How much do you want to withdraw?\n");
            scanf("%lf", &amount);
            if(amount > 0){
                amount *= -1;
            } 
            updateBankAccount(0, amount, dataBase);
            break;
        case 6:
            printf("How much do you want to withdraw?\n");
            scanf("%lf", &amount);
            if(amount > 0){
                amount *= -1;
            }
            updateBankAccount(1, amount, dataBase);
            break;        
        case 7:
            printf("You are now logged out.");
            loggedIn = false;
            break;
        default:
            printf("That was not a valid option.\n");
            loggedInInput = -1;
            break;
        }
    }
    free(globalBankAccount->username);
    free(globalBankAccount->password);
    free(globalBankAccount);
    exit(0);
}