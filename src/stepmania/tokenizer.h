#pragma once

#include <utf8.h>
#include <stdbool.h>

#include "../common.h"
#include "../data_descriptor.h"
#include "../list.h"
#include "types.h"

typedef enum {
    RCONV_STEPMANIA_TOKENIZER_STATE_CLEAN = 0,
    RCONV_STEPMANIA_TOKENIZER_STATE_LINE_COMMENT = 1,
    RCONV_STEPMANIA_TOKENIZER_STATE_IN_PROPERTY_NAME = 2,
    RCONV_STEPMANIA_TOKENIZER_STATE_IN_VALUE = 3,
    RCONV_STEPMANIA_TOKENIZER_STATE_IN_STRING = 4,
    RCONV_STEPMANIA_TOKENIZER_STATE_IN_NOTES_META = 5,
    RCONV_STEPMANIA_TOKENIZER_STATE_IN_NOTES = 6,
    RCONV_STEPMANIA_TOKENIZER_STATE_IN_LINE_COMMENT = 7,
} RconvStepmaniaTokenizerState;

RconvToken**
rconv_stepmania_tokenize(RconvDataDescriptor* dd, int* length);
