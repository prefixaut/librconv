#include "./stepmania.h"

RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaBpmChange)
RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaStop)
RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaDelay)
RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaTimeSignature)
RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaInstrumentTrack)
RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaTickCount)
RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaBackgroundChange)
RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaModifier)
RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaAttack)
RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaTimedAttack)
RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaComboChange)
RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaSpeedChange)
RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaScrollSpeedChange)
RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaFakeSection)
RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaLabel)
RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaNote)
RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaBeat)
RCONV_LIST_TO_ARRAY_GEN(RconvStepmaniaNoteData)

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

void
rconv_stepmania_parse_tags(char** data, char** tag, char** content)
{
	int offset = 0;
	int state = 0;
	size_t size = strlen(*data);
	int start = 0;

	while (offset < size) {
		if (state == 0) {
			if (*(*data + offset) == '#') {
				state = 1;
				start = offset + 1;
			}
			offset++;
			continue;
		}

		if (state == 1) {
			if (*(*data + offset) == ':') {
				int len = offset - start;
				*tag = (char*) malloc((len + 1) * sizeof(char));
				strncpy_s(*tag, len + 1, *data + start, len);
				state = 2;
				start = offset + 1;
			}
			offset++;
			continue;
		}

		if (state == 2) {
			if (*(*data + offset) == ';') {
				int len = offset - start;
				*content = (char*) malloc((len + 1) * sizeof(char));
				strncpy_s(*content, len + 1, *data + start, len);
				offset++;
				break;
			}
			offset++;
		}
	}

	*data += offset;
}

bool
rconv_stepmania_is_yes(char* content)
{
	utf8lwr(content);
	return utf8cmp(content, "yes") == 0 || utf8cmp(content, "1") == 0 || utf8cmp(content, "es") == 0 || utf8cmp(content, "omes") == 0;
}

void
rconv_stepmania_parse_partial(char* data, int* index, char** content, int* offset, int* state, RconvList* list, int size, void** element)
{
	if (*element == NULL) {
		*element = (void*) malloc(size);
	}

	if (*state == 2) {
		rconv_list_add(list, element);
		*element = malloc(size);
		*state = 0;
		*index = 0;
	}

	size_t data_len = strlen(data);
	int start = -1;
	int last_not_whitespace = -1;

	while (*offset < data_len) {
		char c = *(data + *offset);
		if (state == 0) {
			if (rconv_is_whitespace(c)) {
				offset++;
				continue;
			}
			start = *offset;
			*state = 1;
		}

		if (c == '=') {
			*state = 1;
			break;

		} else if (c == ',') {
			*state = 2;
			*index = 0;
			break;
		}

		if (start == -1) {
			start = *offset;
		}
		(*offset)++;
		if (!rconv_is_whitespace(c)) {
			last_not_whitespace = *offset;
		}
	}

	if (start > -1) {
		*content = rconv_substr(data, start, last_not_whitespace);
		(*offset)++;
	} else {
		*content = NULL;
	}

	if (*state == 1) {
		*state = 0;
	}
}

