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
