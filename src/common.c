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
rconv_substr(const char* str, size_t start, size_t end)
{
	long long len = end - start;
	if (len <= 0) {
		return NULL;
	}
	// len+1 for the null-byte at the end
	size_t size = (len + 1) * sizeof(char);
	char* out = (char*) malloc(size);
	if (out == NULL) {
		return NULL;
	}
	strncpy(out, str + start, len);
	out[len] = '\0';

	return out;
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
rconv_repeat(char* str, int amount)
{
	size_t str_len = strlen(str);
	int size = str_len * amount + 1;
	char* out = calloc(size, sizeof(char));

	for (size_t i = 0; i < amount; i++) {
		void* pos = out + (i * str_len);
		if (pos == NULL) {
			free(out);
			out = NULL;
			break;
		}
		memcpy(pos, str, str_len);
	}

	return out;
}

char*
rconv_file_extension(RconvFileType type)
{
	// TODO: implement me
	return NULL;
}

void
rconv_free_all_strings(int len, char** strings)
{
	for (int i = 0; i < len; i++) {
		free(strings + i);
	}
	free(strings);
}
