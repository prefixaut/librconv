#include "stepmania.h"

RconvFormattingParameters
rconv_stepmania_formatting_parameters(RconvStepmaniaChartFile* chart)
{
	RconvFormattingParameters params = {
		.title = chart->title,
		.artist = chart->artist,
		.difficulty = NULL,
		.extension = rconv_file_extension(STEPMANIA),
		.mode = NULL,
		.level = NULL,
	};
	return params;
}

bool
_rconv_stepmania_is_yes(char* content)
{
	utf8lwr(content);
	return utf8cmp(content, "yes") == 0 || utf8cmp(content, "1") == 0 || utf8cmp(content, "es") == 0 || utf8cmp(content, "omes") == 0;
}

char*
_rconv_stepmania_copy_token_content(RconvToken* token)
{
	char* copy = malloc(sizeof(char) * (strlen((char*) token->content) + 1));
	strcpy(copy, (char*) token->content);
	return copy;
}

char*
_rconv_stepmania_parse_string_content(RconvToken** tokens, int token_len, int* token_idx)
{
	RconvList* list = rconv_list();
	size_t content_len = 0;
	bool did_update = false;

	for (; *token_idx < token_len; (*token_idx)++) {
		RconvToken* token = tokens[*token_idx];
		if (token->type == RCONV_TOKEN_EOF || token->type == RCONV_STEPMANIA_TOKEN_PROPERTY_END) {
			break;
		}
		// Only type which could occur during the value part
		if (token->type == RCONV_STEPMANIA_TOKEN_LINE_COMMENT) {
			continue;
		}
		if (token->type != RCONV_TOKEN_DECIMAL
			&& token->type != RCONV_TOKEN_INTEGER
			&& token->type != RCONV_TOKEN_STRING
			&& token->type != RCONV_STEPMANIA_TOKEN_VALUE_SEPARATOR) {
			// TODO: Handle invalid token?
			rconv_list_free(list);
			return "";
		}
		char* s = (char*) token->content;
		if (s == NULL) {
			continue;
		}
		size_t l = strlen(s);
		content_len += l;
		rconv_list_add(list, token);
		did_update = true;
	}

	char* content = calloc(content_len + 1, sizeof(char));
	size_t content_offset = 0;
	RconvListEntry* e = list->head;

	if (content_len != 0) {
		while (e != NULL) {
			char* token_content = (char*) ((RconvToken*) e->value)->content;
			size_t len = strlen(token_content);
			memcpy(content + content_offset, token_content, len);
			content_offset += len;
			e = e->next;
		}
	}

	rconv_list_free(list);
	content[content_len] = '\0';
	return content;
}

RconvDecimal*
_rconv_stepmania_parse_decimal_content(RconvToken** tokens, int token_len, int* token_idx)
{
	char* content = NULL;

	for (; *token_idx < token_len; (*token_idx)++) {
		RconvToken* token = tokens[*token_idx];
		if (token->type == RCONV_TOKEN_EOF || token->type == RCONV_STEPMANIA_TOKEN_PROPERTY_END) {
			break;
		}
		// Only type which could occur during the value part
		if (token->type == RCONV_STEPMANIA_TOKEN_LINE_COMMENT) {
			continue;
		}
		if (token->type != RCONV_TOKEN_DECIMAL && token->type != RCONV_TOKEN_INTEGER) {
			// TODO: Handle invalid token?
			printf("invaid token for decimals found!\n");
			rconv_print_token(token);
			break;
		}
		content = (char*) token->content;
		printf("found decimal content: %s\n", content);
		break;
	}

	if (content == NULL) {
		printf("NO CONTENT for decimal parsing!\n");
		return NULL;
	}

	return rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
}

