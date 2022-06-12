#include "./rconv.h.in"

int
main()
{
	setbuf(stdout, NULL);

	RconvStepmaniaChartFile* chart = rconv_stepmania_parse_from_file("E:\\workspace\\librconv\\resources\\testfile.sm");
	if (chart != NULL) {
		printf("title: %s\n", chart->title);
		rconv_stepmania_free_chart_file(chart);
	}

    return 0;
}
