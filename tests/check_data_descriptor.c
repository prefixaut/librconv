#include <check.h>

#include "../src/data_descriptor.h"

#define TEST_DD_CONTENT "hello world\
multiple lines\
\
some other content in here\
123456789"

START_TEST(test_dd_new_from_file)
{
    RconvDataDescriptor* dd;
    char* path;

    path = "./test-files/dd.txt";
    dd = rconv_dd_new_from_file(path);
    ck_assert_int_eq(dd->type, RCONV_DD_TYPE_FILE);
    ck_assert_int_eq(dd->position, 0);
    rconv_dd_free(dd);
}
END_TEST

START_TEST(test_dd_new_from_string)
{
    RconvDataDescriptor* dd;
    char* content;

    content = TEST_DD_CONTENT;
    dd = rconv_dd_new_from_string(content);
    ck_assert_int_eq(dd->type, RCONV_DD_TYPE_MEMORY);
    ck_assert_int_eq(dd->position, 0);
    rconv_dd_free(dd);
}
END_TEST

START_TEST(test_dd_read_from_string)
{
    RconvDataDescriptor* dd;
    char* content;
    char* read;

    content = TEST_DD_CONTENT;
    dd = rconv_dd_new_from_string(content);
    read = malloc(sizeof(char) * 12);
    rconv_dd_read_string(dd, read, 11);
    ck_assert_str_eq(read, "hello world");
    free(read);
    rconv_dd_free(dd);
}
END_TEST

Suite* dd_suite(void)
{
    Suite* s = suite_create("Data Descriptor");

    TCase* tc_new_from_file = tcase_create("Create from File");
    tcase_add_test(tc_new_from_file, test_dd_new_from_file);
    suite_add_tcase(s, tc_new_from_file);

    TCase* tc_new_from_string = tcase_create("Create from String");
    tcase_add_test(tc_new_from_string, test_dd_new_from_string);
    suite_add_tcase(s, tc_new_from_string);

    TCase* tc_read_from_string = tcase_create("Read from String");
    tcase_add_test(tc_read_from_string, test_dd_read_from_string);
    suite_add_tcase(s, tc_read_from_string);

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
