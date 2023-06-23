#pragma once

#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#define RCONV_DECIMAL_OK 0
#define RCONV_DECIMAL_ERR_ILLEGAL_CHAR 1
#define RCONV_DECIMAL_ERR_ILLEGAL_STATE 2
#define RCONV_DECIMAL_ERR_INVALID_LEN 3

typedef struct {
	int integer;
	int fraction;
    int precision;
	/**
	 * Edge case flag for when integer is 0.
	 * Example: -0.1, would be split into two integers 0, and 1 (precision 1).
	 * Since negative 0 doesn't exist, the negative information for the fraction would
	 * be lost and turn the decimal into 0.1 instead.
	 */
	bool negative;
} RconvDecimal;

RconvDecimal*
rconv_decimal_new(int precision);

void
rconv_decimal_free(RconvDecimal* ptr);

RconvDecimal*
rconv_decimal_new_from_int(int precision, int integer, int fraction);

RconvDecimal*
rconv_decimal_new_from_double(int precision, double value);

int
rconv_decimal_set_from_double(RconvDecimal* result, double value);

RconvDecimal*
rconv_decimal_new_from_string(int precision, const char* str);

int
rconv_decimal_set_from_string(RconvDecimal* result, const char* str);

RconvDecimal*
rconv_decimal_add(RconvDecimal* left, RconvDecimal* right);

RconvDecimal*
rconv_decimal_subtract(RconvDecimal* left, RconvDecimal* right);

bool
rconv_decimal_is_zero(RconvDecimal* ptr);

bool
rconv_decimal_is_equal(RconvDecimal* one, RconvDecimal* two);

char*
rconv_decimal_to_string(RconvDecimal* value);

void
rconv_decimal_print(RconvDecimal* value);
