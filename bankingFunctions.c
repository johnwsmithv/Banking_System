#include "bankingFunctions.h"

// Little Trim function from StackOverflow
void trim(char * s) {
    char * p = s;
    int l = strlen(p);

    while(isspace(p[l - 1])) p[--l] = 0;
    while(* p && isspace(* p)) ++p, --l;

    memmove(s, p, l + 1);
} 

// If the input is a 0 we are updating the Savings Account, if it is a 1 we are updating the checking account
BankAccount * updateCSV(int updateSavingsChecking, int depositWithdrawValue, BankAccount * tempAccount){
    bool balancePositive = true;
    if(updateSavingsChecking == 0){
        if(tempAccount->savingsAccountBalance + depositWithdrawValue < 0){
            printf("You cannot withdraw more than you have in your savings account.\n");
            balancePositive = false;
        } else {
            tempAccount->savingsAccountBalance += depositWithdrawValue;
            printf("Your new savings account balance is $%.2f.\n", tempAccount->savingsAccountBalance);
        }
    } else if (updateSavingsChecking == 1) {
        if(tempAccount->checkingAccountBalance + depositWithdrawValue < 0){
            printf("You cannot withdraw more than you have in your checking account.\n");
            balancePositive = false;
        } else {
            tempAccount->checkingAccountBalance += depositWithdrawValue;
            printf("Your new checking account balance is $%.2f.\n", tempAccount->checkingAccountBalance);
        }
    }

    if(balancePositive) {
        char buffer[150];
        int currentRow = 0;
        userInfoFile = fopen("userInfo.csv", "r");
        tempFile = fopen("temp.csv", "w");
        
        // We need to read the lines that aren't being updated into a temporary file and then put them back into the original
        while(fgets(buffer, sizeof(buffer), userInfoFile)){
            if(tempAccount->rowNumber == currentRow){
                fprintf(tempFile, "%s, %s, %f, %f, %f\n", tempAccount->username, tempAccount->password, tempAccount->savingsAccountBalance, tempAccount->checkingAccountBalance, tempAccount->interest);
            } else {
                fputs(buffer, tempFile);
            }
            currentRow += 1;
        }
        fclose(tempFile);
        fclose(userInfoFile);

        // Rewritting the information back into the original CSV
        userInfoFile = fopen("userInfo.csv", "w");
        tempFile = fopen("temp.csv", "r");
        while(fgets(buffer, sizeof(buffer), tempFile)){
            fputs(buffer, userInfoFile);
        }

        fclose(tempFile);
        fclose(userInfoFile);
    }
    
    return tempAccount;
}

