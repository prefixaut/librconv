#include "floats.h"

RconvFloat*
rconv_float_new()
{
	RconvFloat* ptr = (RconvFloat*) malloc(sizeof(RconvFloat));
	if (ptr == NULL) {
		return NULL;
	}
	ptr->integer = 0;
	ptr->fraction = 0;

	return ptr;
}

RconvFloat*
rconv_float_copy(RconvFloat* original)
{
	RconvFloat* cpy = rconv_float_new();
	cpy->flags = original->flags;
	cpy->integer = original->integer;
	cpy->fraction = original->fraction;
	cpy->fraction_offset = original->fraction_offset;
	return cpy;
}

char*
rconv_float_to_string(RconvFloat* value)
{
	bool negative = rconv_float_is_negative(value);

	if (rconv_float_is_nan(value)) {
		return negative ? "-NaN" : "NaN";
	} else if (rconv_float_is_infinity(value)) {
		return negative ? "-Infinity" : "Infinity";
	}

	char* format = NULL;
	bool free_format = false;
	char* filler = NULL;

	if (value->fraction_offset <= 0) {
		if (negative) {
			format = "-%zu.%zu";

		} else {
			format = "%zu.%zu";
		}
	} else {
		char* pre_format = NULL;
		if (negative) {
			pre_format = "-%%zu.%s%%zu";
		} else {
			pre_format = "%%zu.%s%%zu";
		}
		filler = rconv_repeat("0", value->fraction_offset);
		int format_len = 1 + snprintf(NULL, 0, pre_format, filler);
		format = calloc(format_len, sizeof(char));
		snprintf(format, format_len, pre_format, filler);
		free_format = true;
		free(filler);
		filler = NULL;
	}

	if (format == NULL) {
		return NULL;
	}

	int len = 1 + snprintf(NULL, 0, format, value->integer, value->fraction);
	char* out = calloc(len, sizeof(char));
	snprintf(out, len, format, value->integer, value->fraction);

	if (free_format) {
		free(format);
	}

	return out;
}

bool
rconv_float_is_nan(RconvFloat* value)
{
	return value->flags & RCONV_FLOAT_FLAG_NAN;
}

bool
rconv_float_is_infinity(RconvFloat* value)
{
	return value->flags & RCONV_FLOAT_FLAG_INFINITY;
}

bool
rconv_float_is_negative(RconvFloat* value)
{
	return value->flags & RCONV_FLOAT_FLAG_NEGATIVE;
}

int
rconv_float_set_from_string(RconvFloat* result, const char* str)
{
	size_t max = strlen(str);
	int offset = 0;
	int last_not_zero = 0;
	int state = 0;

	bool negative = false;
	long int_val = 0;
	bool int_set = false;
	int int_start = -1;
	int int_end = -1;

	size_t flt_val = 0;
	bool flt_set = false;
	int flt_start = -1;
	int flt_end = -1;

	while (offset < max) {
		char tmp = *(str + offset);

		if (state == 0) {
			if (rconv_is_whitespace(tmp)) {
				offset++;
				continue;
			}
			if (tmp == '-') {
				negative = true;
				offset++;
				int_start = offset;
				state = 1;
				continue;
			}
			if (rconv_is_number(tmp)) {
				int_start = offset;
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
				int_end = offset;
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
				if (flt_start == -1) {
					flt_start = offset;
				}
				if (tmp != '0') {
					last_not_zero = offset;
				}
				offset++;
				continue;
			}
			flt_end = last_not_zero;
			break;
		}

		return RCONV_FLOAT_ERR_ILLEGAL_STATE;
	}

	if (state == 3) {
		flt_end = offset;
	}

	if (int_start > -1 && int_end > -1) {
		int len = int_end - int_start;
		if (len > 0) {
			char* tmp = rconv_substr(str, int_start, int_end);
			if (tmp != NULL) {
				int_val = atoll(tmp);
				free(tmp);
				int_set = true;
			}
		}
	}

	if (flt_start > -1 && flt_end > -1) {
		int len = flt_end - flt_start;
		if (len > 0) {
			char* tmp = rconv_substr(str, flt_start, flt_end);
			if (tmp != NULL) {
				flt_val = atoll(tmp);
				free(tmp);
				flt_set = true;
			}
		}
	}

	if (!int_set && !flt_set) {
		return RCONV_FLOAT_ERR_INVALID_LEN;
	}

	if (int_set) {
		result->integer = int_val;
	} else {
		result->integer = 0;
	}

	if (flt_set) {
		result->fraction = flt_val;
	} else {
		result->fraction = 0;
	}

	if (negative) {
		result->flags = RCONV_FLOAT_FLAG_NEGATIVE;
	} else {
		result->flags = RCONV_FLOAT_FLAG_NONE;
	}

	return RCONV_FLOAT_OK;
}

void
rconv_float_set_from_number(RconvFloat* result, long long integer, size_t fraction, unsigned short fraction_offset)
{
	if (integer < 0) {
		result->integer = integer * -1;
		result->flags = RCONV_FLOAT_FLAG_NEGATIVE;
	} else {
		result->integer = integer;
		result->flags = RCONV_FLOAT_FLAG_NONE;
	}
	result->fraction = fraction;
	result->fraction_offset = fraction_offset;
}

