#include <check.h>

#include "../src/stepmania.h"

START_TEST(test_parse_stepmania_from_file)
{
    RconvStepmaniaChartFile* chart;

    chart = rconv_stepmania_parse_from_file("./test-files/css2022.sm");
    ck_assert_str_eq(chart->artist, "Will Wood");
    ck_assert_str_eq(chart->title, "2econd 2ight 2eer (that was fun, goodbye)");
    ck_assert_str_eq(chart->subtitle, "From prefixaut to JohnFortnite");

    rconv_stepmania_free_chart_file(chart);
}
END_TEST

Suite* stepmania_suite(void)
{
    Suite* s = suite_create("Stepmania");

    TCase* tc_parse = tcase_create("Parse from File");
    tcase_add_test(tc_parse, test_parse_stepmania_from_file);
    suite_add_tcase(s, tc_parse);

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
