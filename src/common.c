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
	} else if (token->type == RCONV_STEPMANIA_TOKEN_PROPERTY_NAME) {
		name = "sm:property-name";
	} else if (token->type == RCONV_STEPMANIA_TOKEN_VALUE_START) {
		name = "sm:value-start";
	} else if (token->type == RCONV_STEPMANIA_TOKEN_VALUE_SEPARATOR) {
		name = "sm:value-separator";
	} else if (token->type == RCONV_STEPMANIA_TOKEN_PROPERTY_END) {
		name = "sm:property-end";
	} else if (token->type == RCONV_STEPMANIA_TOKEN_ATTACK_VALUE_SEPARATOR) {
		name = "sm:attack-value-separator";
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
	printf("%s: %s (%s)\n", name, (char*) token->content, flags);
}
