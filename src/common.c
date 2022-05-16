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

bool
rconv_is_whitespace(char c)
{
	return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

bool
rconv_is_number(char c)
{
	return c >= 48 && c <= 57;
}

char*
rconv_file_extension(RconvFileType type)
{
	// TODO: implement me
	return NULL;
}