RconvStepmaniaBackgroundChange**
rconv_stepmania_parse_background_changes(char* data, int* len)
{
	RconvList* list = rconv_list();

	RconvStepmaniaBackgroundChange* elem = NULL;
	int state = 0;
	int idx = 0;
	int start = 0;
	int offset = 0;
	char* content = NULL;

	while (true) {
		rconv_stepmania_parse_partial(data, &idx, &content, &offset, &state, list, sizeof(RconvStepmaniaBackgroundChange), (void**) &elem);

		if (content == NULL) {
			break;
		}

		bool free_content = true;
		if (idx == 0) {
			elem->beat = rconv_float_new_from_string(content);
		} else if (idx == 1) {
			elem->path = content;
			free_content = false;
		} else if (idx == 2) {
			elem->update_rate = rconv_float_new_from_string(content);
		} else if (idx == 3) {
			elem->crossfade = rconv_parse_bool(content);
		} else if (idx == 4) {
			elem->stretch_rewind = rconv_parse_bool(content);
		} else if (idx == 5) {
			elem->stretch_no_loop = rconv_parse_bool(content);
		} else if (idx == 6) {
			elem->effect = content;
			free_content = false;
		} else if (idx == 7) {
			elem->file2 = content;
			free_content = false;
		} else if (idx == 8) {
			elem->transition = content;
			free_content = false;
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

	RCONV_STEPMANIA_CLEAR_LIST(RconvStepmaniaBackgroundChange)
}

char**
rconv_stepmania_parse_string_list(char* data, int* len)
{
	RconvList* list = rconv_list();

	int offset = 0;
	size_t size = strlen(data);
	int start = 0;
	int has_content = false;

	while (offset < size) {
		if (*(data + offset) == ',') {
			if (!has_content) {
				continue;
			}

			int str_len = offset - start;
			char* elem = malloc((str_len + 1) * sizeof(char));
			strncpy_s(elem, str_len + 1, data + start, str_len);
			rconv_list_add(list, elem);
			start = offset + 1;
			offset++;
			has_content = false;
			continue;
		}

		if (!rconv_is_whitespace(data[offset]) && !has_content) {
			has_content = true;
			start = offset;
		}
		offset++;
	}

	if (has_content) {
		int str_len = offset - start;
		char* elem = malloc((str_len + 1) * sizeof(char));
		strncpy_s(elem, str_len + 1, data + start, str_len);
		rconv_list_add(list, elem);
	}

	RCONV_STEPMANIA_CLEAR_LIST_NAMED(char,string)
}

RconvStepmaniaStop**
rconv_stepmania_parse_stops(char* data, int* len)
{
	RconvList* list = rconv_list();

	RconvStepmaniaStop* elem = NULL;
	int state = 0;
	int idx = 0;
	int start = 0;
	int offset = 0;
	char* content = NULL;

	while (true) {
		rconv_stepmania_parse_partial(data, &idx, &content, &offset, &state, list, sizeof(RconvStepmaniaStop), (void**) &elem);

		if (content == NULL) {
			break;
		}

		if (idx == 0) {
			elem->beat = rconv_float_new_from_string(content);
		} else if (idx == 1) {
			elem->duration = rconv_float_new_from_string(content);
		}

		free(content);

		idx++;
	}

	RCONV_STEPMANIA_CLEAR_LIST(RconvStepmaniaStop)
}

RconvStepmaniaBpmChange**
rconv_stepmania_parse_bpms(char* data, int* len)
{
	RconvList* list = rconv_list();

	RconvStepmaniaBpmChange* elem = NULL;
	int state = 0;
	int idx = 0;
	int start = 0;
	int offset = 0;
	char* content = NULL;

	while (true) {
		rconv_stepmania_parse_partial(data, &idx, &content, &offset, &state, list, sizeof(RconvStepmaniaBpmChange), (void**) &elem);

		if (content == NULL) {
			break;
		}

		if (idx == 0) {
			elem->beat = rconv_float_new_from_string(content);
		} else if (idx == 1) {
			elem->bpm = rconv_float_new_from_string(content);
		}

		free(content);

		idx++;
	}

	RCONV_STEPMANIA_CLEAR_LIST(RconvStepmaniaBpmChange)
}

RconvStepmaniaTimeSignature**
rconv_stepmania_parse_time_signatures(char* data, int* len)
{
	RconvList* list = rconv_list();

	RconvStepmaniaTimeSignature* elem = NULL;
	int state = 0;
	int idx = 0;
	int start = 0;
	int offset = 0;
	char* content = NULL;

	while (true) {
		rconv_stepmania_parse_partial(data, &idx, &content, &offset, &state, list, sizeof(RconvStepmaniaTimeSignature), (void**) &elem);

		if (content == NULL) {
			break;
		}

		if (idx == 0) {
			elem->beat = rconv_float_new_from_string(content);
		} else if (idx == 1) {
			elem->numerator = atoi(content);
		} else if (idx == 2) {
			elem->denominator = atoi(content);
		}

		free(content);

		idx++;
	}

	RCONV_STEPMANIA_CLEAR_LIST(RconvStepmaniaTimeSignature)
}

RconvStepmaniaModifier**
rconv_stepmania_parse_modifiers(char* data, int* len)
{
	// TOOD: Implement me
	return NULL;
}

RconvStepmaniaTimedAttack**
rconv_stepmania_parse_timed_attacks(char* data, int* len)
{
	/*
	RconvList* list = rconv_list();
	RconvStepmaniaTimedAttack* elem = (RconvStepmaniaTimedAttack*) malloc(sizeof(RconvStepmaniaBackgroundChange));

	int state = 0;
	int idx = 0;
	int start = 0;
	int offset = 0;
	char* content = NULL;

	while (true) {
		rconv_stepmania_parse_partial(data, idx, &content, offset, state);

		if (content == NULL) {
			break;
		}

		if (state == 2) {
			rconv_list_add(list, elem);
			elem = (RconvStepmaniaTimedAttack*) malloc(sizeof(RconvStepmaniaTimedAttack));
			state = 0;
			idx = 0;
			continue;
		}

		if (idx == 0) {
			rconv_float_from_string(elem->time, content);
		} else if (idx == 1) {
			rconv_float_from_string(elem->length, content);
		} else if (idx == 2) {
			elem->denominator = atoi(content);
		}

		free(content);

		idx++;
	}

	*items = (RconvStepmaniaTimedAttack*) rconv_list_to_array(list, len);
	if (signatures == -1) {
		*signatures = NULL;
	}
	rconv_list_free(list);
	*/
	return NULL;
}

RconvStepmaniaComboChange**
rconv_stepmania_parse_combo_changes(char* data, int* len)
{
	RconvList* list = rconv_list();

	RconvStepmaniaComboChange* elem = NULL;
	int state = 0;
	int idx = 0;
	int start = 0;
	int offset = 0;
	char* content = NULL;

	while (true) {
		rconv_stepmania_parse_partial(data, &idx, &content, &offset, &state, list, sizeof(RconvStepmaniaDelay), (void**) &elem);

		if (content == NULL) {
			break;
		}

		if (idx == 0) {
			elem->beat = rconv_float_new_from_string(content);
		} else if (idx == 1) {
			elem->hit = atoi(content);
		} else if (idx == 2) {
			elem->miss = atoi(content);
		}

		free(content);

		idx++;
	}

	RCONV_STEPMANIA_CLEAR_LIST(RconvStepmaniaComboChange)
}

RconvStepmaniaDelay**
rconv_stepmania_parse_delays(char* data, int* len)
{
	RconvList* list = rconv_list();

	RconvStepmaniaDelay* elem = NULL;
	int state = 0;
	int idx = 0;
	int start = 0;
	int offset = 0;
	char* content = NULL;

	while (true) {
		rconv_stepmania_parse_partial(data, &idx, &content, &offset, &state, list, sizeof(RconvStepmaniaDelay), (void**) &elem);

		if (content == NULL) {
			break;
		}

		if (idx == 0) {
			elem->beat = rconv_float_new_from_string(content);
		} else if (idx == 1) {
			elem->duration = rconv_float_new_from_string(content);
		}

		free(content);

		idx++;
	}

	RCONV_STEPMANIA_CLEAR_LIST(RconvStepmaniaDelay)
}

RconvStepmaniaTickCount**
rconv_stepmania_parse_tick_counts(char* data, int* len)
{
	RconvList* list = rconv_list();

	RconvStepmaniaTickCount* elem = NULL;
	int state = 0;
	int idx = 0;
	int start = 0;
	int offset = 0;
	char* content = NULL;

	while (true) {
		rconv_stepmania_parse_partial(data, &idx, &content, &offset, &state, list, sizeof(RconvStepmaniaTickCount), (void**) &elem);

		if (content == NULL) {
			break;
		}

		if (idx == 0) {
			elem->beat = rconv_float_new_from_string(content);
		} else if (idx == 1) {
			elem->count = atoi(content);
		}

		free(content);

		idx++;
	}

	RCONV_STEPMANIA_CLEAR_LIST(RconvStepmaniaTickCount)
}

RconvStepmaniaComboChange**
rconv_stepmania_parse_combos(char* data, int* len)
{
	RconvList* list = rconv_list();

	RconvStepmaniaComboChange* elem = NULL;
	int state = 0;
	int idx = 0;
	int start = 0;
	int offset = 0;
	char* content = NULL;

	while (true) {
		rconv_stepmania_parse_partial(data, &idx, &content, &offset, &state, list, sizeof(RconvStepmaniaComboChange), (void**) &elem);

		if (content == NULL) {
			break;
		}

		if (idx == 0) {
			elem->beat = rconv_float_new_from_string(content);
		} else if (idx == 1) {
			elem->hit = atoi(content);
		} else if (idx == 2) {
			elem->miss = atoi(content);
		}

		free(content);

		idx++;
	}

	RCONV_STEPMANIA_CLEAR_LIST(RconvStepmaniaComboChange)
}

RconvStepmaniaSpeedChange**
rconv_stepmania_parse_speed_changes(char* data, int* len)
{
	RconvList* list = rconv_list();

	RconvStepmaniaSpeedChange* elem = NULL;
	int state = 0;
	int idx = 0;
	int start = 0;
	int offset = 0;
	char* content = NULL;

	while (true) {
		rconv_stepmania_parse_partial(data, &idx, &content, &offset, &state, list, sizeof(RconvStepmaniaSpeedChange), (void**) &elem);

		if (content == NULL) {
			break;
		}

		if (idx == 0) {
			elem->beat = rconv_float_new_from_string(content);
		} else if (idx == 1) {
			elem->ratio = rconv_float_new_from_string(content);
		} else if (idx == 2) {
			elem->duration = rconv_float_new_from_string(content);
		} else if (idx == 3) {
			elem->in_seconds = rconv_parse_bool(content);
		}

		free(content);

		idx++;
	}

	RCONV_STEPMANIA_CLEAR_LIST(RconvStepmaniaSpeedChange)
}

RconvStepmaniaScrollSpeedChange**
rconv_stepmania_parse_scroll_changes(char* data, int* len)
{
	RconvList* list = rconv_list();

	RconvStepmaniaScrollSpeedChange* elem = NULL;
	int state = 0;
	int idx = 0;
	int start = 0;
	int offset = 0;
	char* content = NULL;

	while (true) {
		rconv_stepmania_parse_partial(data, &idx, &content, &offset, &state, list, sizeof(RconvStepmaniaScrollSpeedChange), (void**) &elem);

		if (content == NULL) {
			break;
		}

		if (idx == 0) {
			elem->beat = rconv_float_new_from_string(content);
		} else if (idx == 1) {
			elem->factor = rconv_float_new_from_string(content);
		}

		free(content);

		idx++;
	}

	RCONV_STEPMANIA_CLEAR_LIST(RconvStepmaniaScrollSpeedChange)
}

RconvStepmaniaFakeSection**
rconv_stepmania_parse_fake_sections(char* data, int* len)
{
	RconvList* list = rconv_list();

	RconvStepmaniaFakeSection* elem = NULL;
	int state = 0;
	int idx = 0;
	int start = 0;
	int offset = 0;
	char* content = NULL;

	while (true) {
		rconv_stepmania_parse_partial(data, &idx, &content, &offset, &state, list, sizeof(RconvStepmaniaFakeSection), (void**) &elem);

		if (content == NULL) {
			break;
		}

		if (idx == 0) {
			elem->beat = rconv_float_new_from_string(content);
		} else if (idx == 1) {
			elem->duration = rconv_float_new_from_string(content);
		}

		free(content);

		idx++;
	}

	RCONV_STEPMANIA_CLEAR_LIST(RconvStepmaniaFakeSection)
}

RconvStepmaniaLabel**
rconv_stepmania_parse_labels(char* data, int* len)
{
	RconvList* list = rconv_list();

	RconvStepmaniaLabel* elem = NULL;
	int state = 0;
	int idx = 0;
	int start = 0;
	int offset = 0;
	char* content = NULL;

	while (true) {
		rconv_stepmania_parse_partial(data, &idx, &content, &offset, &state, list, sizeof(RconvStepmaniaLabel), (void**) &elem);

		if (content == NULL) {
			break;
		}

		bool free_content = true;

		if (idx == 0) {
			elem->beat = rconv_float_new_from_string(content);
		} else if (idx == 1) {
			elem->content = content;
			free_content = false;
		}

		if (free_content) {
			free(content);
		}

		idx++;
	}

	RCONV_STEPMANIA_CLEAR_LIST(RconvStepmaniaLabel)
}

void
rconv_stepmania_parse_chart_type(char* data, RconvStepmaniaChartType* type)
{
	if (utf8cmp(data, "dance-single") == 0) {
		*type = DANCE_SINGLE;
	} else if (utf8cmp(data, "dance-double") == 0) {
		*type = DANCE_DOUBLE;
	} else if (utf8cmp(data, "dance-couple") == 0) {
		*type = DANCE_COUPLE;
	} else if (utf8cmp(data, "dance-routine") == 0) {
		*type = DANCE_ROUTINE;
	} else if (utf8cmp(data, "dance-solo") == 0) {
		*type = DANCE_SOLO;
	} else if (utf8cmp(data, "pump-single") == 0) {
		*type = PUMP_SINGLE;
	} else if (utf8cmp(data, "pump-halfdouble") == 0) {
		*type = PUMP_HALF_DOUBLE;
	} else if (utf8cmp(data, "pump-double") == 0) {
		*type = PUMP_DOUBLE;
	} else if (utf8cmp(data, "pump-couple") == 0) {
		*type = PUMP_COUPLE;
	} else {
		*type = -1;
	}
}

void
rconv_stepmania_parse_difficulty(char* data, RconvStepmaniaDifficulty* diff)
{
	if (utf8cmp(data, "beginner") == 0) {
		*diff = BEGINNER;
	} else if (utf8cmp(data, "easy") == 0) {
		*diff = EASY;
	} else if (utf8cmp(data, "medium") == 0) {
		*diff = MEDIUM;
	} else if (utf8cmp(data, "hard") == 0) {
		*diff = HARD;
	} else if (utf8cmp(data, "challange") == 0) {
		*diff = CHALLANGE;
	} else {
		*diff = EDIT;
	}
}

void
rconv_stepmania_parse_note_data(char* content, RconvList* list)
{
	RconvStepmaniaNoteData* data = (RconvStepmaniaNoteData*) malloc(sizeof(RconvStepmaniaNoteData));

	size_t max = strlen(content);
	long offset = 0;
	int state = 0;
	long start = -1;
	int radar_index = 0;

	while (offset < max) {
		char tmp = *(content + offset);
		if (state == 0) {
			if (rconv_is_whitespace(tmp)) {
				offset++;
				continue;
			}
			start = offset;
			offset++;
			state = 1;
			continue;
		}

		if (state == 1) {
			if (tmp != ':') {
				offset++;
				continue;
			}
			char* tmp = rconv_substr(content, start, offset);
			rconv_stepmania_parse_chart_type(tmp, &data->chart_type);
			free(tmp);
			state = 2;
			start = offset;
			offset++;
			continue;
		}

		if (state == 2) {
			if (tmp != ':') {
				offset++;
				continue;
			}
			data->description = rconv_substr(content, start, offset);
			state = 3;
			start = offset;
			offset++;
			continue;
		}

		if (state == 3) {
			if (tmp != ':') {
				offset++;
				continue;
			}
			char* tmp = rconv_substr(content, start, offset);
			rconv_stepmania_parse_difficulty(tmp, &data->difficulty);
			free(tmp);
			state = 4;
			start = offset;
			offset++;
			continue;
		}

		if (state == 4) {
			if (tmp != ':') {
				offset++;
				continue;
			}
			char* tmp = rconv_substr(content, start, offset);
			data->level = atoi(tmp);
			free(tmp);
			state = 5;
			start = offset;
			offset++;
			continue;
		}

		if (state == 5) {
			if (tmp == ',' || tmp == ':') {
				char* radar_tmp_val = rconv_substr(content, start, offset);
				if (radar_index < 5) {
					// float* pos = ((&data->radar_values) + radar_index);
					// *pos = atof(tmp);
					data->radar_values[radar_index] = atof(radar_tmp_val);
				}
				free(radar_tmp_val);
				radar_index++;
				start = offset;
				offset++;
				if (tmp == ':') {
					state = 6;
					break;
				}
				continue;
			}

			offset++;
			continue;
		}

		break;
	}

	if (state != 6) {
		// Invalid state!
		return;
	}

	rconv_list_add(list, data);
}

RconvStepmaniaChartFile*
rconv_stepmania_parse(char* data)
{
	RconvStepmaniaChartFile* chart = (RconvStepmaniaChartFile*) malloc(sizeof(RconvStepmaniaChartFile));
	char* tag = NULL;
	char* content = NULL;
	RconvList* notes = rconv_list();

	while (true) {
		rconv_stepmania_parse_tags(&data, &tag, &content);
		if (tag == NULL || content == NULL) {
			if (tag != NULL) {
				free(tag);
			}
			if (content != NULL) {
				free(content);
			}
			break;
		}

		utf8lwr(tag);
		printf("parsing tag '%s' with content '%s' ...\n", tag, content);

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
			chart->sample_start = rconv_float_new_from_string(content);
		} else if (utf8cmp(tag, "samplelength") == 0) {
			chart->sample_length = rconv_float_new_from_string(content);
		} else if (utf8cmp(tag, "offset") == 0) {
			chart->offset = rconv_float_new_from_string(content);
		} else if (utf8cmp(tag, "selectable") == 0) {
			chart->selectable = rconv_stepmania_is_yes(content);
		} else if (utf8cmp(tag, "bgchanges") == 0) {
			chart->background_changes = rconv_stepmania_parse_background_changes(content, &chart->background_changes_len);
		} else if (utf8cmp(tag, "bgchanges2") == 0) {
			chart->background_changes2 = rconv_stepmania_parse_background_changes(content, &chart->background_changes2_len);
		} else if (utf8cmp(tag, "bgchanges3") == 0) {
			chart->background_changes3 = rconv_stepmania_parse_background_changes(content, &chart->background_changes3_len);
		} else if (utf8cmp(tag, "animations") == 0) {
			chart->animations = rconv_stepmania_parse_background_changes(content, &chart->animations_len);
		} else if (utf8cmp(tag, "fgchanges") == 0) {
			chart->foreground_changes = rconv_stepmania_parse_background_changes(content, &chart->foreground_changes_len);
		} else if (utf8cmp(tag, "keysounds") == 0) {
			chart->key_sounds = rconv_stepmania_parse_string_list(content, &chart->key_sounds_len);
		} else if (utf8cmp(tag, "stops") == 0 || utf8cmp(tag, "freezes") == 0) {
			chart->stops = rconv_stepmania_parse_stops(content, &chart->stops_len);
		} else if (utf8cmp(tag, "bpms") == 0) {
			chart->bpms = rconv_stepmania_parse_bpms(content, &chart->bpms_len);
		} else if (utf8cmp(tag, "timesignatures") == 0) {
			chart->time_signatures = rconv_stepmania_parse_time_signatures(content, &chart->time_signatures_len);
		} else if (utf8cmp(tag, "attacks") == 0) {
			chart->attacks = rconv_stepmania_parse_timed_attacks(content, &chart->attacks_len);
		} else if (utf8cmp(tag, "delays") == utf8cmp(tag, "warps") == 0) {
			chart->delays = rconv_stepmania_parse_delays(content, &chart->delays_len);
		} else if (utf8cmp(tag, "tickcounts") == 0) {
			chart->tick_counts = rconv_stepmania_parse_tick_counts(content, &chart->tick_counts_len);
		} else if (utf8cmp(tag, "notes") == 0) {
			rconv_stepmania_parse_note_data(content, notes);
		} else if (utf8cmp(tag, "combos") == 0) {
			chart->combos = rconv_stepmania_parse_combo_changes(content, &chart->combos_len);
		} else if (utf8cmp(tag, "speeds") == 0) {
			chart->speeds = rconv_stepmania_parse_speed_changes(content, &chart->speeds_len);
		} else if (utf8cmp(tag, "scrolls") == 0) {
			chart->scrolls = rconv_stepmania_parse_scroll_changes(content, &chart->scrolls_len);
		} else if (utf8cmp(tag, "fakes") == 0) {
			chart->fakes = rconv_stepmania_parse_fake_sections(content, &chart->fakes_len);
		} else if (utf8cmp(tag, "labels") == 0) {
			chart->labels = rconv_stepmania_parse_labels(content, &chart->labels_len);
		} else {
			printf("unknown tag '%s'!\n", tag);
			break;
		}

		free(tag);
		if (free_content) {
			free(content);
		}
	}

	return chart;
}