void
rconv_float_set_from_float(RconvFloat* result, float value)
{
	// TODO: Implement me
}

RconvFloat*
rconv_float_new_from_string(const char* str)
{
	RconvFloat* ptr = rconv_float_new();
	rconv_float_set_from_string(ptr, str);
	return ptr;
}

RconvFloat*
rconv_float_new_from_number(long long integer, size_t fraction, unsigned short fraction_offset)
{
	RconvFloat* ptr = rconv_float_new();
	rconv_float_set_from_number(ptr, integer, fraction, fraction_offset);
	return ptr;
}

RconvFloat*
rconv_float_new_from_float(float value)
{
	RconvFloat* ptr = rconv_float_new();
	rconv_float_set_from_float(ptr, value);
	return ptr;
}

char*
rconv_float_long_to_str(size_t value)
{
	int len = 1 + snprintf(NULL, 0, "%zu", value);
	char* str = malloc(sizeof(char) * len);
	if (str == NULL) {
		return NULL;
	}
	snprintf(str, len, "%zu", value);
	return str;
}

int
rconv_float_fraction_len(size_t fraction)
{
	char* str = rconv_float_long_to_str(fraction);
	int out = strlen(str);
	free(str);
	return out;
}

void
rconv_float_normalize_fractions(unsigned short* offset, size_t* l, unsigned short loff, size_t* r, unsigned short roff)
{
	int llen = rconv_float_fraction_len(*l) + loff;
	int rlen = rconv_float_fraction_len(*r) + roff;
	if (loff >= roff) {
		*offset = roff;
	} else {
		*offset = loff;
	}

	if (llen == rlen) {
		return;
	}

	while (llen < rlen) {
		*l = *l * 10;
		llen++;
	}
	while (rlen < llen) {
		*r = *r * 10;
		rlen++;
	}
}

RconvFloat*
rconv_float_add(RconvFloat* left, RconvFloat* right)
{
	RconvFloat* out = malloc(sizeof(RconvFloat));

	if (out == NULL) {
		return NULL;
	}

	if (rconv_float_is_infinity(left) || rconv_float_is_infinity(right)) {
		out->flags = RCONV_FLOAT_FLAG_INFINITY;
		out->integer = 0;
		out->fraction = 0;
		return out;

	} else if (rconv_float_is_nan(left) || rconv_float_is_nan(right)) {
		out->flags = RCONV_FLOAT_FLAG_NAN;
		out->integer = 0;
		out->fraction = 0;
		return out;
	}

	size_t lf = left->fraction;
	size_t rf = right->fraction;
	size_t fsum;
	unsigned short foff = 0;
	int flags = RCONV_FLOAT_FLAG_NONE;

	rconv_float_normalize_fractions(&foff, &lf, left->fraction_offset, &rf, right->fraction_offset);
	int before_len = rconv_float_fraction_len(lf);
	bool ln = rconv_float_is_negative(left);
	bool rn = rconv_float_is_negative(right);

	if ((ln && rn) || (!ln && !rn)) {
		fsum = lf + rf;
		flags |= (ln && rn) ? RCONV_FLOAT_FLAG_NEGATIVE : RCONV_FLOAT_FLAG_NONE;
	} else if (ln) {
		flags = (lf > rf) ? RCONV_FLOAT_FLAG_NEGATIVE : RCONV_FLOAT_FLAG_NONE;
		if (flags & RCONV_FLOAT_FLAG_NEGATIVE) {
			fsum = lf - rf;
		} else {
			fsum = rf - lf;
		}
	} else {
		flags = (rf > lf) ? RCONV_FLOAT_FLAG_NEGATIVE : RCONV_FLOAT_FLAG_NONE;
		if (flags & RCONV_FLOAT_FLAG_NEGATIVE) {
			fsum = rf - lf;
		} else {
			fsum = lf - rf;
		}
	}

	int after_len = rconv_float_fraction_len(fsum);
	int diff = 0;

	if (before_len != after_len) {
		int len = after_len - before_len;
		char* str = rconv_float_long_to_str(fsum);

		if (len > 0) {
			char* sub = rconv_substr(str, 0, len);
			diff = atoi(sub);
			free(sub);
			char* new_sum = rconv_substr(str, len, strlen(str));
			fsum = atoi(new_sum);
			free(new_sum);
		} else {
			int tmp = len * -10;
			fsum = tmp - fsum;
			foff += len * -1;
		}
		free(str);
	}

	out->flags = flags;
	if ((ln && !rn) || (!ln && rn)) {
		if (left->integer > right->integer) {
			out->integer = left->integer - right->integer;
		} else if (left->integer < right->integer) {
			out->integer = right->integer - left->integer;
		} else {
			out->integer = 0;
		}
	} else {
		out->integer = left->integer + right->integer;
	}
	out->integer += diff;
	out->fraction = fsum;
	out->fraction_offset = foff;

	return out;
}

RconvFloat*
rconv_float_subtract(RconvFloat* left, RconvFloat* right)
{
	RconvFloat* rcpy = rconv_float_copy(right);
	// Flip the negative flag
	rcpy->flags &= ~RCONV_FLOAT_FLAG_NEGATIVE;
	RconvFloat* out = rconv_float_add(left, rcpy);
	free(rcpy);
	return out;
}
