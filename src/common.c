#include "./common.h"

RCONV_LIST_TO_ARRAY_GEN_NAMED(RconvToken, token)

bool
rconv_parse_bool(char* str)
{
	if (str == NULL) {
		return false;
	}

	utf8lwr(str);
	return strcmp(str, "y") == 0 || strcmp(str, "yes") == 0 || strcmp(str, "true") == 0 || strcmp(str, "1") || strcmp(str, "on");
}

char*
rconv_substr(const char* str, size_t start, size_t end)
{
	long long len = end - start;
	if (len <= 0) {
		return NULL;
	}
	// len+1 for the null-byte at the end
	size_t size = (len + 1) * sizeof(char);
	char* out = (char*) malloc(size);
	if (out == NULL) {
		return NULL;
	}
	strncpy(out, str + start, len);
	out[len] = '\0';

	return out;
}

bool
rconv_is_whitespace(char c)
{
	return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

/**
 * Shamelessly stolen from stack-overflow
 * https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way
 */
char*
rconv_trim(char* str)
{
	char *end;

	// Trim leading space
	while(isspace((unsigned char) *str)) {
		str++;
	}

	if(*str == 0) {
		return str;
	}

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace((unsigned char) *end)) {
		end--;
	}

	// Write new null terminator character
	end[1] = '\0';

	return str;
}

bool
rconv_is_number(char c)
{
	return c >= 48 && c <= 57;
}

bool
rconv_is_integer_string(char* str)
{
	if (str == NULL) {
		return false;
	}

	size_t len = strlen(str);
	for (size_t i = 0; i < len; i++) {
		// Allow + or - prefix for numbers
		if (i == 0 && (str[i] == '+' || str[i] == '-')) {
			continue;
		}

		if (!rconv_is_number(str[i])) {
			return false;
		}
	}

	return true;
}

bool
rconv_is_decimal_string(char* str)
{
	if (str == NULL) {
		return false;
	}

	size_t len = strlen(str);
	bool had_point = false;

	if (len == 1 && str[0] == '.') {
		return false;
	}

	for (size_t i = 0; i < len; i++) {
		// Allow + or - prefix for numbers
		if (i == 0 && (str[i] == '+' || str[i] == '-')) {
			continue;
		}

		if (str[i] == '.') {
			if (had_point) {
				return false;
			}
			had_point = true;
			continue;
		}

		if (!rconv_is_number(str[i])) {
			return false;
		}
	}

	return true;
}

RconvTokenType
rconv_detect_common_token_value_type(char* str)
{
	size_t len = strlen(str);
	bool had_point = false;
    bool had_number = false;
    RconvTokenType type = RCONV_TOKEN_INTEGER;

	for (size_t i = 0; i < len; i++) {
		// Allow + or - prefix for numbers
		if (i == 0 && (str[i] == '+' || str[i] == '-')) {
			continue;
		}

		if (str[i] == '.') {
			if (had_point) {
                type = RCONV_TOKEN_STRING;
                break;
			}
            type = RCONV_TOKEN_DECIMAL;
			had_point = true;
			continue;
		}

		if (!rconv_is_number(str[i])) {
            type = RCONV_TOKEN_STRING;
            break;
		}
        had_number = true;
	}

    if (had_point && !had_number) {
        type = RCONV_TOKEN_STRING;
    }

    return type;
}

char*
rconv_repeat(char* str, int amount)
{
	size_t mem_len = strlen(str);
	int size = mem_len * amount + 1;
	char* out = calloc(size, sizeof(char));

	for (int i = 0; i < amount; i++) {
		void* pos = out + (i * mem_len);
		if (pos == NULL) {
			free(out);
			out = NULL;
			break;
		}
		memcpy(pos, str, mem_len);
	}

	return out;
}

char*
rconv_file_extension(RconvFileType type)
{
	// TODO: implement me
	return NULL;
}

void
rconv_free_all_strings(int len, char** strings)
{
	for (int i = 0; i < len; i++) {
		free(strings + i);
	}
	free(strings);
}

char*
_rconv_token_name(RconvToken* token) {
	char* name;

	if (token->type == RCONV_TOKEN_STRING) {
		name = "string";
	} else if (token->type == RCONV_TOKEN_INTEGER) {
		name = "integer";
	} else if (token->type == RCONV_TOKEN_DECIMAL) {
		name = "decimal";
	} else if (token->type == RCONV_STEPMANIA_TOKEN_PROPERTY_START) {
		name = "sm:property-start";
	} else if (token->type == RCONV_STEPMANIA_TOKEN_VALUE_START) {
		name = "sm:value-start";
	} else if (token->type == RCONV_STEPMANIA_TOKEN_VALUE_SEPARATOR) {
		name = "sm:value-separator";
	} else if (token->type == RCONV_STEPMANIA_TOKEN_PROPERTY_END) {
		name = "sm:property-end";
	} else if (token->type == RCONV_STEPMANIA_TOKEN_NOTE) {
		name = "sm:note";
	} else if (token->type == RCONV_STEPMANIA_TOKEN_INLINE_ATTACK) {
		name = "sm:inline-attack";
	} else if (token->type == RCONV_STEPMANIA_TOKEN_INLINE_KEYSOUND) {
		name = "sm:inline-keysound";
	} else if (token->type == RCONV_STEPMANIA_TOKEN_LINE_COMMENT) {
		name = "sm:comment-line";
	} else {
		name = "unknown";
	}
	
	return name;
}

char*
_rconv_token_flags(RconvToken* token)
{
	char* flags;

	if (token->type == RCONV_TOKEN_STRING) {
		if ((token->flags & RCONV_TOKENFLAG_STRING_USED_QUOTES) > 0) {
			if ((token->flags & RCONV_TOKENFLAG_STRING_USED_SINGLE_QUOTE) > 0) {
				flags = "wrapped in single quotes";
			} else {
				flags = "wrapped in double quotes";
			}
		}
	} else if (token->type == RCONV_STEPMANIA_TOKEN_VALUE_SEPARATOR) {
		if (token->flags == RCONV_TOKENFLAG_STEPMANIA_VALUE_SEPARATOR_EQUALS) {
			flags = "equals";
		} else if (token->flags == RCONV_TOKENFLAG_STEPMANIA_VALUE_SEPARATOR_COLON) {
			flags = "colon";
		} else {
			flags = "comma";
		}
	}

	return flags;
}

void
rconv_print_token(RconvToken* token)
{
	char* name = _rconv_token_name(token);
	char* flags = _rconv_token_flags(token);
	printf("%ld:%ld => %s: '%s' (%s)\n", token->line, token->column, name, (char*) token->content, flags);
}
