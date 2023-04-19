#include "parsers.h"

RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES_GEN(RconvStepmaniaBpmChange, bpm_change, bpm_changes)
RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES_GEN(RconvStepmaniaStop, stop, stops)
RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES_GEN(RconvStepmaniaDelay, delay, delays)
RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES_GEN(RconvStepmaniaTimeSignature, time_signature, time_signatures)
RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES_GEN(RconvStepmaniaInstrumentTrack, instrument_track, instrument_tracks)
RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES_GEN(RconvStepmaniaTickCount, tick_count, tick_counts)
RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES_GEN(RconvStepmaniaBackgroundChange, background_change, background_changes)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN(RconvStepmaniaModifier, ' ', modifier, modifiers)
RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES_GEN(RconvStepmaniaComboChange, combo_change, combo_changes)
RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES_GEN(RconvStepmaniaSpeedChange, speed_change, speed_changes)
RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES_GEN(RconvStepmaniaScrollSpeedChange, scroll_speed_change, scroll_speed_changes)
RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES_GEN(RconvStepmaniaFakeSection, fake_section, fake_sections)
RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES_GEN(RconvStepmaniaLabel, label, labels)

RCONV_LIST_TO_ARRAY_GEN_NAMED(RconvStepmaniaNoteData, note_data)
RCONV_LIST_TO_ARRAY_GEN_NAMED(RconvStepmaniaTimedAttack, timed_attack)

void
rconv_stepmania_handle_bpm_change_entry(RconvStepmaniaBpmChange* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	} else if (idx == 1) {
		elem->bpm = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	}
	free(content);
}

void
rconv_stepmania_handle_stop_entry(RconvStepmaniaStop* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	} else if (idx == 1) {
		elem->duration = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	}

	free(content);
}

void
rconv_stepmania_handle_delay_entry(RconvStepmaniaDelay* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	} else if (idx == 1) {
		elem->duration = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	}

	free(content);
}

void
rconv_stepmania_handle_time_signature_entry(RconvStepmaniaTimeSignature* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	} else if (idx == 1) {
		elem->numerator = atoi(content);
	} else if (idx == 2) {
		elem->denominator = atoi(content);
	}

	free(content);
}

void
rconv_stepmania_handle_instrument_track_entry(RconvStepmaniaInstrumentTrack* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->instrument = content;
	} else if (idx == 1) {
		elem->file = content;
	} else {
		free(content);
	}
}

void
rconv_stepmania_handle_tick_count_entry(RconvStepmaniaTickCount* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	} else if (idx == 1) {
		elem->count = atoi(content);
	}

	free(content);
}

void
rconv_stepmania_handle_background_change_entry(RconvStepmaniaBackgroundChange* elem, int idx, char* content)
{
	bool free_content = true;
	if (idx == 0) {
		elem->beat = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	} else if (idx == 1) {
		elem->path = content;
		free_content = false;
	} else if (idx == 2) {
		elem->update_rate = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
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
rconv_stepmania_handle_modifier_entry(RconvStepmaniaModifier* elem, int idx, char* content)
{
	if (content == NULL) {
		return;
	}

	bool free_content = true;

	size_t len = strlen(content);

	if (content[0] == '*') {
		char* tmp = rconv_substr(content, 1, len);
		elem->approach_rate = atoi(tmp);
		free(tmp);
	} else if (content[0] == 'p' || content[0] == 'P') {
		elem->player = rconv_substr(content, 1, len);
	} else if (content[len] == '%') {
		elem->is_percent = true;
		char* tmp = rconv_substr(content, 0, len - 1);
		elem->magnitude = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, tmp);
		free(tmp);
	} else {
		size_t tmp = strtoull(content, NULL, 10);

		if (tmp == 0 && utf8cmp(content, "0") != 0) {
			char* copy = malloc((len + 1) * sizeof(char));
			strcpy(copy, content);
			utf8lwr(copy);

			if (utf8cmp(copy, "no") == 0) {
				elem->is_percent = true;
				elem->magnitude = rconv_decimal_new_from_double(RCONV_STEPMANIA_FRACTION_PRECISION, 0.0);
			} else {
				elem->name = content;
				free_content = false;
			}
			free(copy);
		} else {
			elem->is_percent = false;
			elem->magnitude = rconv_decimal_new_from_double(RCONV_STEPMANIA_FRACTION_PRECISION, tmp);
		}
	}

	if (free_content) {
		free(content);
	}
}

void
rconv_stepmania_handle_combo_change_entry(RconvStepmaniaComboChange* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	} else if (idx == 1) {
		elem->hit = atoi(content);
	} else if (idx == 2) {
		elem->miss = atoi(content);
	}

	free(content);
}

