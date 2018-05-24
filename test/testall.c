#include <stdio.h>

#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>

#include "testcolorbruteforcer.h"


int main()
{
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    add_tests_colorbruterforcer();

    // Run all tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    printf("\n");
    CU_basic_show_failures(CU_get_failure_list());
    printf("\n\n");

    CU_cleanup_registry();
    return CU_get_error();
}
