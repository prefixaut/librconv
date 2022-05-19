#include "./rconv.h.in"

int
main()
{
	char* buffer = NULL;
	long length;
	FILE* fp;
	errno_t err;

	if ((err = fopen_s(&fp, "E:\\workspace\\librconv\\resources\\TWO-TORIAL_DourGent\\TWO-TORIAL (DourGent)\\TWO-TORIAL.sm", "r")) == 0) {
		fseek(fp, 0, SEEK_END);
		length = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		buffer = malloc(length);
		if (buffer) {
			fread(buffer, 1, length, fp);
		}
		fclose(fp);
	} else {
		printf("%s\n", strerror(err));
		return 1;
	}

	if (buffer != NULL) {
		RconvStepmaniaChartFile* chart = rconv_stepmania_parse(buffer);
        free(buffer);
        printf("title: %s\n", chart->title);
        free(chart);
	}

    return 0;
}
