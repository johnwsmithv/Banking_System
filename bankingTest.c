#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"
#include "bankingFunctions.h"

void testDepositSavings(void){
    BankAccount * beforeUpdate = (BankAccount *)malloc(sizeof(BankAccount));
    beforeUpdate = accountInformation("random1");
    beforeUpdate = updateCSV(0, 1500, beforeUpdate);
    BankAccount * afterUpdate = (BankAccount *)malloc(sizeof(BankAccount));
    afterUpdate = accountInformation("random1");
    CU_ASSERT(beforeUpdate->savingsAccountBalance == afterUpdate->savingsAccountBalance);

    free(beforeUpdate);
    free(afterUpdate);
}

void testDepositChecking(void){
    BankAccount * beforeUpdate = (BankAccount *)malloc(sizeof(BankAccount));
    beforeUpdate = accountInformation("random1");
    beforeUpdate = updateCSV(1, 150, beforeUpdate);
    BankAccount * afterUpdate = (BankAccount *)malloc(sizeof(BankAccount));
    afterUpdate = accountInformation("random1");
    CU_ASSERT(beforeUpdate->checkingAccountBalance == afterUpdate->checkingAccountBalance);

    free(beforeUpdate);
    free(afterUpdate);
}

void testWithdrawSavings(void){
    BankAccount * beforeUpdate = (BankAccount *)malloc(sizeof(BankAccount));
    beforeUpdate = accountInformation("random1");
    beforeUpdate = updateCSV(0, -1500, beforeUpdate);
    BankAccount * afterUpdate = (BankAccount *)malloc(sizeof(BankAccount));
    afterUpdate = accountInformation("random1");
    CU_ASSERT(beforeUpdate->savingsAccountBalance == afterUpdate->savingsAccountBalance);

    free(beforeUpdate);
    free(afterUpdate);
}

void testWithdrawChecking(void){
    BankAccount * beforeUpdate = (BankAccount *)malloc(sizeof(BankAccount));
    beforeUpdate = accountInformation("random1");
    beforeUpdate = updateCSV(1, -150, beforeUpdate);
    BankAccount * afterUpdate = (BankAccount *)malloc(sizeof(BankAccount));
    afterUpdate = accountInformation("random1");
    CU_ASSERT(beforeUpdate->checkingAccountBalance == afterUpdate->checkingAccountBalance);

    free(beforeUpdate);
    free(afterUpdate);
}

int main(){
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("Bank Tests", 0, 0);

    CU_add_test(suite, "depositSavings", testDepositSavings);
    CU_add_test(suite, "depositChecking", testDepositChecking);
    CU_add_test(suite, "withdrawSavings", testWithdrawSavings);
    CU_add_test(suite, "withdrawChecking", testWithdrawChecking);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}