void
_rconv_stepmania_parse_property(
	RconvStepmaniaChartFile* chart,
	RconvToken** tokens,
	int token_len,
	int* token_idx
) {
	char* property_name = (char*) tokens[(*token_idx)++]->content;
	printf("Handling property '%s'\n", property_name);

	if (*token_idx > token_len || tokens[*token_idx]->type != RCONV_STEPMANIA_TOKEN_VALUE_START) {
		// TODO: Handle invalid token
		return;
	}

	// Skip the value-start token
	(*token_idx)++;

	if (strcmp(property_name, "title") == 0) {
		chart->title = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
	} else if (strcmp(property_name, "subtitle") == 0) {
		chart->subtitle = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
	} else if (strcmp(property_name, "artist") == 0) {
		chart->artist = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
	} else if (strcmp(property_name, "titletranslit") == 0) {
		chart->title_transliterated = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
	} else if (strcmp(property_name, "subtitletranslit") == 0) {
		chart->subtitle_transliterated = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
	} else if (strcmp(property_name, "artisttranslit") == 0) {
		chart->artist_transliterated = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
	} else if (strcmp(property_name, "genre") == 0) {
		chart->genre = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
	} else if (strcmp(property_name, "credit") == 0) {
		chart->credit = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
	} else if (strcmp(property_name, "banner") == 0) {
		chart->banner = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
	} else if (strcmp(property_name, "background") == 0) {
		chart->background = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
	} else if (strcmp(property_name, "lyrics") == 0) {
		chart->lyrics = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
	} else if (strcmp(property_name, "cdtitle") == 0) {
		chart->cd_title = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
	} else if (strcmp(property_name, "music") == 0) {	
		chart->music = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
	} else if (strcmp(property_name, "samplestart") == 0) {
		chart->sample_start = _rconv_stepmania_parse_decimal_content(tokens, token_len, token_idx);
	} else if (strcmp(property_name, "samplelength") == 0) {
		chart->sample_length = _rconv_stepmania_parse_decimal_content(tokens, token_len, token_idx);
	} else if (strcmp(property_name, "offset") == 0) {
		chart->offset = _rconv_stepmania_parse_decimal_content(tokens, token_len, token_idx);
	// } else if (strcmp(property_name, "selectable") == 0) {
		// char* tmp = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
		// if (tmp != NULL) {
		// 	chart->selectable = _rconv_stepmania_is_yes(tmp);
		// 	free(tmp);
		// } else {
		// 	// TODO: Define default value ??
		// 	chart->selectable = true;
		// }
	} else {
		// printf("else: '%s'\n", property_name);
		while (*token_idx < token_len) {
			RconvTokenType type = tokens[*token_idx]->type;
			// rconv_print_token(tokens[*token_idx]);
			*token_idx += 1;
			if (type == RCONV_STEPMANIA_TOKEN_PROPERTY_END || type == RCONV_TOKEN_EOF) {
				break;
			}
		}
	}
}

RconvStepmaniaChartFile*
rconv_stepmania_parse_from_tokens(RconvToken** tokens, int token_len)
{
	if (tokens == NULL || token_len == 0) {
		return NULL;
	}

	RconvStepmaniaChartFile* chart = (RconvStepmaniaChartFile*) calloc(1, sizeof(RconvStepmaniaChartFile));
	if (chart == NULL) {
		return NULL;
	}

	// TODO: Token validation?

	int start = 0;

	for (int idx = 0; idx < token_len; idx++) {
		RconvToken* token = tokens[idx];
		if (token->type != RCONV_STEPMANIA_TOKEN_PROPERTY_START) {
			continue;
		}

		idx++;
		_rconv_stepmania_parse_property(chart, tokens, token_len, &idx);

		// Free all processed tokens
		for (int tmp = start; tmp <= idx && tmp < token_len; tmp++) {
			RconvToken* free_me = tokens[tmp];
			if (free_me == NULL) {
				continue;
			}
			if (free_me->content != NULL) {
				free(free_me->content);
			}
			free(free_me);
		}

		start = idx + 1;
	}

	return chart;
}

RconvStepmaniaChartFile*
rconv_stepmania_parse(RconvDataDescriptor* dd)
{
	int token_len = 0;
	RconvToken** tokens = rconv_stepmania_tokenize(dd, &token_len);

	return rconv_stepmania_parse_from_tokens(tokens, token_len);
}

RconvStepmaniaChartFile*
rconv_stepmania_parse_from_string(char* data)
{
	RconvDataDescriptor* dd = rconv_dd_new_from_string(data);
	if (dd == NULL) {
		return NULL;
	}

	RconvStepmaniaChartFile* chart = rconv_stepmania_parse(dd);
	rconv_dd_free(dd);

	return chart;
}

RconvStepmaniaChartFile*
rconv_stepmania_parse_from_file(char* file_path)
{
	if (file_path == NULL) {
		return NULL;
	}

	FILE* fp = fopen(file_path, "r");
	if (fp == NULL) {
		return NULL;
	}

	RconvDataDescriptor* dd = rconv_dd_new_from_file(fp);
	if (dd == NULL) {
		return NULL;
	}

	RconvStepmaniaChartFile* chart = rconv_stepmania_parse(dd);
	rconv_dd_free(dd);

	return chart;
}
