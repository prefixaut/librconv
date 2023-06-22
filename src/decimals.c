#include "decimals.h"

RconvDecimal*
rconv_decimal_new(int precision)
{
	RconvDecimal* ptr = (RconvDecimal*) calloc(1, sizeof(RconvDecimal));

	ptr->integer = 0;
	ptr->fraction = 0;
	ptr->precision = precision;

	return ptr;
}

void
rconv_decimal_free(RconvDecimal* ptr)
{
	if (ptr != NULL) {
		free(ptr);
	}
}

RconvDecimal*
rconv_decimal_new_from_int(int precision, int integer, int fraction)
{
	RconvDecimal* ptr = rconv_decimal_new(precision);
	ptr->integer = integer;

	// Fix the precision of the fraction
	int fract_max = pow(10, precision);
	while (fraction > fract_max) {
		fraction = (int) fraction / 10;
	}
	while (fract_max > (fraction * 10)) {
		fraction *= 10;
	}

	ptr->fraction = fraction;

	return ptr;
}


RconvDecimal*
rconv_decimal_new_from_double(int precision, double value)
{
	RconvDecimal* ptr = rconv_decimal_new(precision);
	int did_set = rconv_decimal_set_from_double(ptr, value);

	if (did_set == RCONV_DECIMAL_OK) {
		return ptr;
	}

	rconv_decimal_free(ptr);
	return NULL;
}

int
rconv_decimal_set_from_double(RconvDecimal* result, double value)
{
	result->integer = (int) value;
	// Get the fraction value without the integer part
	double tmp = (value - (double) result->integer);
	// Multiply the fraction to be in the range of precision and discard the rest
	result->fraction = (int) (tmp * pow(10, result->precision));

	return RCONV_DECIMAL_OK;
}

RconvDecimal*
rconv_decimal_new_from_string(int precision, const char* str)
{
	RconvDecimal* ptr = rconv_decimal_new(precision);
	int did_set = rconv_decimal_set_from_string(ptr, str);

	if (did_set == RCONV_DECIMAL_OK) {
		return ptr;
	}

	rconv_decimal_free(ptr);
	return NULL;
}

int
rconv_decimal_set_from_string(RconvDecimal* result, const char* str)
{
	if (str == NULL) {
		return RCONV_DECIMAL_ERR_ILLEGAL_CHAR;
	}

	int state = 0;
	size_t pos = 0;
	size_t fraction_start = 0;
	int fraction_len = 0;
	size_t end = 0;
	size_t len = strlen(str);
	bool did_end = false;

	int integer = 0;
	int fraction = 0;

	while (pos < len) {
		char tmp = *(str + pos);

		if (state == 0) {
			if (tmp == '.') {
				char* slice = rconv_substr(str, 0, pos);
				integer = atoi(slice);
				free(slice);

				pos++;
				fraction_start = pos;
				state = 1;
				continue;
			}

			if (!isdigit(tmp)) {
				return RCONV_DECIMAL_ERR_ILLEGAL_CHAR;
			}

			pos++;
			continue;
		}

		if (state == 1) {
			if (!isdigit(tmp)) {
				end = pos;
				did_end = true;
				break;
			}

			pos++;
			continue;
		}

		return RCONV_DECIMAL_ERR_ILLEGAL_STATE;
	}

	if (!did_end) {
		end = len;
	}

	if (fraction_start > 0) {
		char* slice = rconv_substr(str, fraction_start, end);
		fraction_len = end - fraction_start;
		fraction = atoi(slice);
		free(slice);

		if (fraction_len < result->precision) {
			fraction = fraction * pow(10, result->precision - fraction_len);
		} else if (fraction_len > result->precision) {
			fraction = (int) (fraction / pow(10, fraction_len - result->precision));
		}
	}

	result->integer = integer;
	result->fraction = fraction;

	return RCONV_DECIMAL_OK;
}

int*
_rconv_decimal_normalize_fractions(RconvDecimal* left, RconvDecimal* right)
{
	int* out = malloc(sizeof(int) * 3);
	out[0] = left->precision;
	out[1] = left->fraction;
	out[2] = right->fraction;

	if (left->precision == right->precision) {
		return out;
	}

	if (left->precision > right->precision) {
		int inc = pow(10, left->precision - right->precision);
		out[2] = right->fraction * inc;
	} else {
		int inc = pow(10, right->precision - left->precision);
		out[1] = left->fraction * inc;
	}

	return out;
}

RconvDecimal*
rconv_decimal_add(RconvDecimal* left, RconvDecimal* right)
{
	int int_sum = left->integer + right->integer;

	int* normalized = _rconv_decimal_normalize_fractions(left, right);

	int fract_max = pow(10, normalized[0]);
	int fract_sum = left->fraction + right->fraction;

	// It can only be partially bigger, as we limit the precision.
	// In case of precision 4, we would have a max of 1000
	// So maximum would be something like 999 + 999 = 1998
	if (fract_sum >= fract_max) {
		fract_sum -= fract_max;
		int_sum += 1;
	}

	RconvDecimal* out = rconv_decimal_new_from_int(normalized[0], int_sum, fract_sum);
	free(normalized);

	return out;
}

RconvDecimal*
rconv_decimal_subtract(RconvDecimal* left, RconvDecimal* right)
{
	return left;
}

bool
rconv_decimal_is_zero(RconvDecimal* ptr)
{
	return ptr == NULL || (ptr->integer == 0 && ptr->fraction == 0);
}

bool
rconv_decimal_is_equal(RconvDecimal* one, RconvDecimal* two)
{
	if (one == NULL && two == NULL) {
		return true;
	}
	if (one == NULL || two == NULL) {
		return false;
	}

	int* normalized = _rconv_decimal_normalize_fractions(one, two);
	bool out = one->integer == two->integer && normalized[1] == normalized[2];
	free(normalized);

	return out;
}

char*
rconv_decimal_to_string(RconvDecimal* value)
{
	if (value == NULL) {
		return NULL;
	}

	int int_len = snprintf(NULL, 0, "%d", value->integer);
	int len = int_len + (value->fraction > 0 ? 1 + value->precision : 0);
	char* dest = malloc(sizeof(char) * (len + 1));
	snprintf(dest, int_len + 1, "%d", value->integer);

	if (value->fraction > 0) {
		dest[int_len] = '.';
		snprintf(dest + int_len + 1, value->precision + 1, "%d", value->fraction);
	}

	return dest;
}
