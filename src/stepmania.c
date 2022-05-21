#include "./stepmania.h"

#define RCONV_STEPMANIA_PARSE_LIST_ENTRIES(type,idxFn) \
type** rconv_stepmania_parse_## type ##_list_entries(char* data, int* len) \
{ \
	RconvList* list = rconv_list(); \
	type* elem = NULL; \
	int state = 0; \
	int idx = 0; \
	int start = 0; \
	int offset = 0; \
	char* content = NULL; \
 	 \
	while (true) { \
		rconv_stepmania_parse_partial(data, &idx, &content, &offset, &state, list, sizeof(type), (void**) &elem); \
 		 \
		if (content == NULL) { \
			break; \
		} \
		 \
		idxFn(elem, idx, content); \
		 \
		idx++; \
	} \
	 \
	type** items = rconv_list_to_## type ##_array(list, len); \
	rconv_list_free(list); \
	return items; \
}

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

void
rconv_stepmania_handle_background_change_entry(RconvStepmaniaBackgroundChange* elem, int idx, char* content)
{
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
}

void
rconv_stepmania_handle_stop_entry(RconvStepmaniaStop* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_float_new_from_string(content);
	} else if (idx == 1) {
		elem->duration = rconv_float_new_from_string(content);
	}

	free(content);
}

void
rconv_stepmania_handle_bpm_change_entry(RconvStepmaniaBpmChange* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_float_new_from_string(content);
	} else if (idx == 1) {
		elem->bpm = rconv_float_new_from_string(content);
	}
	free(content);
}

void
rconv_stepmania_handle_time_signature_entry(RconvStepmaniaTimeSignature* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_float_new_from_string(content);
	} else if (idx == 1) {
		elem->numerator = atoi(content);
	} else if (idx == 2) {
		elem->denominator = atoi(content);
	}

	free(content);
}

void
rconv_stepmania_handle_combo_change_entry(RconvStepmaniaComboChange* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_float_new_from_string(content);
	} else if (idx == 1) {
		elem->hit = atoi(content);
	} else if (idx == 2) {
		elem->miss = atoi(content);
	}

	free(content);
}

void
rconv_stepmania_handle_delay_entry(RconvStepmaniaDelay* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_float_new_from_string(content);
	} else if (idx == 1) {
		elem->duration = rconv_float_new_from_string(content);
	}

	free(content);
}

void
rconv_stepmania_handle_tick_count_entry(RconvStepmaniaTickCount* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_float_new_from_string(content);
	} else if (idx == 1) {
		elem->count = atoi(content);
	}

	free(content);
}

void
rconv_stepmania_handle_speed_change_entry(RconvStepmaniaSpeedChange* elem, int idx, char* content)
{
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
}

void
rconv_stepmania_handle_scroll_speed_change_entry(RconvStepmaniaScrollSpeedChange* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_float_new_from_string(content);
	} else if (idx == 1) {
		elem->factor = rconv_float_new_from_string(content);
	}

	free(content);
}

void
rconv_stepmania_handle_fake_section_entry(RconvStepmaniaFakeSection* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_float_new_from_string(content);
	} else if (idx == 1) {
		elem->duration = rconv_float_new_from_string(content);
	}

	free(content);
}

void
rconv_stepmania_handle_label_entry(RconvStepmaniaLabel* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_float_new_from_string(content);
		free(content);
	} else if (idx == 1) {
		elem->content = content;
	}
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

