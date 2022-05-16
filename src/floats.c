#include "floats.h"

int
rconv_float_from_string(RconvFloat* result, const char* str)
{
	size_t max = strlen(str);
	int offset = 0;
	int state = 0;

	long intVal = 0;
	bool intSet = false;
	int intStart = -1;
	int intEnd = -1;

	long fltVal = 0;
	bool fltSet = false;
	int fltStart = -1;
	int fltEnd = -1;

	while (offset < max) {
		char tmp = *(str + offset);

		if (state == 0) {
			if (rconv_is_whitespace(tmp)) {
				offset++;
				continue;
			}
			if (rconv_is_number(tmp)) {
				intStart = offset;
				offset++;
				state = 2;
				continue;
			}
			if (tmp == '-') {
				intStart = offset;
				offset++;
				state = 1;
				continue;
			}

			return RCONV_FLOAT_ERR_ILLEGAL_CHAR;
		}

		if (state == 1) {
			if (rconv_is_number(tmp)) {
				offset++;
				state = 2;
				continue;
			}

			return RCONV_FLOAT_ERR_ILLEGAL_CHAR;
		}

		if (state == 2) {
			if (tmp == '.') {
				intEnd = offset - 1;
				offset++;
				state = 3;
				continue;
			}
			if (rconv_is_number(tmp)) {
				offset++;
				continue;
			}

			return RCONV_FLOAT_ERR_ILLEGAL_CHAR;
		}

		if (state == 3) {
			if (rconv_is_number(tmp)) {
				if (fltStart == -1) {
					fltStart = offset;
				}
				offset++;
				continue;
			}
			fltEnd = offset;
			break;
		}

		return RCONV_FLOAT_ERR_ILLEGAL_STATE;
	}

	if (intStart > -1 && intEnd > -1) {
		int len = intEnd - intStart;
		if (len > 0) {
			char* tmp = (char*) malloc((len + 1) * sizeof(char));
			strcpy_s(tmp, len + 1, str + intStart);
			intVal = atoi(tmp);
			free(tmp);
			intSet = true;
		}
	}

	if (fltStart > -1 && fltEnd > -1) {
		int len = fltEnd - fltStart;
		if (len > 0) {
			char* tmp = (char*) malloc((len + 1) * sizeof(char));
			strcpy_s(tmp, len + 1, str + fltStart);
			fltVal = atoi(tmp);
			free(tmp);
			fltSet = true;
		}
	}

	if (!intSet && !fltSet) {
		return RCONV_FLOAT_ERR_INVALID_LEN;
	}

	if (intSet) {
		result->integer = intVal;
	} else {
		result->integer = 0;
	}

	if (fltSet) {
		result->fraction = fltVal;
	} else {
		result->fraction = 0;
	}

	return RCONV_FLOAT_OK;
}

void
rconv_float_from_number(RconvFloat* result, long integer, long fraction)
{
	result->integer = integer;
	result->fraction = fraction;
}

void
rconv_float_from_float(RconvFloat* result, float value)
{
	// TODO: Implement me
}
