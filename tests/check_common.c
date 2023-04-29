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

START_TEST(test_is_integer_string)
{
    ck_assert(rconv_is_integer_string("123") == true);
    ck_assert(rconv_is_integer_string("+456") == true);
    ck_assert(rconv_is_integer_string("-789") == true);
    ck_assert(rconv_is_integer_string(" 123") == false);
    ck_assert(rconv_is_integer_string("hello") == false);
    ck_assert(rconv_is_integer_string("test123foobar") == false);
}
END_TEST

START_TEST(test_is_decimal_string)
{
    ck_assert(rconv_is_decimal_string("123") == true);
    ck_assert(rconv_is_decimal_string("+456") == true);
    ck_assert(rconv_is_decimal_string("-789") == true);
    ck_assert(rconv_is_decimal_string(" 123") == false);
    ck_assert(rconv_is_decimal_string("hello") == false);
    ck_assert(rconv_is_decimal_string("test123foobar") == false);

    ck_assert(rconv_is_decimal_string("123.0") == true);
    ck_assert(rconv_is_decimal_string("0.123") == true);
    ck_assert(rconv_is_decimal_string("+123.34") == true);
    ck_assert(rconv_is_decimal_string("-675.4") == true);
    ck_assert(rconv_is_decimal_string(".1337") == true);
    ck_assert(rconv_is_decimal_string("+.66") == true);
    ck_assert(rconv_is_decimal_string("-.4") == true);
    ck_assert(rconv_is_decimal_string("44.542") == true);

    ck_assert(rconv_is_decimal_string("12.0hello231") == false);
    ck_assert(rconv_is_decimal_string(".") == false);
    ck_assert(rconv_is_decimal_string("blafoo") == false);
}
END_TEST

Suite* dd_suite(void)
{
    Suite* s = suite_create("Common");

    TCase* tc_trim = tcase_create("Trim Strings");
    tcase_add_test(tc_trim, test_trim);
    suite_add_tcase(s, tc_trim);

    TCase* tc_is_integer_string = tcase_create("String is Integer");
    tcase_add_test(tc_is_integer_string, test_is_integer_string);
    suite_add_tcase(s, tc_is_integer_string);

    TCase* tc_is_decimal_string = tcase_create("String is Decimal");
    tcase_add_test(tc_is_decimal_string, test_is_decimal_string);
    suite_add_tcase(s, tc_is_decimal_string);

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