RCONV_STEPMANIA_PARSE_LIST_ENTRIES(RconvStepmaniaBpmChange, rconv_stepmania_handle_bpm_change_entry)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES(RconvStepmaniaStop, rconv_stepmania_handle_stop_entry)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES(RconvStepmaniaDelay, rconv_stepmania_handle_delay_entry)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES(RconvStepmaniaTimeSignature, rconv_stepmania_handle_time_signature_entry)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES(RconvStepmaniaTickCount, rconv_stepmania_handle_tick_count_entry)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES(RconvStepmaniaBackgroundChange, rconv_stepmania_handle_background_change_entry)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES(RconvStepmaniaComboChange, rconv_stepmania_handle_combo_change_entry)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES(RconvStepmaniaSpeedChange, rconv_stepmania_handle_speed_change_entry)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES(RconvStepmaniaScrollSpeedChange, rconv_stepmania_handle_scroll_speed_change_entry)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES(RconvStepmaniaFakeSection, rconv_stepmania_handle_fake_section_entry)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES(RconvStepmaniaLabel, rconv_stepmania_handle_label_entry)

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
			chart->background_changes = rconv_stepmania_parse_RconvStepmaniaBackgroundChange_list_entries(content, &chart->background_changes_len);
		} else if (utf8cmp(tag, "bgchanges2") == 0) {
			chart->background_changes2 = rconv_stepmania_parse_RconvStepmaniaBackgroundChange_list_entries(content, &chart->background_changes2_len);
		} else if (utf8cmp(tag, "bgchanges3") == 0) {
			chart->background_changes3 = rconv_stepmania_parse_RconvStepmaniaBackgroundChange_list_entries(content, &chart->background_changes3_len);
		} else if (utf8cmp(tag, "animations") == 0) {
			chart->animations = rconv_stepmania_parse_RconvStepmaniaBackgroundChange_list_entries(content, &chart->animations_len);
		} else if (utf8cmp(tag, "fgchanges") == 0) {
			chart->foreground_changes = rconv_stepmania_parse_RconvStepmaniaBackgroundChange_list_entries(content, &chart->foreground_changes_len);
		} else if (utf8cmp(tag, "keysounds") == 0) {
			chart->key_sounds = rconv_stepmania_parse_string_list(content, &chart->key_sounds_len);
		} else if (utf8cmp(tag, "stops") == 0 || utf8cmp(tag, "freezes") == 0) {
			chart->stops = rconv_stepmania_parse_RconvStepmaniaStop_list_entries(content, &chart->stops_len);
		} else if (utf8cmp(tag, "bpms") == 0) {
			chart->bpms = rconv_stepmania_parse_RconvStepmaniaBpmChange_list_entries(content, &chart->bpms_len);
			printf("bpms: ptr(%p) len(%d)", chart->bpms, chart->bpms_len);
		} else if (utf8cmp(tag, "timesignatures") == 0) {
			chart->time_signatures = rconv_stepmania_parse_RconvStepmaniaTimeSignature_list_entries(content, &chart->time_signatures_len);
		} else if (utf8cmp(tag, "attacks") == 0) {
			chart->attacks = rconv_stepmania_parse_timed_attacks(content, &chart->attacks_len);
		} else if (utf8cmp(tag, "delays") == utf8cmp(tag, "warps") == 0) {
			chart->delays = rconv_stepmania_parse_RconvStepmaniaDelay_list_entries(content, &chart->delays_len);
		} else if (utf8cmp(tag, "tickcounts") == 0) {
			chart->tick_counts = rconv_stepmania_parse_RconvStepmaniaTickCount_list_entries(content, &chart->tick_counts_len);
		} else if (utf8cmp(tag, "notes") == 0) {
			rconv_stepmania_parse_note_data(content, notes);
		} else if (utf8cmp(tag, "combos") == 0) {
			chart->combos = rconv_stepmania_parse_RconvStepmaniaComboChange_list_entries(content, &chart->combos_len);
		} else if (utf8cmp(tag, "speeds") == 0) {
			chart->speeds = rconv_stepmania_parse_RconvStepmaniaSpeedChange_list_entries(content, &chart->speeds_len);
		} else if (utf8cmp(tag, "scrolls") == 0) {
			chart->scrolls = rconv_stepmania_parse_RconvStepmaniaScrollSpeedChange_list_entries(content, &chart->scrolls_len);
		} else if (utf8cmp(tag, "fakes") == 0) {
			chart->fakes = rconv_stepmania_parse_RconvStepmaniaFakeSection_list_entries(content, &chart->fakes_len);
		} else if (utf8cmp(tag, "labels") == 0) {
			chart->labels = rconv_stepmania_parse_RconvStepmaniaLabel_list_entries(content, &chart->labels_len);
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

void
rconv_stepmania_free_bpm_change(RconvStepmaniaBpmChange* bpm_change)
{
	if (bpm_change == NULL) {
		return;
	}

	free(bpm_change->beat);
	bpm_change->beat = NULL;

	free(bpm_change->bpm);
	bpm_change->bpm = NULL;

	free(bpm_change);
}

void
rconv_stepmania_free_stop(RconvStepmaniaStop* stop)
{
	if (stop == NULL) {
		return;
	}

	free(stop->beat);
	stop->beat = NULL;

	free(stop->duration);
	stop->duration = NULL;

	free(stop);
}

void
rconv_stepmania_free_delay(RconvStepmaniaDelay* delay)
{
	if (delay == NULL) {
		return;
	}

	free(delay->beat);
	delay->beat = NULL;

	free(delay->duration);
	delay->duration = NULL;

	free(delay);
}

void
rconv_stepmania_free_time_signature(RconvStepmaniaTimeSignature* time_signature)
{
	if (time_signature == NULL) {
		return;
	}

	free(time_signature->beat);
	time_signature->beat = NULL;

	free(time_signature);
}

void
rconv_stepmania_free_instrument_track(RconvStepmaniaInstrumentTrack* instrument_track)
{
	if (instrument_track == NULL) {
		return;
	}

	free(instrument_track->file);
	instrument_track->file = NULL;

	free(instrument_track->instrument);
	instrument_track->instrument = NULL;

	free(instrument_track);
}

void
rconv_stepmania_free_tick_count(RconvStepmaniaTickCount* tick_count)
{
	if (tick_count == NULL) {
		return;
	}

	free(tick_count->beat);
	tick_count->beat = NULL;

	free(tick_count);
}

void
rconv_stepmania_free_background_change(RconvStepmaniaBackgroundChange* background_change)
{
	if (background_change == NULL) {
		return;
	}

	free(background_change->beat);
	background_change->beat = NULL;

	free(background_change->path);
	background_change->path = NULL;

	free(background_change->update_rate);
	background_change->update_rate = NULL;

	free(background_change->effect);
	background_change->effect = NULL;

	free(background_change->file2);
	background_change->file2 = NULL;

	free(background_change->transition);
	background_change->transition = NULL;

	free(background_change->color1);
	background_change->color1 = NULL;

	free(background_change->color2);
	background_change->color2 = NULL;

	free(background_change);
}

void
rconv_stepmania_free_modifier(RconvStepmaniaModifier* modifier)
{
	if (modifier == NULL) {
		return;
	}

	free(modifier->name);
	modifier->name = NULL;

	free(modifier->player);
	modifier->player = NULL;

	free(modifier->magnitude);
	modifier->magnitude = NULL;

	free(modifier);
}

void
rconv_stepmania_free_attack(RconvStepmaniaAttack* attack)
{
	if (attack == NULL) {
		return;
	}

	free(attack->length);
	attack->length = NULL;

	for (int i = 0; i < attack->mods_len; i++) {
		free(*(attack->mods + i));
	}

	free(attack->mods);
	attack->mods = NULL;

	free(attack);
}

void
rconv_stepmania_free_timed_attack(RconvStepmaniaTimedAttack* timed_attack)
{
	if (timed_attack == NULL) {
		return;
	}

	free(timed_attack->time);
	timed_attack->time = NULL;

	free(timed_attack->length);
	timed_attack->length = NULL;

	for (int i = 0; i < timed_attack->mods_len; i++) {
		rconv_stepmania_free_modifier(*(timed_attack->mods + 1));
	}
	
	free(timed_attack->mods);
	timed_attack->mods = NULL;

	free(timed_attack);
}

void
rconv_stepmania_free_combo_change(RconvStepmaniaComboChange* combo_change)
{
	if (combo_change == NULL) {
		return;
	}

	free(combo_change->beat);
	combo_change->beat == NULL;

	free(combo_change);
}

void
rconv_stepmania_free_speed_change(RconvStepmaniaSpeedChange* speed_change)
{
	if (speed_change == NULL) {
		return;
	}

	free(speed_change->beat);
	speed_change->beat == NULL;

	free(speed_change->ratio);
	speed_change->ratio = NULL;

	free(speed_change->duration);
	speed_change->duration = NULL;

	free(speed_change);
}

void
rconv_stepmania_free_scroll_speed_change(RconvStepmaniaScrollSpeedChange* scroll_speed_change)
{
	if (scroll_speed_change == NULL) {
		return;
	}

	free(scroll_speed_change->beat);
	scroll_speed_change->beat = NULL;

	free(scroll_speed_change->factor);
	scroll_speed_change->factor = NULL;

	free(scroll_speed_change);
}

void
rconv_stepmania_free_fake_section(RconvStepmaniaFakeSection* fake_section)
{
	if (fake_section == NULL) {
		return;
	}

	free(fake_section->beat);
	fake_section->beat = NULL;

	free(fake_section->duration);
	fake_section->duration = NULL;

	free(fake_section);
}

void
rconv_stepmania_free_label(RconvStepmaniaLabel* label)
{
	if (label == NULL) {
		return;
	}

	free(label->beat);
	label->beat = NULL;

	free(label->content);
	label->content = NULL;

	free(label);
}

void
rconv_stepmania_free_note(RconvStepmaniaNote* note)
{
	if (note == NULL) {
		return;
	}

	for (int i = 0; i < note->attack_len; i++) {
		rconv_stepmania_free_attack(*(note->attack + i));
	}

	free(note->attack);
	note->attack = NULL;

	for (int i = 0; i < note->modifiers_len; i++) {
		rconv_stepmania_free_modifier(*(note->modifiers + i));
	}

	free(note->modifiers);
	note->modifiers = NULL;

	free(note);
}

void
rconv_stepmania_free_beat(RconvStepmaniaBeat* beat)
{
	if (beat == NULL) {
		return;
	}
	for (int i = 0; i < beat->notes_len; i++) {
		rconv_stepmania_free_note(*(beat->notes + i));
	}
	free(beat->notes);
	beat->notes = NULL;
	free(beat);
}

void
rconv_stepmania_free_note_data(RconvStepmaniaNoteData* note_data)
{
	if (note_data == NULL) {
		return;
	}

	free(note_data->description);
	note_data->description = NULL;

	for (int i = 0; i < note_data->beats_len; i++) {
		rconv_stepmania_free_beat(*(note_data->beats + i));
	}

	free(note_data->beats);
	note_data->beats = NULL;

	free(note_data);
}

void
rconv_stepmania_free_chart_file(RconvStepmaniaChartFile* chart_file)
{
	if (chart_file == NULL) {
		return;
	}

	free(chart_file->title);
	chart_file->title = NULL;

	free(chart_file->subtitle);
	chart_file->subtitle = NULL;

	free(chart_file->artist);
	chart_file->artist = NULL;

	free(chart_file->title_transliterated);
	chart_file->title_transliterated = NULL;

	free(chart_file->subtitle_transliterated);
	chart_file->subtitle_transliterated = NULL;

	free(chart_file->artist_transliterated);
	chart_file->artist_transliterated = NULL;

	free(chart_file->genre);
	chart_file->genre = NULL;

	free(chart_file->credit);
	chart_file->credit = NULL;

	free(chart_file->banner);
	chart_file->banner = NULL;

	free(chart_file->background);
	chart_file->background = NULL;

	free(chart_file->lyrics);
	chart_file->lyrics = NULL;

	free(chart_file->cd_title);
	chart_file->cd_title = NULL;

	free(chart_file->music);
	chart_file->music = NULL;

	for (int i = 0; i < chart_file->instrument_tracks_len; i++) {
		rconv_stepmania_free_instrument_track(*(chart_file->instrument_tracks + i));
	}

	free(chart_file->instrument_tracks);
	chart_file->instrument_tracks = NULL;

	free(chart_file->sample_start);
	chart_file->sample_start = NULL;

	free(chart_file->sample_length);
	chart_file->sample_length = NULL;

	free(chart_file->display_bpm);
	chart_file->display_bpm = NULL;

	for (int i = 0; i < chart_file->background_changes_len; i++) {
		rconv_stepmania_free_background_change(*(chart_file->background_changes + i));
	}

	free(chart_file->background_changes);
	chart_file->background_changes = NULL;

	for (int i = 0; i < chart_file->background_changes2_len; i++) {
		rconv_stepmania_free_background_change(*(chart_file->background_changes2 + i));
	}

	free(chart_file->background_changes2);
	chart_file->background_changes2 = NULL;

	for (int i = 0; i < chart_file->background_changes3_len; i++) {
		rconv_stepmania_free_background_change(*(chart_file->background_changes3 + i));
	}

	free(chart_file->background_changes3);
	chart_file->background_changes3 = NULL;

	for (int i = 0; i < chart_file->animations_len; i++) {
		rconv_stepmania_free_background_change(*(chart_file->animations + i));
	}

	free(chart_file->animations);
	chart_file->animations = NULL;

	for (int i = 0; i < chart_file->foreground_changes_len; i++) {
		rconv_stepmania_free_background_change(*(chart_file->foreground_changes + i));
	}

	free(chart_file->foreground_changes);
	chart_file->foreground_changes = NULL;

	for (int i = 0; i < chart_file->key_sounds_len; i++) {
		free(*(chart_file->key_sounds + i));
	}

	free(chart_file->key_sounds);
	chart_file->key_sounds = NULL;

	free(chart_file->offset);
	chart_file->offset = NULL;

	for (int i = 0; i < chart_file->stops_len; i++) {
		rconv_stepmania_free_stop(*(chart_file->stops + i));
	}

	free(chart_file->stops);
	chart_file->stops = NULL;

	for (int i = 0; i < chart_file->bpms_len; i++) {
		rconv_stepmania_free_bpm_change(*(chart_file->bpms + i));
	}

	free(chart_file->bpms);
	chart_file->bpms = NULL;

	for (int i = 0; i < chart_file->time_signatures_len; i++) {
		rconv_stepmania_free_time_signature(*(chart_file->time_signatures + i));
	}

	free(chart_file->time_signatures);
	chart_file->time_signatures = NULL;

	for (int i = 0; i < chart_file->attacks_len; i++) {
		rconv_stepmania_free_attack(*(chart_file->attacks + i));
	}

	free(chart_file->attacks);
	chart_file->attacks = NULL;

	for (int i = 0; i < chart_file->delays_len; i++) {
		rconv_stepmania_free_delay(*(chart_file->delays + i));
	}

	free(chart_file->delays);
	chart_file->delays = NULL;

	for (int i = 0; i < chart_file->tick_counts_len; i++) {
		rconv_stepmania_free_tick_count(*(chart_file->tick_counts + i));
	}

	free(chart_file->tick_counts);
	chart_file->tick_counts = NULL;

	for (int i = 0; i < chart_file->note_data_len; i++) {
		rconv_stepmania_free_note_data(*(chart_file->note_data + i));
	}

	free(chart_file->note_data);
	chart_file->note_data = NULL;

	for (int i = 0; i < chart_file->combos_len; i++) {
		rconv_stepmania_free_combo_change(*(chart_file->combos + i));
	}

	free(chart_file->combos);
	chart_file->combos = NULL;

	for (int i = 0; i < chart_file->speeds_len; i++) {
		rconv_stepmania_free_speed_change(*(chart_file->speeds + i));
	}

	free(chart_file->speeds);
	chart_file->speeds = NULL;

	for (int i = 0; i < chart_file->scrolls_len; i++) {
		rconv_stepmania_free_scroll_speed_change(*(chart_file->scrolls + i));
	}

	free(chart_file->scrolls);
	chart_file->scrolls = NULL;

	for (int i = 0; i < chart_file->fakes_len; i++) {
		rconv_stepmania_free_fake_section(*(chart_file->fakes + i));
	}

	free(chart_file->fakes);
	chart_file->fakes = NULL;

	for (int i = 0; i < chart_file->labels_len; i++) {
		rconv_stepmania_free_label(*(chart_file->labels + i));
	}

	free(chart_file->labels);
	chart_file->labels = NULL;

	free(chart_file);
}
