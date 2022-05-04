#pragma once

#ifndef _INC_RCONV_COMMON
#	define _INC_RCONV_COMMON

#	include <limits.h>
#	include <stdbool.h>
#	include <string.h>

/*
 * Definitions
 */
#	define RCONV_ONE_MINUTE 60000

/*
 * Constants
 */
const char* RCONV_PLACEHOLDER_TITLE = "%title%";
const char* RCONV_PLACEHOLDER_ARTIST = "%artist";
const char* RCONV_PLACEHOLDER_DIFFICULTY = "%difficulty%";
const char* RCONV_PLACEHOLDER_LEVEL = "%level%";
const char* RCONV_PLACEHOLDER_MODE = "%mode%";
const char* RCONV_DEFAULT_FOLDER_FORMAT = "%title% (%artist%)";
const char* RCONV_DEFAULT_CHART_FORMAT = "%artist% - %title%_%difficulty%_%level%_%mode%";
const char* RCONV_DEFAULT_NON_DIFFICULTY_CHART_FORMAT = "%artist% - %title%";

/*
 * Types
 */
typedef enum {
	MEMO,
	MEMO2,
	MALODY,
	STEPMANIA,
	STEPMANIA5,
	KICK_IT_UP,
	FXF,
} RconvFileType;

typedef struct {
	char* title;
	char* artist;
	char* difficulty;
	char* level;
	char* mode;
	char* extension;
} RconvFormattingParameters;

typedef struct {
	int* array;
	size_t used;
	size_t size;
} RconvList;

/*
 * Functions
 */
char*
rconv_detect_file_type(char* file_path);

char*
rconv_file_extension(RconvFileType type);

char*
rconv_default_chart_format(RconvFileType type);

bool
rconv_parse_bool(char* str);

#endif
