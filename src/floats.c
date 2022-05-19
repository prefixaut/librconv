#include "floats.h"

RconvFloat*
rconv_float_zero()
{
	RconvFloat* ptr = (RconvFloat*) malloc(sizeof(RconvFloat));
	ptr->integer = 0;
	ptr->fraction = 0;

	return ptr;
}

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

	size_t fltVal = 0;
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
			if (tmp == '-') {
				intStart = offset;
				offset++;
				state = 1;
				continue;
			}
			if (rconv_is_number(tmp)) {
				intStart = offset;
				offset++;
				state = 2;
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
				intEnd = offset;
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

	if (state == 3) {
		fltEnd = offset;
	}

	if (intStart > -1 && intEnd > -1) {
		int len = intEnd - intStart;
		if (len > 0) {
			char* tmp = rconv_substr(str, intStart, intEnd);
			if (tmp != NULL) {
				intVal = atoll(tmp);
				free(tmp);
				intSet = true;
			}
		}
	}

	if (fltStart > -1 && fltEnd > -1) {
		int len = fltEnd - fltStart;
		if (len > 0) {
			char* tmp = rconv_substr(str, fltStart, fltEnd);
			if (tmp != NULL) {
				fltVal = atoll(tmp);
				free(tmp);
				fltSet = true;
			}
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
rconv_float_from_number(RconvFloat* result, long long integer, size_t fraction)
{
	result->integer = integer;
	result->fraction = fraction;
}

void
rconv_float_from_float(RconvFloat* result, float value)
{
	// TODO: Implement me
}
