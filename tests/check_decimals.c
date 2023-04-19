#include <check.h>

#include "../src/decimals.h"

START_TEST(test_set_from_double)
{
    RconvDecimal* dec;
    
    dec = rconv_decimal_new(5);
    rconv_decimal_set_from_double(dec, 13.379);
    ck_assert_int_eq(dec->integer, 13);
    // 899 due to double precision handling
    ck_assert_int_eq(dec->fraction, 37899);
    rconv_decimal_free(dec);

    dec = rconv_decimal_new(5);
    rconv_decimal_set_from_double(dec, 77.2);
    ck_assert_int_eq(dec->integer, 77);
    ck_assert_int_eq(dec->fraction, 20000);
    rconv_decimal_free(dec);

    dec = rconv_decimal_new(2);
    rconv_decimal_set_from_double(dec, 841.246168);
    ck_assert_int_eq(dec->integer, 841);
    ck_assert_int_eq(dec->fraction, 24);
    rconv_decimal_free(dec);

    dec = rconv_decimal_new(2);
    rconv_decimal_set_from_double(dec, 99.12);
    ck_assert_int_eq(dec->integer, 99);
    ck_assert_int_eq(dec->fraction, 12);
    rconv_decimal_free(dec);
}
END_TEST

START_TEST(test_set_from_string)
{
    RconvDecimal* dec;

    dec = rconv_decimal_new(5);
    rconv_decimal_set_from_string(dec, "13.379");
    ck_assert_int_eq(dec->integer, 13);
    ck_assert_int_eq(dec->fraction, 37900);
    rconv_decimal_free(dec);

    dec = rconv_decimal_new(3);
    rconv_decimal_set_from_string(dec, "531.481023");
    ck_assert_int_eq(dec->integer, 531);
    ck_assert_int_eq(dec->fraction, 481);
    rconv_decimal_free(dec);

    dec = rconv_decimal_new(4);
    rconv_decimal_set_from_string(dec, "420.1337");
    ck_assert_int_eq(dec->integer, 420);
    ck_assert_int_eq(dec->fraction, 1337);
    rconv_decimal_free(dec);
}

Suite* stepmania_suite(void)
{
    Suite* s;
    s = suite_create("Decimals");

    TCase* tc_set_from_double = tcase_create("Set from Double");
    tcase_add_test(tc_set_from_double, test_set_from_double);
    suite_add_tcase(s, tc_set_from_double);

    TCase* tc_set_from_string = tcase_create("Set from String");
    tcase_add_test(tc_set_from_string, test_set_from_string);
    suite_add_tcase(s, tc_set_from_string);

    return s;
}

int main(void)
{
    int number_failed;
    Suite* s;
    SRunner* sr;

    s = stepmania_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return number_failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
