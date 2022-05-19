#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "common.h"

#define RCONV_FLOAT_OK 0
#define RCONV_FLOAT_ERR_ILLEGAL_CHAR 1
#define RCONV_FLOAT_ERR_ILLEGAL_STATE 2
#define RCONV_FLOAT_ERR_INVALID_LEN 3

typedef struct {
    long long integer;
    size_t fraction;
} RconvFloat;

RconvFloat*
rconv_float_new_zero();

int
rconv_float_set_from_string(RconvFloat* result, const char* str);

void
rconv_float_set_from_number(RconvFloat* result, long long integer, size_t fraction);

void
rconv_float_set_from_float(RconvFloat* result, float value);

RconvFloat*
rconv_float_new_from_string(const char* str);

RconvFloat*
rconv_float_new_from_number(long long integer, size_t fraction);

RconvFloat*
rconv_float_new_from_float(float value);
