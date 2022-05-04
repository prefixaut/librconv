#include "./stepmania.h"

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
rconv_stepmania_is_whitespace(char c)
{
	return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

char*
rconv_stepmania_parse_tags(char* data, char* tag, char* content, int offset)
{
	int state = 0;
	int size = strlen(data);
	int start = 0;

	while (offset < size) {
		if (state == 0) {
			if (data[offset] == '#') {
				state = 1;
				start = offset + 1;
			}
			offset++;
			continue;
		}

		if (state == 1) {
			if (data[offset] == ':') {
				int len = offset - start;
				*tag = malloc((len + 1) * sizeof(char));
				strncpy(tag, data + start, len);
				state = 2;
				start = offset + 1;
			}
			offset++;
			continue;
		}

		if (state == 2) {
			if (data[offset] == ';') {
				int len = offset - start;
				*content = malloc((len + 1) * sizeof(char));
				strncpy(content, data + start, len);
				offset++;
				break;
			}
			offset++;
		}
	}

	return offset;
}

bool
rconv_stepmania_is_yes(char* content)
{
	utf8lwr(content);
	return utf8cmp(content, "yes") == 0 || utf8cmp(content, "1") == 0 || utf8cmp(content, "es") == 0 || utf8cmp(content, "omes") == 0;
}

void
rconv_stepmania_parse_partial(char* data, int* index, char* content, int* offset, int* state)
{
	int size = strlen(data);
	int start = -1;

	while (offset < size) {
		if (state == 0) {
			if (rconv_stepmania_is_whitespace(data[offset])) {
				offset++;
				continue;
			}
			start = offset;
			*state = 1;
		}

		if (data[offset] == '=') {
			*state = 1;
		} else if (data[offset] == ',') {
			*state = 2;
			*index = 0;
		} else {
			*offset++;
			continue;
		}

		int len = offset - start;
		*content = malloc((len + 1) * sizeof(char));
		strncpy(content, data + start, len);
		*offset++;
	}

	if (start > -1) {
		int len = offset - start;
		*content = malloc((len + 1) * sizeof(char));
		strncpy(content, data + start, len);
	} else {
		*content = NULL;
	}

	*state = 2;
}

void
rconv_stepmania_parse_background_changes(char* data, int* len, RconvStepmaniaBackgroundChange* changes)
{
	RconvList* list = rconv_list();

	RconvStepmaniaBackgroundChange* change = (RconvStepmaniaBackgroundChange*) malloc(sizeof(RconvStepmaniaBackgroundChange));

	while (true) {
		rconv_stepmania_parse_partial(data, idx, content, offset, state);

		if (content == NULL) {
			break;
		}

		if (state == 2) {
			rconv_list_add(list, change);
			change = (RconvStepmaniaBackgroundChange*) malloc(sizeof(RconvStepmaniaBackgroundChange));
			state = 0;
			idx = 0;
			continue;
		}

		bool free_content = false;
		if (idx == 0) {
			has_data = true;
			mpd_set_string(change->beat, content, NULL);
			free_content = true;
		} else if (idx == 1) {
			change->path = content;
		} else if (idx == 2) {
			mpd_set_string(change->update_rate, content, NULL);
			free_content = true;
		} else if (idx == 3) {
			change->crossfade = rconv_parse_bool(content);
			free_content = true;
		} else if (idx == 4) {
			change->stretch_rewind = rconv_parse_bool(content);
			free_content = true;
		} else if (idx == 5) {
			change->stretch_no_loop = rconv_parse_bool(content);
			free_content = true;
		} else if (idx == 6) {
			change->effect = content;
		} else if (idx == 7) {
			change->file2 = content;
		} else if (idx == 8) {
			change->transition = content;
		} else if (idx == 9) {
			// TODO: parse color
		} else if (idx == 10) {
			// TODO: parse color
		}

		if (free_content) {
			free(content);
		}

		idx++;
	}

	*changes = (RconvStepmaniaBackgroundChange*) rconv_list_to_array(list, len);
	if (changes == -1) {
		*changes = NULL;
	}
	rconv_list_free(list);
}

void
rconv_stepmania_parse_stops(char* data, int* len, RconvStepmaniaStop* stops)
{
	RconvList* list = rconv_list();

	RconvStepmaniaStop* stop = (RconvStepmaniaStop*) malloc(sizeof(RconvStepmaniaStop));

	int state = 0;
	int idx = 0;
	int start = 0;
	int offset = 0;
	char* content = NULL;

	while (true) {
		rconv_stepmania_parse_partial(data, idx, content, offset, state);

		if (content == NULL) {
			break;
		}

		if (state == 2) {
			rconv_list_add(list, stop);
			stop = (RconvStepmaniaStop*) malloc(sizeof(RconvStepmaniaStop));
			state = 0;
			idx = 0;
			continue;
		}

		if (idx == 0) {
			mpd_set_string(stop->beat, content, NULL);
		} else if (idx == 1) {
			mpd_set_string(stop->duration, content, NULL);
		}

		free(content);

		idx++;
	}

	*stops = (RconvStepmaniaStop*) rconv_list_to_array(list, len);
	if (stops == -1) {
		*stops = NULL;
	}
	rconv_list_free(list);
}

void
rconv_stepmania_parse_bpms(char* data, int* len, RconvStepmaniaBpmChange* bpms)
{
	RconvList* list = rconv_list();

	RconvStepmaniaBpmChange* change = (RconvStepmaniaBpmChange*) malloc(sizeof(RconvStepmaniaStop));

	int state = 0;
	int idx = 0;
	int start = 0;
	int offset = 0;
	char* content = NULL;

	while (true) {
		rconv_stepmania_parse_partial(data, idx, content, offset, state);

		if (content == NULL) {
			break;
		}

		if (state == 2) {
			rconv_list_add(list, change);
			change = (RconvStepmaniaBpmChange*) malloc(sizeof(RconvStepmaniaBpmChange));
			state = 0;
			idx = 0;
			continue;
		}

		if (idx == 0) {
			mpd_set_string(change->beat, content, NULL);
		} else if (idx == 1) {
			mpd_set_string(change->bpm, content, NULL);
		}

		free(content);

		idx++;
	}

	*bpms = (RconvStepmaniaBpmChange*) rconv_list_to_array(list, len);
	if (bpms == -1) {
		*bpms = NULL;
	}
	rconv_list_free(list);
}

void
rconv_stepmania_parse_time_signatures(char* data, int* len, RconvStepmaniaTimeSignature* signatures)
{
	RconvList* list = rconv_list();

	RconvStepmaniaTimeSignature* sig = (RconvStepmaniaTimeSignature*) malloc(sizeof(RconvStepmaniaStop));

	int state = 0;
	int idx = 0;
	int start = 0;
	int offset = 0;
	char* content = NULL;

	while (true) {
		rconv_stepmania_parse_partial(data, idx, content, offset, state);

		if (content == NULL) {
			break;
		}

		if (state == 2) {
			rconv_list_add(list, sig);
			sig = (RconvStepmaniaTimeSignature*) malloc(sizeof(RconvStepmaniaTimeSignature));
			state = 0;
			idx = 0;
			continue;
		}

		if (idx == 0) {
			mpd_set_string(sig->beat, content, NULL);
		} else if (idx == 1) {
			sig->numerator = atoi(content);
		} else if (idx == 2) {
			sig->denominator = atoi(content);
		}

		free(content);

		idx++;
	}

	*signatures = (RconvStepmaniaTimeSignature*) rconv_list_to_array(list, len);
	if (signatures == -1) {
		*signatures = NULL;
	}
	rconv_list_free(list);
}

RconvStepmaniaChartFile*
rconv_stepmania_parse(char* data)
{
	RconvStepmaniaChartFile* chart = (RconvStepmaniaChartFile*) malloc(sizeof(RconvStepmaniaChartFile));
	int offset = 0;
	char* tag = NULL;
	char* content = NULL;

	while (true) {
		offset = rconv_stepmania_parse_tags(data, tag, content, offset);
		if (tag == NULL || content == NULL) {
			if (tag != NULL) {
				free(tag);
			}
			if (content != NULL) {
				free(content);
			}
			break;
		}

		bool free_content = true;
		if (utf8cmp(tag, "title") == 0) {
			chart->title = content;
			free_content = false;
		} else if (utf8cmp(tag, "subtitle") == 0) {
			chart->subtitle = content;
			free_content = false;
		} else if (utf8cmp(tag, "artist") == 0) {
			chart->artist = content;
			free_content = false;
		} else if (utf8cmp(tag, "titletranslit") == 0) {
			chart->title_transliterated = content;
			free_content = false;
		} else if (utf8cmp(tag, "subtitletranslit") == 0) {
			chart->subtitle_transliterated = content;
			free_content = false;
		} else if (utf8cmp(tag, "artisttranslit") == 0) {
			chart->artist_transliterated = content;
			free_content = false;
		} else if (utf8cmp(tag, "genre") == 0) {
			chart->genre = content;
			free_content = false;
		} else if (utf8cmp(tag, "credit") == 0) {
			chart->credit = content;
			free_content = false;
		} else if (utf8cmp(tag, "banner") == 0) {
			chart->banner = content;
			free_content = false;
		} else if (utf8cmp(tag, "background") == 0) {
			chart->background = content;
			free_content = false;
		} else if (utf8cmp(tag, "lyricspath") == 0) {
			chart->lyrics = content;
			free_content = false;
		} else if (utf8cmp(tag, "cdtitle") == 0) {
			chart->cd_title = content;
			free_content = false;
		} else if (utf8cmp(tag, "music") == 0) {
			chart->music = content;
			free_content = false;
		} else if (utf8cmp(tag, "displaybmp") == 0) {
			chart->display_bpm = content;
			free_content = false;
		} else if (utf8cmp(tag, "samplestart") == 0) {
			mpd_set_string(chart->sample_start, content, NULL);
		} else if (utf8cmp(tag, "samplelength") == 0) {
			mpd_set_string(chart->sample_length, content, NULL);
		} else if (utf8cmp(tag, "offset") == 0) {
			mpd_set_string(chart->offset, content, NULL);
		} else if (utf8cmp(tag, "selectable") == 0) {
			chart->selectable = rconv_stepmania_is_yes(content);
		} else if (utf8cmp(tag, "bgchanges") == 0) {
			rconv_stepmania_parse_background_changes(content, chart->background_changes_len, chart->background_changes);
		} else if (utf8cmp(tag, "bgchanges2") == 0) {
			rconv_stepmania_parse_background_changes(content, chart->background_changes2_len, chart->background_changes2);
		} else if (utf8cmp(tag, "bgchanges3") == 0) {
			rconv_stepmania_parse_background_changes(content, chart->background_changes3_len, chart->background_changes3);
		} else if (utf8cmp(tag, "animations") == 0) {
			rconv_stepmania_parse_background_changes(content, chart->animations_len, chart->animations);
		} else if (utf8cmp(tag, "fgchanges") == 0) {
			rconv_stepmania_parse_background_changes(content, chart->foreground_changes_len, chart->foreground_changes);
		} else if (utf8cmp(tag, "keysounds") == 0) {
			RconvList* list = rconv_list();

			int offset = 0;
			int size = strlen(content);
			int start = 0;
			int has_content = false;

			while (content < size) {
				if (content[offset] == ',') {
					if (!has_content) {
						continue;
					}

					int len = offset - start;
					char* elem = malloc((len + 1) * sizeof(char));
					strncpy(elem, content + start, len);
					rconv_list_add(list, elem);
					start = offset + 1;
					offset++;
					has_content = false;
					continue;
				}

				if (!rconv_stepmania_is_whitespace(content[offset]) && !has_content) {
					has_content = true;
					start = offset;
				}
				offset++;
			}

			if (has_content) {
				int len = offset - start;
				char* elem = malloc((len + 1) * sizeof(char));
				strncpy(elem, content + start, len);
				rconv_list_add(list, elem);
			}

			*chart->key_sounds = rconv_list_to_array(list, chart->key_sounds_len);
			if (chart->key_sounds == -1) {
				*chart->key_sounds = NULL;
			}
			rconv_list_free(list);
		} else if (utf8cmp(tag, "stops") == 0 || utf8cmp(tag, "freezes") == 0) {
			rconv_stepmania_parse_stops(content, chart->stops_len, chart->stops);
		} else if (utf8cmp(tag, "bpms") == 0) {
			rconv_stepmania_parse_bpms(content, chart->bpms_len, chart->bpms);
		} else if (utf8cmp(tag, "timesignatures") == 0) {
		}

		free(tag);
		if (free_content) {
			free(content);
		}
	}
}
