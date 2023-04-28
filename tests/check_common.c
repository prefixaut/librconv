#include <stdlib.h>
#include <math.h>
#include <check.h>

#include "../src/common.h"

START_TEST(test_trim)
{
    // Annoying workaround to make the string dynamically allocated to use trim properly
    char* str = "  hello world\n 123\n \t ";
    char* tmp = malloc(sizeof(char) * 24);
    strcpy(tmp, str);

    rconv_trim(&tmp);
    printf("%s\n", tmp);
    ck_assert_str_eq(tmp, "hello world\n 123");
}
END_TEST


Suite* dd_suite(void)
{
    Suite* s = suite_create("Common");

    TCase* tc_trim = tcase_create("Trim Strings");
    tcase_add_test(tc_trim, test_trim);
    suite_add_tcase(s, tc_trim);

    return s;
}

int main(void)
{
    int number_failed;
    Suite* s;
    SRunner* sr;

    s = dd_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return number_failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
