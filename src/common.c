#include "./common.h"

bool
rconv_parse_bool(char* str)
{
	if (str == NULL) {
		return false;
	}

	utf8lwr(str);
	return strcmp(str, "y") == 0 || strcmp(str, "yes") == 0 || strcmp(str, "true") == 0 || strcmp(str, "1") || strcmp(str, "on");
}

char*
rconv_file_extension(RconvFileType type)
{
	// TODO: implement me
	return NULL;
}
