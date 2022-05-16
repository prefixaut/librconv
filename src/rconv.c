#include "./rconv.h.in"

int
main()
{
	char* buffer = 0;
	long length;
	FILE* fp;
	errno_t err;

	if ((err = fopen_s(&fp, "", "rb")) != 0) {
		fseek(fp, 0, SEEK_END);
		length = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		buffer = malloc(length);
		if (buffer) {
			fread(buffer, 1, length, fp);
		}
		fclose(fp);
	} else {
		puts("Error oh nooo");
		return 1;
	}

	if (buffer) {
		RconvStepmaniaChartFile* chart = rconv_stepmania_parse(buffer);
        free(buffer);
        printf("title: %s", chart->title);
        free(chart);
	}

    return 0;
}
