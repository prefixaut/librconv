#pragma once

#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <utf8.h>
#include <regex.h>

#include "list.h"

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

typedef enum {
	// Common token types, usually used for values
	RCONV_TOKEN_STRING,
	RCONV_TOKEN_INTEGER,
	RCONV_TOKEN_DECIMAL,

	// These stepmania-tokens work normally quite fine for nearly all properties.
	// However, attacks and notes are using their own format again which is worked around
	// with with token flags and context clues.
	RCONV_STEPMANIA_TOKEN_PROPERTY_START,			// <#>foobar:value11=value12,value21=value22;
	RCONV_STEPMANIA_TOKEN_VALUE_START,				// #foobar<:>value11=value12,value21=value22;
	RCONV_STEPMANIA_TOKEN_VALUE_SEPARATOR, 			// #foobar:value11<=>value12<,>value21<=>value22;
	RCONV_STEPMANIA_TOKEN_PROPERTY_END,				// #foobar:value11=value12,value21=value22<;>
	RCONV_STEPMANIA_TOKEN_NOTE,						// <0>/<1>/<M>/...
	RCONV_STEPMANIA_TOKEN_INLINE_ATTACK,			// 0<{attack}>
	RCONV_STEPMANIA_TOKEN_INLINE_KEYSOUND,			// 0<[keysound]>
	RCONV_STEPMANIA_TOKEN_LINE_COMMENT,				// <//content>
} RconvTokenType;

#define RCONV_TOKENFLAG_STRING_USED_QUOTES 1
#define RCONV_TOKENFLAG_STRING_USED_SINGLE_QUOTE 3

#define RCONV_TOKENFLAG_STEPMANIA_VALUE_SEPARATOR_COMMA 0
#define RCONV_TOKENFLAG_STEPMANIA_VALUE_SEPARATOR_EQUALS 1
#define RCONV_TOKENFLAG_STEPMANIA_VALUE_SEPARATOR_COLON 2

typedef struct {
	RconvTokenType type;
	int flags;
	void* content;
	size_t line;
	size_t column;
} RconvToken;

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
RCONV_LIST_TO_ARRAY_GEN_H_NAMED(RconvToken, token)

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

char*
rconv_trim(char* str);

bool
rconv_is_number(char str);

bool
rconv_is_integer_string(char* str);

bool
rconv_is_decimal_string(char* str);

RconvTokenType
rconv_detect_common_token_value_type(char* str);

char*
rconv_repeat(char* str, int amount);

void
rconv_free_all_strings(int len, char** strings);

void
rconv_print_token(RconvToken* token);
