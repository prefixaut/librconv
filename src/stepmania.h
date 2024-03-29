#pragma once

/*
 * Libraries
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utf8.h>

/*
 * Own modules
 */
#include "common.h"
#include "data_descriptor.h"
#include "decimals.h"
#include "list.h"
#include "stepmania/types.h"
#include "stepmania/parsers.h"
#include "stepmania/tokenizer.h"

/*
 * Definitions
 */
#define RCONV_STEPMANIA_SPECIAL_NOTE_START 'D'

/*
 * Functions
 */
RconvStepmaniaChartFile*
rconv_stepmania_parse(RconvDataDescriptor* dd);

RconvStepmaniaChartFile*
rconv_stepmania_parse_from_tokens(RconvToken** tokens, int token_len);

RconvStepmaniaChartFile*
rconv_stepmania_parse_from_string(char* data);

RconvStepmaniaChartFile*
rconv_stepmania_parse_from_file(char* file_path);

RconvFormattingParameters
rconv_stepmania_formatting_parameters(RconvStepmaniaChartFile* chart);