BankAccount * accountInformation(char * username){
    BankAccount * tempAccount = (BankAccount *)malloc(sizeof(BankAccount));

    bool usernameExists = false;
    int row = 0;
    int col = 0;
    // Will probs need to be a lot bigger; could somehow find the size of the file and go that way
    char buffer[50];
    char * rowValue;

    userInfoFile = fopen("userInfo.csv", "r");
    // printf("Buffer: %s\n", buffer);
    // If the file is empty, the return value is NULL
    if(userInfoFile != NULL) {
        while(!usernameExists){
            fgets(buffer, sizeof(buffer), userInfoFile);
            rowValue = strtok(buffer, ", ");
            while(rowValue) {
                //printf("Row Value: %s\nStr Len: %d\n", rowValue, strlen(rowValue));
                if(col == 0) {
                    if(!strcmp(rowValue, username)){
                        usernameExists = true;
                        // strncpy(account->username, rowValue, sizeof(rowValue));
                        tempAccount->username = malloc(strlen(rowValue));
                        strcpy(tempAccount->username, rowValue);
                    }
                }
                
                // Grabbing all of the values and assigning them to the local struct
                if(col == 1 && usernameExists){
                    //strncpy(account->password, rowValue, sizeof(rowValue));
                    tempAccount->password = malloc(strlen(rowValue));
                    strcpy(tempAccount->password, rowValue);
                    //printf("Password: %s\n", rowValue);
                }
                // Casting the char * to a double
                if(col == 2 && usernameExists){
                    tempAccount->savingsAccountBalance = strtod(rowValue, NULL);
                    //printf("Savings: %f\n", tempAccount->savingsAccountBalance);
                }
                // Casting the char * to a double
                if(col == 3 && usernameExists){
                    tempAccount->checkingAccountBalance = strtod(rowValue, NULL);
                    //printf("checking: %f\n", tempAccount->checkingAccountBalance);
                }
                // Casting the char * to a float
                if(col == 4 && usernameExists){
                    tempAccount->interest = atof(rowValue);
                    //printf("interest: %f\n", tempAccount->interest);
                    // While in here we can add the row number which this information sits on
                    tempAccount->rowNumber = row;
                }
                rowValue = strtok(NULL, ", ");
                ++col;
            }
            col = 0;
            ++row;
        }

        // If we get to the end of the file and we have not found the username or their data, we want to fill in the data with NULL
        if(!usernameExists){
            tempAccount->username = NULL;
            tempAccount->password = NULL;
            tempAccount->interest = 0.0;
            tempAccount->checkingAccountBalance = 0.0;
            tempAccount->savingsAccountBalance = 0.0;
        }
    } else {
        printf("The table is empty, therefore the username does not exist.\n");
    }

    fclose(userInfoFile);
    //printf("Account Values: %s, %s, %f, %f, %f\n", userAccount->username, userAccount->password, userAccount->savingsAccountBalance, userAccount->checkingAccountBalance, userAccount->interest);
    return tempAccount;
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

void *accountInformationSql(char * username, sqlite3 * dataBase) {

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

void * updateAccountInformationSql(char * username, char * column, int value, sqlite3 * dataBase) {
    char * query = sqlite3_mprintf("UPDATE USER_INFO SET %s = '%d' WHERE USERNAME = '%s'", column, value, username);
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
void * updateBankAccount(bool updateSavings, int value, sqlite3 * dataBase) {
    bool balancePositive = true;
    if(updateSavings == 0){
        if(globalBankAccount->savingsAccountBalance + value < 0){
            printf("You cannot withdraw more than you have in your savings account.\n");
            balancePositive = false;
        } else {
            globalBankAccount->savingsAccountBalance += value;
            printf("Your new savings account balance is $%.2f.\n", globalBankAccount->savingsAccountBalance);
            updateAccountInformationSql(globalBankAccount->username, "SAVINGS", globalBankAccount->savingsAccountBalance, dataBase);
        }
    } else if (updateSavings == 1) {
        if(globalBankAccount->checkingAccountBalance + value < 0){
            printf("You cannot withdraw more than you have in your checking account.\n");
            balancePositive = false;
        } else {
            globalBankAccount->checkingAccountBalance += value;
            printf("Your new checking account balance is $%.2f.\n", globalBankAccount->checkingAccountBalance);
            updateAccountInformationSql(globalBankAccount->username, "CHECKING", globalBankAccount->checkingAccountBalance, dataBase);
        }
    }

    return 0;
}

void bankingSql(void) {
    sqlite3 * dataBase = NULL;
    sqlite3_open("userInfo.db", &dataBase);
    globalBankAccount = (BankAccount *)malloc(sizeof(BankAccount));
    int userOptions;

    // Buffers for the user to type in the their username and password
    char usernameInput[20];
    char passwordInput[20];

    // Different booleans used throughout the code
    bool userInputIncorrect = false;
    bool passwordInputIncorrect = false;
    bool userNameExists = false;
    bool loggedIn = false;
    bool amountPositive = false;

    double amount;
    
    // Creating a User Account Struct
    BankAccount * userAccount;
    userAccount = (BankAccount *)malloc(sizeof(BankAccount));

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
    } else if(userOptions == 2){ // The user wants to LOGIN
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
                while(!passwordInputIncorrect){
                    printf(BOLD(CYN("Password:\n")));
                    scanf("%s", passwordInput);
                    if(!strcmp(passwordInput, globalBankAccount->password)){
                        // Print out a few different options
                        printf(BLU("You are now logged in.\n"));
                        passwordInputIncorrect = true;
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
            // globalBankAccount = updateCSV(0, amount, globalBankAccount);

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
            // globalBankAccount = updateCSV(1, amount, globalBankAccount);
            updateBankAccount(1, amount, dataBase);
            break;
        case 5:
            printf("How much do you want to withdraw?\n");
            scanf("%lf", &amount);
            if(amount > 0){
                amount *= -1;
            } 
            // globalBankAccount = updateCSV(0, amount, globalBankAccount);
            updateBankAccount(0, amount, dataBase);
            break;
        case 6:
            printf("How much do you want to withdraw?\n");
            scanf("%lf", &amount);
            if(amount > 0){
                amount *= -1;
            }
            // globalBankAccount = updateCSV(1, amount, globalBankAccount);
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

// TODO: Look into using SQLite
void banking(void) {
    int userOptions;
    char usernameInput[20];
    char passwordInput[20];

    bool userInputIncorrect = false;
    bool passwordInputIncorrect = false;
    bool userNameExists = false;
    bool loggedIn = false;
    bool amountPositive = false;

    double amount;
    
    // Creating a User Account Struct
    BankAccount * userAccount;
    userAccount = (BankAccount *)malloc(sizeof(BankAccount));

    // Want to start the app up with a greeting
    printf("Welcome to the bank. How may I help you?\n");
    // Print options
    printf("Options:\n1: Create an Account\n2: Login to an Existing Account\n");

    // Constantly check and see if the user is inputting a correct input
    while(!userInputIncorrect) {
        scanf("%d", &userOptions);
        if(userOptions == 1 || userOptions == 2){
            userInputIncorrect = true;
        } else {
            printf("That is not a correct input. Input 1 or 2.\n");
        }
    }

    // The user wants to create an account
    if(userOptions == 1){
        while(!userNameExists){
            printf("Username:\n");
            scanf("%s", usernameInput);
            userAccount = accountInformation(usernameInput);
            if(userAccount->username == NULL){
                // Time to add stuff to the CSV
                userAccount->username = usernameInput;
                printf("Password:\n");
                scanf("%s", passwordInput);
                userAccount->password = passwordInput;
                userAccount->savingsAccountBalance = 0.0;
                userAccount->checkingAccountBalance = 0.0;
                // Interest is the amount applied to the savings account at the end of every month
                userAccount->interest = 0.05 / 100.0;

                printf("Before we save the data to the file\n");
                userInfoFile = fopen("userInfo.csv", "a+");
                fprintf(userInfoFile, "%s, %s, %f, %f, %f\n", userAccount->username, userAccount->password, userAccount->savingsAccountBalance, userAccount->checkingAccountBalance, userAccount->interest);
                printf("Your account has been created.\n");
                fclose(userInfoFile);
                userNameExists = true;
                loggedIn = true;
            } else {
                printf("%s already exists. Try another username or login.\n", usernameInput);
            }
        }
    } else if(userOptions == 2){
        while(!userNameExists){
            printf("Login:\nUsername:\n");
            scanf("%s", usernameInput);
            userAccount = accountInformation(usernameInput);
            //printf("Account Values: %s, %s, %f, %f, %f\n", userAccount->username, userAccount->password, userAccount->savingsAccountBalance, userAccount->checkingAccountBalance, userAccount->interest);
            if(userAccount->username != NULL){
                // Time to add stuff to the CSV
                userNameExists = true;
                while(!passwordInputIncorrect){
                    printf("Password:\n");
                    scanf("%s", passwordInput);
                    if(!strcmp(passwordInput, userAccount->password)){
                        // Print out a few different options
                        printf("You are now logged in.\n");
                        passwordInputIncorrect = true;
                        loggedIn = true;
                    }
                }
            } else {
                printf("This username does not exist.\n");
            }
        }
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
            printf("Savings Account Balance: $%.2f\n", userAccount->savingsAccountBalance);
            break;
        case 2:
            printf("Checking Account Balance: $%.2f\n", userAccount->checkingAccountBalance);
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
            userAccount = updateCSV(0, amount, userAccount);
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
            userAccount = updateCSV(1, amount, userAccount);
            break;
        case 5:
            printf("How much do you want to withdraw?\n");
            scanf("%lf", &amount);
            if(amount > 0){
                amount *= -1;
            } 
            userAccount = updateCSV(0, amount, userAccount);
            break;
        case 6:
            printf("How much do you want to withdraw?\n");
            scanf("%lf", &amount);
            if(amount > 0){
                amount *= -1;
            }
            userAccount = updateCSV(1, amount, userAccount);
            break;        
        case 7:
            printf("You are now logged out.");
            loggedIn = false;
            break;
        default:
            printf("That was not a valid option.\n");
        }
    }
    free(userAccount->username);
    free(userAccount->password);
    free(userAccount);
    exit(0);
}