#pragma once

#include <limits.h>
#include <stdbool.h>
#include <string.h>
#include <utf8.h>

/*
 * Definitions
 */
#define RCONV_ONE_MINUTE 60000
#define RCONV_PLACEHOLDER_TITLE "%title%";
#define RCONV_PLACEHOLDER_ARTIST "%artist";
#define RCONV_PLACEHOLDER_DIFFICULTY "%difficulty%";
#define RCONV_PLACEHOLDER_LEVEL "%level%";
#define RCONV_PLACEHOLDER_MODE "%mode%";
#define RCONV_DEFAULT_FOLDER_FORMAT "%title% (%artist%)";
#define RCONV_DEFAULT_CHART_FORMAT "%artist% - %title%_%difficulty%_%level%_%mode%";
#define RCONV_DEFAULT_NON_DIFFICULTY_CHART_FORMAT "%artist% - %title%";

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

/*
 * Functions
 */
char*
rconv_detect_file_type(char* file_path);

char*
rconv_file_extension(RconvFileType type);

char*
rconv_default_chart_format(RconvFileType type);

char*
rconv_substr(const char* str, size_t start, size_t end);

bool
rconv_parse_bool(char* str);

bool
rconv_is_whitespace(char str);

bool
rconv_is_number(char str);

char*
rconv_repeat(char* str, int amount);

void
rconv_free_all_strings(int len, char** strings);
