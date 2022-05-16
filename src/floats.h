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
    long integer;
    long fraction;
} RconvFloat;

int
rconv_float_from_string(RconvFloat* result, const char* str);

void
rconv_float_from_number(RconvFloat* result, long integer, long fraction);

void
rconv_float_from_float(RconvFloat* result, float value);
