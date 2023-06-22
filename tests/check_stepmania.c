#include <check.h>

#include "./utils.h"
#include "../src/stepmania.h"

START_TEST(test_stepmania_tokenize)
{
    RconvToken** tokens = NULL;
    int length = 0;
    FILE* fp = NULL;
    RconvDataDescriptor* dd;

    fp = fopen("./test-files/sample1.sm", "r");
    dd = rconv_dd_new_from_file(fp);
    rconv_stepmania_tokenize(dd, &length);
    rconv_dd_free(dd);
    free(tokens);
}
END_TEST

START_TEST(test_parse_stepmania_from_file)
{
    RconvStepmaniaChartFile* chart;

    chart = rconv_stepmania_parse_from_file("./test-files/css2022.sm");
    ck_assert_ptr_nonnull(chart);
    ck_assert_str_eq(chart->artist, "Will Wood");
    ck_assert_str_eq(chart->title, "2econd 2ight 2eer (that was fun, goodbye)");
    ck_assert_str_eq(chart->subtitle, "From prefixaut to JohnFortnite");
    ck_assert_str_eq(chart->credit, "PreFiXAUT");
    ck_assert_str_eq(chart->music, "audio.mp3");
    ck_assert_str_eq(chart->banner, "bn.png");
    ck_assert_str_eq(chart->background, "bg.png");
    ck_assert_str_eq(chart->cd_title, "cdtitle.png");
    // rconv_ck_decimal_equal(chart->sample_start, 40, 583);
    rconv_stepmania_free_chart_file(chart);
}
END_TEST

Suite* stepmania_suite(void)
{
    Suite* s = suite_create("Stepmania");

    // TCase* tc_tokenize = tcase_create("Tokenize from File");
    // tcase_add_test(tc_tokenize, test_stepmania_tokenize);
    // suite_add_tcase(s, tc_tokenize);

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
