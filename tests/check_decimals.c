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
END_TEST

START_TEST(test_new_from_int)
{
    RconvDecimal* dec;
    
    dec = rconv_decimal_new_from_int(5, 12345, 67890);
    ck_assert_int_eq(dec->precision, 5);
    ck_assert_int_eq(dec->integer, 12345);
    ck_assert_int_eq(dec->fraction, 67890);
    rconv_decimal_free(dec);

    dec = rconv_decimal_new_from_int(5, 12345, 678);
    ck_assert_int_eq(dec->precision, 5);
    ck_assert_int_eq(dec->integer, 12345);
    ck_assert_int_eq(dec->fraction, 67800);
    rconv_decimal_free(dec);

    dec = rconv_decimal_new_from_int(5, 12345, 1234567890);
    ck_assert_int_eq(dec->precision, 5);
    ck_assert_int_eq(dec->integer, 12345);
    ck_assert_int_eq(dec->fraction, 12345);
    rconv_decimal_free(dec);
}
END_TEST

START_TEST(test_is_equal)
{
    RconvDecimal* left;
    RconvDecimal* right;

    left = rconv_decimal_new_from_int(3, 123, 999);
    right = rconv_decimal_new_from_int(3, 123, 999);
    ck_assert(rconv_decimal_is_equal(left, right) == true);
    rconv_decimal_free(left);
    rconv_decimal_free(right);

    left = rconv_decimal_new_from_int(5, 123, 99900);
    right = rconv_decimal_new_from_int(3, 123, 999);
    ck_assert(rconv_decimal_is_equal(left, right) == true);
    rconv_decimal_free(left);
    rconv_decimal_free(right);

    left = rconv_decimal_new_from_int(3, 123, 999);
    right = rconv_decimal_new_from_int(5, 123, 99900);
    ck_assert(rconv_decimal_is_equal(left, right) == true);
    rconv_decimal_free(left);
    rconv_decimal_free(right);

    left = rconv_decimal_new_from_int(3, 123, 999);
    right = rconv_decimal_new_from_int(3, 123, 999);
    ck_assert(rconv_decimal_is_equal(left, right) == true);
    rconv_decimal_free(left);
    rconv_decimal_free(right);

    left = rconv_decimal_new_from_int(3, 123, 123);
    right = rconv_decimal_new_from_int(3, 123, 999);
    ck_assert(rconv_decimal_is_equal(left, right) == false);
    rconv_decimal_free(left);
    rconv_decimal_free(right);

    left = rconv_decimal_new_from_int(3, 123, 999);
    right = rconv_decimal_new_from_int(3, 123, 123);
    ck_assert(rconv_decimal_is_equal(left, right) == false);
    rconv_decimal_free(left);
    rconv_decimal_free(right);

    left = rconv_decimal_new_from_int(3, 999, 999);
    right = rconv_decimal_new_from_int(3, 123, 999);
    ck_assert(rconv_decimal_is_equal(left, right) == false);
    rconv_decimal_free(left);
    rconv_decimal_free(right);

    left = rconv_decimal_new_from_int(3, 123, 999);
    right = rconv_decimal_new_from_int(3, 999, 999);
    ck_assert(rconv_decimal_is_equal(left, right) == false);
    rconv_decimal_free(left);
    rconv_decimal_free(right);
}
END_TEST

START_TEST(test_add)
{
    RconvDecimal* left;
    RconvDecimal* right;
    RconvDecimal* sum;
    RconvDecimal* expected;

    left = rconv_decimal_new_from_int(5, 1337, 12345);
    right = rconv_decimal_new_from_int(5, 33, 12345);
    sum = rconv_decimal_add(left, right);
    expected = rconv_decimal_new_from_int(5, 1370, 24690);
    ck_assert(rconv_decimal_is_equal(sum, expected) == true);
    rconv_decimal_free(left);
    rconv_decimal_free(right);
    rconv_decimal_free(sum);
    rconv_decimal_free(expected);
}
END_TEST

START_TEST(test_to_string)
{
    RconvDecimal* val;
    char* str;

    val = rconv_decimal_new_from_int(5, 1337, 12345);
    str = rconv_decimal_to_string(val);
    ck_assert_str_eq(str, "1337.12345");
    rconv_decimal_free(val);
    free(str);
}
END_TEST

Suite* decimals_suite(void)
{
    Suite* s = suite_create("Decimals");

    TCase* tc_set_from_double = tcase_create("Set from Double");
    tcase_add_test(tc_set_from_double, test_set_from_double);
    suite_add_tcase(s, tc_set_from_double);

    TCase* tc_set_from_string = tcase_create("Set from String");
    tcase_add_test(tc_set_from_string, test_set_from_string);
    suite_add_tcase(s, tc_set_from_string);

    TCase* tc_new_from_int = tcase_create("Create from Integers");
    tcase_add_test(tc_new_from_int, test_new_from_int);
    suite_add_tcase(s, tc_new_from_int);

    TCase* tc_is_equal = tcase_create("Is Equal");
    tcase_add_test(tc_is_equal, test_is_equal);
    suite_add_tcase(s, tc_is_equal);

    TCase* tc_add = tcase_create("Add");
    tcase_add_test(tc_add, test_add);
    suite_add_tcase(s, tc_add);

    TCase* tc_to_string = tcase_create("To String");
    tcase_add_test(tc_to_string, test_to_string);
    suite_add_tcase(s, tc_to_string);

    return s;
}

int main(void)
{
    int number_failed;
    Suite* s;
    SRunner* sr;

    s = decimals_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return number_failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
