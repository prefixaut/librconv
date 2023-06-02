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

RconvStepmaniaChartFile*
rconv_stepmania_parse_from_tokens(RconvToken* tokens, int token_len)
{
	RconvStepmaniaChartFile* chart = (RconvStepmaniaChartFile*) calloc(1, sizeof(RconvStepmaniaChartFile));
	if (chart == NULL) {
		return NULL;
	}

	for (int i = 0; i < token_len; i++) {

	}

	return chart;
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

	for (; *token_idx < token_len; (*token_idx)++) {
		RconvToken* token = tokens[*token_idx];
		if (token->type == RCONV_TOKEN_EOF || token->type == RCONV_STEPMANIA_TOKEN_PROPERTY_END) {
			printf("VALUE END (%d)\n", *token_idx);
			break;
		}
		// Only type which could occur during the value part
		if (token->type == RCONV_STEPMANIA_TOKEN_LINE_COMMENT) {
			continue;
		}
		char* s = (char*) token->content;
		if (s == NULL) {
			continue;
		}
		size_t l = strlen(s);
		content_len += l;
		printf("VALUE '%s' (%ld), len: %ld\n", s, l, content_len);
		rconv_list_add(list, token);
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
	printf("parsed string value: '%s'\n", content);
	content[content_len] = '\0';
	return content;
}

void
_rconv_stepmania_parse_property(
	RconvStepmaniaChartFile* chart,
	RconvToken** tokens,
	int token_len,
	int* token_idx
) {
	char* property_name = (char*) tokens[*token_idx]->content;
	*token_idx += 1;
	RconvToken* content_token = tokens[*token_idx + 2];

	// rconv_print_token(content_token);
	printf("parsing property: %s\n", property_name);

	if (strcmp(property_name, "title") == 0) {
		printf("parsing title\n");
		chart->title = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
		*token_idx += 1;
	} else if (strcmp(property_name, "subtitle") == 0) {
		printf("parsing subtitle\n");
		chart->subtitle = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
		*token_idx += 1;
	} else if (strcmp(property_name, "artist") == 0) {
		printf("parsing artist\n");
		chart->artist = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
		*token_idx += 1;
	} else if (strcmp(property_name, "titletranslit") == 0) {
		chart->title_transliterated = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
		*token_idx += 1;
	} else if (strcmp(property_name, "subtitletranslit") == 0) {
		chart->subtitle_transliterated = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
		*token_idx += 1;
	} else if (strcmp(property_name, "artisttranslit") == 0) {
		chart->artist_transliterated = _rconv_stepmania_parse_string_content(tokens, token_len, token_idx);
		*token_idx += 1;
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
	// printf("next token: %d from %d\n", *token_idx, token_len);
}

RconvStepmaniaChartFile*
rconv_stepmania_parse(RconvDataDescriptor* dd)
{
	int token_len = 0;
	RconvToken** tokens = rconv_stepmania_tokenize(dd, &token_len);

	if (tokens == NULL || token_len == 0) {
		return NULL;
	}

	// TODO: Token validation?

	RconvStepmaniaChartFile* chart = (RconvStepmaniaChartFile*) calloc(1, sizeof(RconvStepmaniaChartFile));
	if (chart == NULL) {
		return NULL;
	}

	int start = 0;

	for (int idx = 0; idx < token_len; idx++) {
		RconvToken* token = tokens[idx];
		if (token->type != RCONV_STEPMANIA_TOKEN_PROPERTY_START) {
			continue;
		}

		idx++;
		_rconv_stepmania_parse_property(chart, tokens, token_len, &idx);

		// Free all processed tokens
		// printf("free tokens from %d-%d\n", start, idx);
		for (int tmp = start; tmp <= idx && tmp < token_len; tmp++) {
			RconvToken* free_me = tokens[tmp];
			// printf("free token start: %d\n", tmp);
			// fflush(stdout);
			// rconv_print_token(free_me);
			if (free_me == NULL) {
				continue;
			}
			if (free_me->content != NULL) {
				// printf("free token content '%s' (%p)\n", (char*) free_me->content, free_me->content);
				// fflush(stdout);
				free(free_me->content);
			}
			// printf("free token\n");
			// fflush(stdout);
			free(free_me);
			// printf("free token finish: %d\n", tmp);
			// fflush(stdout);
		}

		start = idx + 1;
	}

	// char* tag = NULL;
	// char* content = NULL;
	// RconvList* notes = rconv_list();

	// while (true) {
	// 	rconv_stepmania_parse_tags(dd, &tag, &content);
	// 	if (tag == NULL) {
	// 		if (content != NULL) {
	// 			free(content);
	// 		}
	// 		break;
	// 	}

	// 	utf8lwr(tag);

	// 	bool free_content = true;
	// 	if (utf8cmp(tag, "title") == 0) {
	// 		free(chart->title);
	// 		chart->title = content;
	// 		free_content = false;
	// 	} else if (utf8cmp(tag, "subtitle") == 0) {
	// 		free(chart->subtitle);
	// 		chart->subtitle = content;
	// 		free_content = false;
	// 	} else if (utf8cmp(tag, "artist") == 0) {
	// 		free(chart->artist);
	// 		chart->artist = content;
	// 		free_content = false;
	// 	} else if (utf8cmp(tag, "titletranslit") == 0) {
	// 		free(chart->title_transliterated);
	// 		chart->title_transliterated = content;
	// 		free_content = false;
	// 	} else if (utf8cmp(tag, "subtitletranslit") == 0) {
	// 		free(chart->subtitle_transliterated);
	// 		chart->subtitle_transliterated = content;
	// 		free_content = false;
	// 	} else if (utf8cmp(tag, "artisttranslit") == 0) {
	// 		free(chart->artist_transliterated);
	// 		chart->artist_transliterated = content;
	// 		free_content = false;
	// 	} else if (utf8cmp(tag, "genre") == 0) {
	// 		free(chart->genre);
	// 		chart->genre = content;
	// 		free_content = false;
	// 	} else if (utf8cmp(tag, "credit") == 0) {
	// 		free(chart->credit);
	// 		chart->credit = content;
	// 		free_content = false;
	// 	} else if (utf8cmp(tag, "banner") == 0) {
	// 		free(chart->banner);
	// 		chart->banner = content;
	// 		free_content = false;
	// 	} else if (utf8cmp(tag, "background") == 0) {
	// 		free(chart->background);
	// 		chart->background = content;
	// 		free_content = false;
	// 	} else if (utf8cmp(tag, "lyricspath") == 0) {
	// 		free(chart->lyrics);
	// 		chart->lyrics = content;
	// 		free_content = false;
	// 	} else if (utf8cmp(tag, "cdtitle") == 0) {
	// 		free(chart->cd_title);
	// 		chart->cd_title = content;
	// 		free_content = false;
	// 	} else if (utf8cmp(tag, "music") == 0) {
	// 		free(chart->music);
	// 		chart->music = content;
	// 		free_content = false;
	// 	} else if (utf8cmp(tag, "displaybmp") == 0) {
	// 		free(chart->display_bpm);
	// 		chart->display_bpm = content;
	// 		free_content = false;
	// 	} else if (utf8cmp(tag, "samplestart") == 0) {
	// 		free(chart->sample_start);
	// 		chart->sample_start = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	// 	} else if (utf8cmp(tag, "samplelength") == 0) {
	// 		free(chart->sample_length);
	// 		chart->sample_length = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	// 	} else if (utf8cmp(tag, "offset") == 0) {
	// 		free(chart->offset);
	// 		chart->offset = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	// 	} else if (utf8cmp(tag, "selectable") == 0) {
	// 		chart->selectable = _rconv_stepmania_is_yes(content);
	// 	} else if (utf8cmp(tag, "bgchanges") == 0) {
	// 		rconv_stepmania_free_all_background_changes(chart->background_changes_len, chart->background_changes);
	// 		chart->background_changes =
	// 			rconv_stepmania_parse_background_changes(content, &chart->background_changes_len);
	// 	} else if (utf8cmp(tag, "bgchanges2") == 0) {
	// 		rconv_stepmania_free_all_background_changes(chart->background_changes2_len, chart->background_changes2);
	// 		chart->background_changes2 =
	// 			rconv_stepmania_parse_background_changes(content, &chart->background_changes2_len);
	// 	} else if (utf8cmp(tag, "bgchanges3") == 0) {
	// 		rconv_stepmania_free_all_background_changes(chart->background_changes3_len, chart->background_changes3);
	// 		chart->background_changes3 =
	// 			rconv_stepmania_parse_background_changes(content, &chart->background_changes3_len);
	// 	} else if (utf8cmp(tag, "animations") == 0) {
	// 		rconv_stepmania_free_all_background_changes(chart->animations_len, chart->animations);
	// 		chart->animations = rconv_stepmania_parse_background_changes(content, &chart->animations_len);
	// 	} else if (utf8cmp(tag, "fgchanges") == 0) {
	// 		rconv_stepmania_free_all_background_changes(chart->foreground_changes_len, chart->foreground_changes);
	// 		chart->foreground_changes =
	// 			rconv_stepmania_parse_background_changes(content, &chart->foreground_changes_len);
	// 	} else if (utf8cmp(tag, "keysounds") == 0) {
	// 		rconv_free_all_strings(chart->key_sounds_len, chart->key_sounds);
	// 		chart->key_sounds = rconv_stepmania_parse_string_list(content, &chart->key_sounds_len);
	// 	} else if (utf8cmp(tag, "stops") == 0 || utf8cmp(tag, "freezes") == 0) {
	// 		rconv_stepmania_free_all_stops(chart->stops_len, chart->stops);
	// 		chart->stops = rconv_stepmania_parse_stops(content, &chart->stops_len);
	// 	} else if (utf8cmp(tag, "bpms") == 0) {
	// 		rconv_stepmania_free_all_bpm_changes(chart->bpms_len, chart->bpms);
	// 		chart->bpms = rconv_stepmania_parse_bpm_changes(content, &chart->bpms_len);
	// 	} else if (utf8cmp(tag, "timesignatures") == 0) {
	// 		rconv_stepmania_free_all_time_signatures(chart->time_signatures_len, chart->time_signatures);
	// 		chart->time_signatures = rconv_stepmania_parse_time_signatures(content, &chart->time_signatures_len);
	// 	} else if (utf8cmp(tag, "instrumenttracks") == 0) {
	// 		rconv_stepmania_free_all_instrument_tracks(chart->instrument_tracks_len, chart->instrument_tracks);
	// 		chart->instrument_tracks =
	// 			rconv_stepmania_parse_instrument_tracks(content, &chart->instrument_tracks_len);
	// 	} else if (utf8cmp(tag, "attacks") == 0) {
	// 		rconv_stepmania_free_all_timed_attacks(chart->attacks_len, chart->attacks);
	// 		chart->attacks = rconv_stepmania_parse_timed_attacks(content, &chart->attacks_len);
	// 	} else if ((utf8cmp(tag, "delays") == 0) || (utf8cmp(tag, "warps") == 0)) {
	// 		rconv_stepmania_free_all_delays(chart->delays_len, chart->delays);
	// 		chart->delays = rconv_stepmania_parse_delays(content, &chart->delays_len);
	// 	} else if (utf8cmp(tag, "tickcounts") == 0) {
	// 		rconv_stepmania_free_all_tick_counts(chart->tick_counts_len, chart->tick_counts);
	// 		chart->tick_counts = rconv_stepmania_parse_tick_counts(content, &chart->tick_counts_len);
	// 	} else if (utf8cmp(tag, "notes") == 0) {
	// 		rconv_stepmania_parse_note_data(content, notes);
	// 	} else if (utf8cmp(tag, "combos") == 0) {
	// 		rconv_stepmania_free_all_combo_changes(chart->combos_len, chart->combos);
	// 		chart->combos = rconv_stepmania_parse_combo_changes(content, &chart->combos_len);
	// 	} else if (utf8cmp(tag, "speeds") == 0) {
	// 		rconv_stepmania_free_all_speed_changes(chart->speeds_len, chart->speeds);
	// 		chart->speeds = rconv_stepmania_parse_speed_changes(content, &chart->speeds_len);
	// 	} else if (utf8cmp(tag, "scrolls") == 0) {
	// 		rconv_stepmania_free_all_scroll_speed_changes(chart->scrolls_len, chart->scrolls);
	// 		chart->scrolls = rconv_stepmania_parse_scroll_speed_changes(content, &chart->scrolls_len);
	// 	} else if (utf8cmp(tag, "fakes") == 0) {
	// 		rconv_stepmania_free_all_fake_sections(chart->fakes_len, chart->fakes);
	// 		chart->fakes = rconv_stepmania_parse_fake_sections(content, &chart->fakes_len);
	// 	} else if (utf8cmp(tag, "labels") == 0) {
	// 		rconv_stepmania_free_all_labels(chart->labels_len, chart->labels);
	// 		chart->labels = rconv_stepmania_parse_labels(content, &chart->labels_len);
	// 	} else {
	// 		printf("unknown tag '%s'!\n", tag);
	// 	}

	// 	free(tag);
	// 	if (free_content) {
	// 		free(content);
	// 	}
	// }

	// chart->note_data = rconv_list_to_note_data_array(notes, &chart->note_data_len);
	// rconv_list_free(notes);

	return chart;
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
