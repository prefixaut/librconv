#pragma once

#include "common.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RCONV_FLOAT_OK 0
#define RCONV_FLOAT_ERR_ILLEGAL_CHAR 1
#define RCONV_FLOAT_ERR_ILLEGAL_STATE 2
#define RCONV_FLOAT_ERR_INVALID_LEN 3
#define RCONV_FLOAT_FLAG_NONE 0
#define RCONV_FLOAT_FLAG_NEGATIVE 1
#define RCONV_FLOAT_FLAG_NAN 2
#define RCONV_FLOAT_FLAG_INFINITY 4

typedef struct {
	/// Flags of the Float
	short flags;
	/// The integer part of the float
	size_t integer;
	/// The fraction part of the float
	size_t fraction;
	/// The power of 10 the fraction is offset
	/// 0.02 would be:
	/// - integer: 0
	/// - fraction: 2
	/// - fraction_offset: 1
	unsigned short fraction_offset;
} RconvFloat;

RconvFloat*
rconv_float_new();

RconvFloat*
rconv_float_copy(RconvFloat* original);

char*
rconv_float_to_string(RconvFloat* value);

bool
rconv_float_is_nan(RconvFloat* value);

bool
rconv_float_is_infinity(RconvFloat* value);

bool
rconv_float_is_negative(RconvFloat* value);

int
rconv_float_set_from_string(RconvFloat* result, const char* str);

void
rconv_float_set_from_number(RconvFloat* result, long long integer, size_t fraction, unsigned short fraction_offset);

void
rconv_float_set_from_float(RconvFloat* result, float value);

RconvFloat*
rconv_float_new_from_string(const char* str);

RconvFloat*
rconv_float_new_from_number(long long integer, size_t fraction, unsigned short fraction_offset);

RconvFloat*
rconv_float_new_from_float(float value);

RconvFloat*
rconv_float_add(RconvFloat* left, RconvFloat* right);

RconvFloat*
rconv_float_subtract(RconvFloat* left, RconvFloat* right);
