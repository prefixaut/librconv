#include "./rconv.h.in"

int
main()
{
	setbuf(stdout, NULL);

	RconvStepmaniaChartFile* chart = rconv_stepmania_parse_from_file("/mnt/e/workspace/librconv/resources/testfile.sm");
	printf("title: %s\n", chart->title);
	rconv_stepmania_free_chart_file(chart);

    return 0;
}
