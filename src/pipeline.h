#pragma once

/*
 * Libraries
 */
#include <stdbool.h>

/*
 * Own modules
 */
#include "common.h"

/*
 * Types
 */
typedef struct {
	bool bundle;
	bool song_folders;
	bool json_pretty;
	bool keep;
	bool lenient;
	bool merge;
	char* output;
	bool preserve;
	bool resources;
	bool normalize;
	char* folder_format;
	char* chart_format;
} RconvConvertOptions;

typedef struct {
	char* folder_name;
	char* file_path;
} RconvConvertResult;

/*
 * Functions
 */
RconvConvertResult*
rconv_convert(char* file_path, RconvFileType from, RconvFileType to, RconvConvertOptions options);

RconvConvertOptions
rconv_default_options(RconvFileType type);

char*
rconv_format_folder_name(RconvConvertOptions options, RconvFormattingParameters params);

char*
rconv_format_file_name(RconvConvertOptions options, RconvFormattingParameters params);