void
rconv_stepmania_handle_speed_change_entry(RconvStepmaniaSpeedChange* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	} else if (idx == 1) {
		elem->ratio = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	} else if (idx == 2) {
		elem->duration = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	} else if (idx == 3) {
		elem->in_seconds = rconv_parse_bool(content);
	}

	free(content);
}

void
rconv_stepmania_handle_scroll_speed_change_entry(RconvStepmaniaScrollSpeedChange* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	} else if (idx == 1) {
		elem->factor = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	}

	free(content);
}

void
rconv_stepmania_handle_fake_section_entry(RconvStepmaniaFakeSection* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	} else if (idx == 1) {
		elem->duration = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
	}

	free(content);
}

void
rconv_stepmania_handle_label_entry(RconvStepmaniaLabel* elem, int idx, char* content)
{
	if (idx == 0) {
		elem->beat = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
		free(content);
	} else if (idx == 1) {
		elem->content = content;
	}
}

RconvStepmaniaTimedAttack**
rconv_stepmania_parse_timed_attacks(char* data, int* len)
{
	if (data == NULL) {
		*len = 0;
		return NULL;
	}

	RconvList* list = rconv_list();
	RconvStepmaniaTimedAttack* element = rconv_stepmania_new_timed_attack();
	int element_idx = 0;
	int idx = 0;
	bool is_end = false;
	// 0 Beginning
	// 1 Sub-Tag
	// 2 Content
	int state = 0;
	int offset = 0;
	char* content = NULL;
	size_t data_len = strlen(data);
	bool reached_end = false;

	while (true) {
		if (state == 4) {
			rconv_list_add(list, element);
			element = rconv_stepmania_new_timed_attack();
			element_idx++;
			is_end = false;
			state = 0;
		}

		if (reached_end) {
			break;
		}

		int start = -1;
		int last_not_whitespace = -1;

		while ((offset < data_len) && !reached_end) {
			const char c = *(data + offset);
			if (state == 0) {
				if (rconv_is_whitespace(c)) {
					offset++;
					continue;
				}
				start = offset;
				state = 1;
			}

			if (c == '=') {
				state = 2;
				break;

			} else if (c == ':') {
				state = 3;
				break;
			} else if (c == ';') {
				state = 3;
				reached_end = true;
				break;
			}

			if (start == -1) {
				start = offset;
			}
			offset++;
			if (!rconv_is_whitespace(c)) {
				last_not_whitespace = offset;
			}
		}

		if (start > -1) {
			content = rconv_substr(data, start, last_not_whitespace);
			offset++;
		} else {
			content = NULL;
		}

		if (state == 2) {
			bool valid = false;

			if (content != NULL) {
				utf8lwr(content);
				valid = (idx % 3 == 0 && utf8cmp(content, "time") == 0)
						|| (idx % 3 == 1 && ((is_end = utf8cmp(content, "end") == 0) || utf8cmp(content, "len") == 0))
						|| (idx % 3 == 2 && utf8cmp(content, "mods") == 0);
			}

			if (!valid) {
				printf("Invalid attack #%d, part #%d", element_idx + 1, idx + 1);
				free(content);
				rconv_stepmania_free_timed_attack(element);
				rconv_list_free(list);
				*len = 0;
				return NULL;
			}

			state = 2;

		} else if (state == 3) {
			if (idx % 3 == 0) {
				element->time = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);
				state = 0;
			} else if (idx % 3 == 1) {
				RconvDecimal* tmp = rconv_decimal_new_from_string(RCONV_STEPMANIA_FRACTION_PRECISION, content);

				if (is_end) {
					element->length = rconv_decimal_subtract(tmp, element->time);
					free(tmp);
				} else {
					element->length = tmp;
				}

				state = 0;
			} else {
				element->mods = rconv_stepmania_parse_modifiers(content, &element->mods_len);
				state = 4;
			}

			
			idx++;
		}

		free(content);
		content = NULL;
	}

	RconvStepmaniaTimedAttack** out = rconv_list_to_timed_attack_array(list, len);
	rconv_list_free(list);
	return out;
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
	RconvStepmaniaNoteData* data = (RconvStepmaniaNoteData*) calloc(1, sizeof(RconvStepmaniaNoteData));

	if (data == NULL) {
		return;
	}

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
