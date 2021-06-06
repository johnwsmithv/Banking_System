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
            printf("You cannot withdraw more than you have in your savings account.");
            balancePositive = false;
        } else {
            tempAccount->savingsAccountBalance += depositWithdrawValue;
            printf("Your new savings account balance is $%.2f.\n", tempAccount->savingsAccountBalance);
        }
    } else if (updateSavingsChecking == 1) {
        if(tempAccount->checkingAccountBalance + depositWithdrawValue < 0){
            printf("You cannot withdraw more than you have in your checking account.");
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
        }
    }
    free(userAccount->username);
    free(userAccount->password);
    free(userAccount);
    exit(0);
}