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

RconvStepmaniaColor*
rconv_stepmania_new_color(float red, float green, float blue, float alpha)
{
	float* color = (float*) calloc(4, sizeof(float));
	if (color == NULL) {
		return NULL;
	}
	*(color + 0) = red;
	*(color + 1) = green;
	*(color + 2) = blue;
	*(color + 3) = alpha;

	return (RconvStepmaniaColor*) color;
}

void
rconv_stepmania_parse_tags(FILE* fp, char** tag, char** content)
{
	int state = 0;
	int start = -1;
	int last_not_whitespace = -1;
	*content = NULL;
	*tag = NULL;

	while (feof(fp) == 0) {
		char c = fgetc(fp);

		if (state == 0) {
			if (c == '#') {
				state = 1;
			}
			continue;
		}

		if (state == 1) {
			if (c == ':') {
				int end = ftell(fp);
				int len = last_not_whitespace - start;

				fseek(fp, start, SEEK_SET);
				*tag = malloc((len + 1) * sizeof(char));
				fread(*tag, sizeof(char), len, fp);
				(*tag)[len] = '\0';
				fseek(fp, end, SEEK_SET);

				state = 2;
				start = -1;
				last_not_whitespace = -1;

			} else if (!rconv_is_whitespace(c)) {
				if (start == -1) {
					start = ftell(fp) - 1;
				}
				last_not_whitespace = ftell(fp);
			}

			continue;
		}

		if (state == 2) {
			if (c == ';') {
				int end = ftell(fp);
				int len = last_not_whitespace - start;

				// FIXME: In the attacks, the allocated space is 4 chars too long
				// Other tags work just fine (???)
				fseek(fp, start, SEEK_SET);
				*content = malloc((len + 1) * sizeof(char));
				fread(*content, sizeof(char), len, fp);
				(*content)[len] = '\0';
				fseek(fp, end, SEEK_SET);

				break;

			} else if (!rconv_is_whitespace(c)) {
				if (start == -1) {
					start = ftell(fp) - 1;
				}
				last_not_whitespace = ftell(fp);
			}
		}
	}
}

bool
rconv_stepmania_is_yes(char* content)
{
	utf8lwr(content);
	return utf8cmp(content, "yes") == 0 || utf8cmp(content, "1") == 0 || utf8cmp(content, "es") == 0 || utf8cmp(content, "omes") == 0;
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

			char* elem = rconv_substr(data, start, offset);
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
		char* elem = rconv_substr(data, start, offset);
		rconv_list_add(list, elem);
	}

	RCONV_STEPMANIA_CLEAR_LIST_NAMED(char, string)
}

bool
rconv_stepmania_float_is_zero(RconvFloat* val)
{
	return val == NULL || (val->integer == 0 && val->fraction == 0);
}

/*
 * ==================== BPM CHANGES ====================
 */

RconvStepmaniaBpmChange*
rconv_stepmania_new_bpm_change()
{
	return (RconvStepmaniaBpmChange*) calloc(1, sizeof(RconvStepmaniaBpmChange));
}

bool
rconv_stepmania_verify_bpm_change_entry(RconvStepmaniaBpmChange* elem, int idx)
{
	if (elem == NULL) {
		return false;
	}

	bool has_set = !rconv_stepmania_float_is_zero(elem->bpm);

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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
rconv_stepmania_free_bpm_change(RconvStepmaniaBpmChange* bpm_change)
{
	if (bpm_change == NULL) {
		return;
	}

	free(bpm_change->beat);
	free(bpm_change->bpm);

	free(bpm_change);
}

void
rconv_stepmania_free_all_bpm_changes(int len, RconvStepmaniaBpmChange** bpm_changes)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_bpm_change(*(bpm_changes + i));
	}
	free(bpm_changes);
}

RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES(
	RconvStepmaniaBpmChange,
	rconv_stepmania_new_bpm_change,
	rconv_stepmania_handle_bpm_change_entry,
	rconv_stepmania_verify_bpm_change_entry,
	rconv_stepmania_free_bpm_change)

/*
 * ==================== STOPS ====================
 */

RconvStepmaniaStop*
rconv_stepmania_new_stop()
{
	return (RconvStepmaniaStop*) calloc(1, sizeof(RconvStepmaniaStop));
}

bool
rconv_stepmania_verify_stop_entry(RconvStepmaniaStop* elem, int idx)
{
	if (elem == NULL) {
		return false;
	}

	bool has_set = !rconv_stepmania_float_is_zero(elem->duration);

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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
rconv_stepmania_free_stop(RconvStepmaniaStop* stop)
{
	if (stop == NULL) {
		return;
	}

	free(stop->beat);
	free(stop->duration);

	free(stop);
}

void
rconv_stepmania_free_all_stops(int len, RconvStepmaniaStop** stops)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_stop(*(stops + i));
	}
	free(stops);
}

RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES(
	RconvStepmaniaStop,
	rconv_stepmania_new_stop,
	rconv_stepmania_handle_stop_entry,
	rconv_stepmania_verify_stop_entry,
	rconv_stepmania_free_stop)

/*
 * ==================== DELAYS ====================
 */

RconvStepmaniaDelay*
rconv_stepmania_new_delay()
{
	return (RconvStepmaniaDelay*) calloc(1, sizeof(RconvStepmaniaDelay));
}

bool
rconv_stepmania_verify_delay_entry(RconvStepmaniaDelay* elem, int idx)
{
	if (elem == NULL) {
		return false;
	}

	bool has_set = !rconv_stepmania_float_is_zero(elem->duration);

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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
rconv_stepmania_free_delay(RconvStepmaniaDelay* delay)
{
	if (delay == NULL) {
		return;
	}

	free(delay->beat);
	free(delay->duration);

	free(delay);
}

void
rconv_stepmania_free_all_delays(int len, RconvStepmaniaDelay** delays)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_delay(*(delays + i));
	}
	free(delays);
}

RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES(
	RconvStepmaniaDelay,
	rconv_stepmania_new_delay,
	rconv_stepmania_handle_delay_entry,
	rconv_stepmania_verify_delay_entry,
	rconv_stepmania_free_delay)

/*
 * ==================== TIME SIGNATURES ====================
 */

RconvStepmaniaTimeSignature*
rconv_stepmania_new_time_signature()
{
	return (RconvStepmaniaTimeSignature*) calloc(1, sizeof(RconvStepmaniaTimeSignature));
}

bool
rconv_stepmania_verify_time_signature_entry(RconvStepmaniaTimeSignature* elem, int idx)
{
	if (elem == NULL) {
		return false;
	}

	bool has_set = elem->numerator > 0 && elem->denominator > 0;

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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
rconv_stepmania_free_time_signature(RconvStepmaniaTimeSignature* time_signature)
{
	if (time_signature == NULL) {
		return;
	}

	free(time_signature->beat);

	free(time_signature);
}

void
rconv_stepmania_free_all_time_signatures(int len, RconvStepmaniaTimeSignature** time_signatures)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_time_signature(*(time_signatures + i));
	}
	free(time_signatures);
}

RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES(
	RconvStepmaniaTimeSignature,
	rconv_stepmania_new_time_signature,
	rconv_stepmania_handle_time_signature_entry,
	rconv_stepmania_verify_time_signature_entry,
	rconv_stepmania_free_time_signature)

/*
 * ==================== INSTRUMENT TRACKS ====================
 */

RconvStepmaniaInstrumentTrack*
rconv_stepmania_new_instrument_track()
{
	return (RconvStepmaniaInstrumentTrack*) calloc(1, sizeof(RconvStepmaniaInstrumentTrack));
}

bool
rconv_stepmania_verify_instrument_track_entry(RconvStepmaniaInstrumentTrack* elem, int idx)
{
	if (elem == NULL) {
		return false;
	}

	return elem->instrument != NULL && strlen(elem->instrument) > 0 && elem->file != NULL && strlen(elem->file) > 0;
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
rconv_stepmania_free_instrument_track(RconvStepmaniaInstrumentTrack* instrument_track)
{
	if (instrument_track == NULL) {
		return;
	}

	free(instrument_track->file);
	free(instrument_track->instrument);

	free(instrument_track);
}

void
rconv_stepmania_free_all_instrument_tracks(int len, RconvStepmaniaInstrumentTrack** instrument_tracks)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_instrument_track(*(instrument_tracks + i));
	}
	free(instrument_tracks);
}

RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES(
	RconvStepmaniaInstrumentTrack,
	rconv_stepmania_new_instrument_track,
	rconv_stepmania_handle_instrument_track_entry,
	rconv_stepmania_verify_instrument_track_entry,
	rconv_stepmania_free_instrument_track)

/*
 * ==================== TICK COUNTS ====================
 */

RconvStepmaniaTickCount*
rconv_stepmania_new_tick_count()
{
	return (RconvStepmaniaTickCount*) calloc(1, sizeof(RconvStepmaniaTickCount));
}

bool
rconv_stepmania_verify_tick_count_entry(RconvStepmaniaTickCount* elem, int idx)
{
	if (elem == NULL) {
		return false;
	}

	bool has_set = elem->count > 0;

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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
rconv_stepmania_free_tick_count(RconvStepmaniaTickCount* tick_count)
{
	if (tick_count == NULL) {
		return;
	}

	free(tick_count->beat);

	free(tick_count);
}

void
rconv_stepmania_free_all_tick_counts(int len, RconvStepmaniaTickCount** tick_counts)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_tick_count(*(tick_counts + i));
	}
	free(tick_counts);
}

RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES(
	RconvStepmaniaTickCount,
	rconv_stepmania_new_tick_count,
	rconv_stepmania_handle_tick_count_entry,
	rconv_stepmania_verify_tick_count_entry,
	rconv_stepmania_free_tick_count)

/*
 * ==================== BACKGROUND CHANGES ====================
 */

RconvStepmaniaBackgroundChange*
rconv_stepmania_new_background_change()
{
	RconvStepmaniaBackgroundChange* bg = (RconvStepmaniaBackgroundChange*) calloc(1, sizeof(RconvStepmaniaBackgroundChange));

	if (bg == NULL) {
		return NULL;
	}

	bg->color1 = rconv_stepmania_new_color(0.0, 0.0, 0.0, 255.0);
	bg->color2 = rconv_stepmania_new_color(0.0, 0.0, 0.0, 255.0);

	return bg;
}

bool
rconv_stepmania_verify_background_change_entry(RconvStepmaniaBackgroundChange* elem, int idx)
{
	if (elem == NULL) {
		return false;
	}

	bool has_set = elem->path != NULL || !rconv_stepmania_float_is_zero(elem->update_rate) || elem->crossfade || elem->stretch_rewind
				   || elem->stretch_no_loop || elem->effect != NULL || elem->file2 != NULL || elem->transition != NULL
				   || elem->color1 != NULL || elem->color2 != NULL;

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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
rconv_stepmania_free_background_change(RconvStepmaniaBackgroundChange* background_change)
{
	if (background_change == NULL) {
		return;
	}

	free(background_change->beat);
	free(background_change->path);
	free(background_change->update_rate);
	free(background_change->effect);
	free(background_change->file2);
	free(background_change->transition);
	free(background_change->color1);
	free(background_change->color2);

	free(background_change);
}

void
rconv_stepmania_free_all_background_changes(int len, RconvStepmaniaBackgroundChange** background_changes)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_background_change(*(background_changes + i));
	}
	free(background_changes);
}

RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES(
	RconvStepmaniaBackgroundChange,
	rconv_stepmania_new_background_change,
	rconv_stepmania_handle_background_change_entry,
	rconv_stepmania_verify_background_change_entry,
	rconv_stepmania_free_background_change)

/*
 * ==================== MODIFIERS ====================
 */

RconvStepmaniaModifier*
rconv_stepmania_new_modifier()
{
	RconvStepmaniaModifier* out = (RconvStepmaniaModifier*) calloc(1, sizeof(RconvStepmaniaModifier));
	out->approach_rate = 1;
	out->magnitude = rconv_float_new_from_number(100, 0, 0);
	out->is_percent = true;
}

bool
rconv_stepmania_verify_modifier_entry(RconvStepmaniaModifier* elem, int idx)
{
	if (elem == NULL) {
		return false;
	}

	return elem->name != NULL && strlen(elem->name) > 0
		&& elem->player != NULL && strlen(elem->player) > 0
		&& elem->approach_rate > 0
		&& elem->magnitude != NULL;
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
		elem->magnitude = rconv_float_new_from_string(tmp);
		free(tmp);
	} else {
		size_t tmp = strtoull(content, NULL, 10);

		if (tmp == 0 && utf8cmp(content, "0") != 0) {
			char* copy = malloc((len + 1) * sizeof(char));
			strcpy(copy, content);
			utf8lwr(copy);

			if (utf8cmp(copy, "no") == 0) {
				elem->is_percent = true;
				elem->magnitude = rconv_float_new_from_number(0, 0, 0);
			} else {
				elem->name = content;
				free_content = false;
			}
			free(copy);
		} else {
			elem->is_percent = false;
			elem->magnitude = rconv_float_new_from_number(tmp, 0, 0);
		}
	}

	if (free_content) {
		free(content);
	}
}

void
rconv_stepmania_free_modifier(RconvStepmaniaModifier* modifier)
{
	if (modifier == NULL) {
		return;
	}

	free(modifier->name);
	free(modifier->player);
	free(modifier->magnitude);

	free(modifier);
}

void
rconv_stepmania_free_all_modifiers(int len, RconvStepmaniaModifier** modifiers)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_modifier(*(modifiers + i));
	}
	free(modifiers);
}

RCONV_STEPMANIA_PARSE_LIST_ENTRIES(
	RconvStepmaniaModifier,
	' ',
	rconv_stepmania_new_modifier,
	rconv_stepmania_handle_modifier_entry,
	rconv_stepmania_verify_modifier_entry,
	rconv_stepmania_free_modifier)

/*
 * ==================== ATTACKS ====================
 */

void
rconv_stepmania_free_attack(RconvStepmaniaAttack* attack)
{
	if (attack == NULL) {
		return;
	}

	for (int i = 0; i < attack->mods_len; i++) {
		free(*(attack->mods + i));
	}

	free(attack->length);
	free(attack->mods);

	free(attack);
}

void
rconv_stepmania_free_all_attacks(int len, RconvStepmaniaAttack** attacks)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_attack(*(attacks + i));
	}
	free(attacks);
}

/*
 * ==================== TIMED ATTACKS ====================
 */

RconvStepmaniaTimedAttack*
rconv_stepmania_new_timed_attack()
{
	return (RconvStepmaniaTimedAttack*) calloc(1, sizeof(RconvStepmaniaTimedAttack));
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
				element->time = rconv_float_new_from_string(content);
				state = 0;
			} else if (idx % 3 == 1) {
				RconvFloat* tmp = rconv_float_new_from_string(content);

				if (is_end) {
					element->length = rconv_float_subtract(tmp, element->time);
					free(tmp);
				} else {
					element->length = tmp;
				}

				state = 0;
			} else {
				element->mods = rconv_stepmania_parse_RconvStepmaniaModifier_list_entries(content, &element->mods_len);
				state = 4;
			}

			
			idx++;
		}

		free(content);
		content = NULL;
	}

	RconvStepmaniaTimedAttack** out = rconv_list_to_RconvStepmaniaTimedAttack_array(list, len);
	rconv_list_free(list);
	return out;
}

void
rconv_stepmania_free_timed_attack(RconvStepmaniaTimedAttack* timed_attack)
{
	if (timed_attack == NULL) {
		return;
	}

	for (int i = 0; i < timed_attack->mods_len; i++) {
		rconv_stepmania_free_modifier(*(timed_attack->mods + 1));
	}

	free(timed_attack->time);
	free(timed_attack->length);
	free(timed_attack->mods);

	free(timed_attack);
}

void
rconv_stepmania_free_all_timed_attacks(int len, RconvStepmaniaTimedAttack** timed_attacks)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_timed_attack(*(timed_attacks + i));
	}
	free(timed_attacks);
}

/*
 * ==================== COMBO CHANGES ====================
 */

RconvStepmaniaComboChange*
rconv_stepmania_new_combo_change()
{
	return (RconvStepmaniaComboChange*) calloc(1, sizeof(RconvStepmaniaComboChange));
}

bool
rconv_stepmania_verify_combo_change_entry(RconvStepmaniaComboChange* elem, int idx)
{
	if (elem == NULL) {
		return false;
	}

	bool has_set = elem->hit > 0 && elem->miss > 0;

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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
rconv_stepmania_free_combo_change(RconvStepmaniaComboChange* combo_change)
{
	if (combo_change == NULL) {
		return;
	}

	free(combo_change->beat);

	free(combo_change);
}

void
rconv_stepmania_free_all_combo_changes(int len, RconvStepmaniaComboChange** combo_changes)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_combo_change(*(combo_changes + i));
	}
	free(combo_changes);
}

RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES(
	RconvStepmaniaComboChange,
	rconv_stepmania_new_combo_change,
	rconv_stepmania_handle_combo_change_entry,
	rconv_stepmania_verify_combo_change_entry,
	rconv_stepmania_free_combo_change)

/*
 * ==================== SPEED CHANGES ====================
 */

RconvStepmaniaSpeedChange*
rconv_stepmania_new_speed_change()
{
	return (RconvStepmaniaSpeedChange*) calloc(1, sizeof(RconvStepmaniaSpeedChange));
}

bool
rconv_stepmania_verify_speed_change_entry(RconvStepmaniaSpeedChange* elem, int idx)
{
	if (elem == NULL) {
		return false;
	}

	bool has_set = !rconv_stepmania_float_is_zero(elem->duration) && !rconv_stepmania_float_is_zero(elem->ratio);

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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
rconv_stepmania_free_speed_change(RconvStepmaniaSpeedChange* speed_change)
{
	if (speed_change == NULL) {
		return;
	}

	free(speed_change->beat);
	free(speed_change->ratio);
	free(speed_change->duration);

	free(speed_change);
}

void
rconv_stepmania_free_all_speed_changes(int len, RconvStepmaniaSpeedChange** speed_changes)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_speed_change(*(speed_changes + i));
	}
	free(speed_changes);
}

RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES(
	RconvStepmaniaSpeedChange,
	rconv_stepmania_new_speed_change,
	rconv_stepmania_handle_speed_change_entry,
	rconv_stepmania_verify_speed_change_entry,
	rconv_stepmania_free_speed_change)

/*
 * ==================== SCROLL SPEED CHANGES ====================
 */

RconvStepmaniaScrollSpeedChange*
rconv_stepmania_new_scroll_speed_change()
{
	return (RconvStepmaniaScrollSpeedChange*) calloc(1, sizeof(RconvStepmaniaScrollSpeedChange));
}

bool
rconv_stepmania_verify_scroll_speed_change_entry(RconvStepmaniaScrollSpeedChange* elem, int idx)
{
	if (elem == NULL) {
		return false;
	}

	bool has_set = !rconv_stepmania_float_is_zero(elem->factor);

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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
rconv_stepmania_free_scroll_speed_change(RconvStepmaniaScrollSpeedChange* scroll_speed_change)
{
	if (scroll_speed_change == NULL) {
		return;
	}

	free(scroll_speed_change->beat);
	free(scroll_speed_change->factor);

	free(scroll_speed_change);
}

void
rconv_stepmania_free_all_scroll_speed_changes(int len, RconvStepmaniaScrollSpeedChange** scroll_speed_changes)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_scroll_speed_change(*(scroll_speed_changes + i));
	}
	free(scroll_speed_changes);
}

RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES(
	RconvStepmaniaScrollSpeedChange,
	rconv_stepmania_new_scroll_speed_change,
	rconv_stepmania_handle_scroll_speed_change_entry,
	rconv_stepmania_verify_scroll_speed_change_entry,
	rconv_stepmania_free_scroll_speed_change)

/*
 * ==================== FAKE SECTIONS ====================
 */

RconvStepmaniaFakeSection*
rconv_stepmania_new_fake_section()
{
	return (RconvStepmaniaFakeSection*) calloc(1, sizeof(RconvStepmaniaFakeSection));
}

bool
rconv_stepmania_verify_fake_section_entry(RconvStepmaniaFakeSection* elem, int idx)
{
	if (elem == NULL) {
		return false;
	}

	bool has_set = rconv_stepmania_float_is_zero(elem->duration);

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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
rconv_stepmania_free_fake_section(RconvStepmaniaFakeSection* fake_section)
{
	if (fake_section == NULL) {
		return;
	}

	free(fake_section->beat);
	free(fake_section->duration);

	free(fake_section);
}

void
rconv_stepmania_free_all_fake_sections(int len, RconvStepmaniaFakeSection** fake_sections)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_fake_section(*(fake_sections + i));
	}
	free(fake_sections);
}

RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES(
	RconvStepmaniaFakeSection,
	rconv_stepmania_new_fake_section,
	rconv_stepmania_handle_fake_section_entry,
	rconv_stepmania_verify_fake_section_entry,
	rconv_stepmania_free_fake_section)

/*
 * ==================== LABELS ====================
 */

RconvStepmaniaLabel*
rconv_stepmania_new_label()
{
	return (RconvStepmaniaLabel*) calloc(1, sizeof(RconvStepmaniaLabel));
}

bool
rconv_stepmania_verify_label_entry(RconvStepmaniaLabel* elem, int idx)
{
	if (elem == NULL) {
		return false;
	}

	bool has_set = elem->content != NULL;

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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

void
rconv_stepmania_free_label(RconvStepmaniaLabel* label)
{
	if (label == NULL) {
		return;
	}

	free(label->beat);
	free(label->content);

	free(label);
}

void
rconv_stepmania_free_all_labels(int len, RconvStepmaniaLabel** labels)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_label(*(labels + i));
	}
	free(labels);
}

RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES(
	RconvStepmaniaLabel,
	rconv_stepmania_new_label,
	rconv_stepmania_handle_label_entry,
	rconv_stepmania_verify_label_entry,
	rconv_stepmania_free_label)

/*
 * ==================== NOTES ====================
 */

RconvStepmaniaNote*
rconv_stepmania_new_note()
{
	return (RconvStepmaniaNote*) calloc(1, sizeof(RconvStepmaniaNote));
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

	for (int i = 0; i < note->modifiers_len; i++) {
		rconv_stepmania_free_modifier(*(note->modifiers + i));
	}

	free(note->attack);
	free(note->modifiers);

	free(note);
}

void
rconv_stepmania_free_all_notes(int len, RconvStepmaniaNote** notes)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_note(*(notes + i));
	}
	free(notes);
}

/*
 * ==================== BEATS ====================
 */

RconvStepmaniaBeat*
rconv_stepmania_new_beat()
{
	return (RconvStepmaniaBeat*) calloc(1, sizeof(RconvStepmaniaBeat));
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

	free(beat);
}

void
rconv_stepmania_free_all_beats(int len, RconvStepmaniaBeat** beats)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_beat(*(beats + i));
	}
	free(beats);
}

/*
 * ==================== NOTE DATA ====================
 */

RconvStepmaniaNoteData*
rconv_stepmania_new_note_data()
{
	return (RconvStepmaniaNoteData*) calloc(1, sizeof(RconvStepmaniaNoteData));
}

void
rconv_stepmania_free_note_data(RconvStepmaniaNoteData* note_data)
{
	if (note_data == NULL) {
		return;
	}

	for (int i = 0; i < note_data->beats_len; i++) {
		rconv_stepmania_free_beat(*(note_data->beats + i));
	}

	free(note_data->description);
	free(note_data->beats);

	free(note_data);
}

void
rconv_stepmania_free_all_note_data(int len, RconvStepmaniaNoteData** note_data)
{
	for (int i = 0; i < len; i++) {
		rconv_stepmania_free_note_data(*(note_data + i));
	}
	free(note_data);
}

/*
 * ==================== CHART FILE ====================
 */

RconvStepmaniaChartFile*
rconv_stepmania_new_chart_file()
{
	return (RconvStepmaniaChartFile*) calloc(1, sizeof(RconvStepmaniaChartFile));
}

void
rconv_stepmania_free_chart_file(RconvStepmaniaChartFile* chart)
{
	if (chart == NULL) {
		return;
	}

	free(chart->title);
	free(chart->subtitle);
	free(chart->artist);
	free(chart->title_transliterated);
	free(chart->subtitle_transliterated);
	free(chart->artist_transliterated);
	free(chart->genre);
	free(chart->credit);
	free(chart->banner);
	free(chart->background);
	free(chart->lyrics);
	free(chart->cd_title);
	free(chart->music);
	free(chart->sample_start);
	free(chart->sample_length);
	free(chart->display_bpm);
	free(chart->offset);

	rconv_free_all_strings(chart->key_sounds_len, chart->key_sounds);
	rconv_stepmania_free_all_instrument_tracks(chart->instrument_tracks_len, chart->instrument_tracks);
	rconv_stepmania_free_all_background_changes(chart->background_changes_len, chart->background_changes);
	rconv_stepmania_free_all_background_changes(chart->background_changes2_len, chart->background_changes2);
	rconv_stepmania_free_all_background_changes(chart->background_changes3_len, chart->background_changes3);
	rconv_stepmania_free_all_background_changes(chart->animations_len, chart->animations);
	rconv_stepmania_free_all_background_changes(chart->foreground_changes_len, chart->foreground_changes);
	rconv_stepmania_free_all_stops(chart->stops_len, chart->stops);
	rconv_stepmania_free_all_bpm_changes(chart->bpms_len, chart->bpms);
	rconv_stepmania_free_all_time_signatures(chart->time_signatures_len, chart->time_signatures);
	rconv_stepmania_free_all_timed_attacks(chart->attacks_len, chart->attacks);
	rconv_stepmania_free_all_delays(chart->delays_len, chart->delays);
	rconv_stepmania_free_all_tick_counts(chart->tick_counts_len, chart->tick_counts);
	rconv_stepmania_free_all_note_data(chart->note_data_len, chart->note_data);
	rconv_stepmania_free_all_combo_changes(chart->combos_len, chart->combos);
	rconv_stepmania_free_all_speed_changes(chart->speeds_len, chart->speeds);
	rconv_stepmania_free_all_scroll_speed_changes(chart->scrolls_len, chart->scrolls);
	rconv_stepmania_free_all_fake_sections(chart->fakes_len, chart->fakes);
	rconv_stepmania_free_all_labels(chart->labels_len, chart->labels);

	free(chart);
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

RconvStepmaniaChartFile*
rconv_stepmania_parse(FILE* fp)
{
	RconvStepmaniaChartFile* chart = (RconvStepmaniaChartFile*) calloc(1, sizeof(RconvStepmaniaChartFile));
	if (chart == NULL) {
		return NULL;
	}

	char* tag = NULL;
	char* content = NULL;
	RconvList* notes = rconv_list();

	while (true) {
		rconv_stepmania_parse_tags(fp, &tag, &content);
		if (tag == NULL) {
			if (content != NULL) {
				free(content);
			}
			break;
		}

		utf8lwr(tag);

		bool free_content = true;
		if (utf8cmp(tag, "title") == 0) {
			free(chart->title);
			chart->title = content;
			free_content = false;
		} else if (utf8cmp(tag, "subtitle") == 0) {
			free(chart->subtitle);
			chart->subtitle = content;
			free_content = false;
		} else if (utf8cmp(tag, "artist") == 0) {
			free(chart->artist);
			chart->artist = content;
			free_content = false;
		} else if (utf8cmp(tag, "titletranslit") == 0) {
			free(chart->title_transliterated);
			chart->title_transliterated = content;
			free_content = false;
		} else if (utf8cmp(tag, "subtitletranslit") == 0) {
			free(chart->subtitle_transliterated);
			chart->subtitle_transliterated = content;
			free_content = false;
		} else if (utf8cmp(tag, "artisttranslit") == 0) {
			free(chart->artist_transliterated);
			chart->artist_transliterated = content;
			free_content = false;
		} else if (utf8cmp(tag, "genre") == 0) {
			free(chart->genre);
			chart->genre = content;
			free_content = false;
		} else if (utf8cmp(tag, "credit") == 0) {
			free(chart->credit);
			chart->credit = content;
			free_content = false;
		} else if (utf8cmp(tag, "banner") == 0) {
			free(chart->banner);
			chart->banner = content;
			free_content = false;
		} else if (utf8cmp(tag, "background") == 0) {
			free(chart->background);
			chart->background = content;
			free_content = false;
		} else if (utf8cmp(tag, "lyricspath") == 0) {
			free(chart->lyrics);
			chart->lyrics = content;
			free_content = false;
		} else if (utf8cmp(tag, "cdtitle") == 0) {
			free(chart->cd_title);
			chart->cd_title = content;
			free_content = false;
		} else if (utf8cmp(tag, "music") == 0) {
			free(chart->music);
			chart->music = content;
			free_content = false;
		} else if (utf8cmp(tag, "displaybmp") == 0) {
			free(chart->display_bpm);
			chart->display_bpm = content;
			free_content = false;
		} else if (utf8cmp(tag, "samplestart") == 0) {
			free(chart->sample_start);
			chart->sample_start = rconv_float_new_from_string(content);
		} else if (utf8cmp(tag, "samplelength") == 0) {
			free(chart->sample_length);
			chart->sample_length = rconv_float_new_from_string(content);
		} else if (utf8cmp(tag, "offset") == 0) {
			free(chart->offset);
			chart->offset = rconv_float_new_from_string(content);
		} else if (utf8cmp(tag, "selectable") == 0) {
			chart->selectable = rconv_stepmania_is_yes(content);
		} else if (utf8cmp(tag, "bgchanges") == 0) {
			rconv_stepmania_free_all_background_changes(chart->background_changes_len, chart->background_changes);
			chart->background_changes =
				rconv_stepmania_parse_RconvStepmaniaBackgroundChange_list_entries(content, &chart->background_changes_len);
		} else if (utf8cmp(tag, "bgchanges2") == 0) {
			rconv_stepmania_free_all_background_changes(chart->background_changes2_len, chart->background_changes2);
			chart->background_changes2 =
				rconv_stepmania_parse_RconvStepmaniaBackgroundChange_list_entries(content, &chart->background_changes2_len);
		} else if (utf8cmp(tag, "bgchanges3") == 0) {
			rconv_stepmania_free_all_background_changes(chart->background_changes3_len, chart->background_changes3);
			chart->background_changes3 =
				rconv_stepmania_parse_RconvStepmaniaBackgroundChange_list_entries(content, &chart->background_changes3_len);
		} else if (utf8cmp(tag, "animations") == 0) {
			rconv_stepmania_free_all_background_changes(chart->animations_len, chart->animations);
			chart->animations = rconv_stepmania_parse_RconvStepmaniaBackgroundChange_list_entries(content, &chart->animations_len);
		} else if (utf8cmp(tag, "fgchanges") == 0) {
			rconv_stepmania_free_all_background_changes(chart->foreground_changes_len, chart->foreground_changes);
			chart->foreground_changes =
				rconv_stepmania_parse_RconvStepmaniaBackgroundChange_list_entries(content, &chart->foreground_changes_len);
		} else if (utf8cmp(tag, "keysounds") == 0) {
			rconv_free_all_strings(chart->key_sounds_len, chart->key_sounds);
			chart->key_sounds = rconv_stepmania_parse_string_list(content, &chart->key_sounds_len);
		} else if (utf8cmp(tag, "stops") == 0 || utf8cmp(tag, "freezes") == 0) {
			rconv_stepmania_free_all_stops(chart->stops_len, chart->stops);
			chart->stops = rconv_stepmania_parse_RconvStepmaniaStop_list_entries(content, &chart->stops_len);
		} else if (utf8cmp(tag, "bpms") == 0) {
			rconv_stepmania_free_all_bpm_changes(chart->bpms_len, chart->bpms);
			chart->bpms = rconv_stepmania_parse_RconvStepmaniaBpmChange_list_entries(content, &chart->bpms_len);
		} else if (utf8cmp(tag, "timesignatures") == 0) {
			rconv_stepmania_free_all_time_signatures(chart->time_signatures_len, chart->time_signatures);
			chart->time_signatures = rconv_stepmania_parse_RconvStepmaniaTimeSignature_list_entries(content, &chart->time_signatures_len);
		} else if (utf8cmp(tag, "instrumenttracks") == 0) {
			rconv_stepmania_free_all_instrument_tracks(chart->instrument_tracks_len, chart->instrument_tracks);
			chart->instrument_tracks =
				rconv_stepmania_parse_RconvStepmaniaInstrumentTrack_list_entries(content, &chart->instrument_tracks_len);
		} else if (utf8cmp(tag, "attacks") == 0) {
			rconv_stepmania_free_all_timed_attacks(chart->attacks_len, chart->attacks);
			chart->attacks = rconv_stepmania_parse_timed_attacks(content, &chart->attacks_len);
		} else if (utf8cmp(tag, "delays") == utf8cmp(tag, "warps") == 0) {
			rconv_stepmania_free_all_delays(chart->delays_len, chart->delays);
			chart->delays = rconv_stepmania_parse_RconvStepmaniaDelay_list_entries(content, &chart->delays_len);
		} else if (utf8cmp(tag, "tickcounts") == 0) {
			rconv_stepmania_free_all_tick_counts(chart->tick_counts_len, chart->tick_counts);
			chart->tick_counts = rconv_stepmania_parse_RconvStepmaniaTickCount_list_entries(content, &chart->tick_counts_len);
		} else if (utf8cmp(tag, "notes") == 0) {
			rconv_stepmania_parse_note_data(content, notes);
		} else if (utf8cmp(tag, "combos") == 0) {
			rconv_stepmania_free_all_combo_changes(chart->combos_len, chart->combos);
			chart->combos = rconv_stepmania_parse_RconvStepmaniaComboChange_list_entries(content, &chart->combos_len);
		} else if (utf8cmp(tag, "speeds") == 0) {
			rconv_stepmania_free_all_speed_changes(chart->speeds_len, chart->speeds);
			chart->speeds = rconv_stepmania_parse_RconvStepmaniaSpeedChange_list_entries(content, &chart->speeds_len);
		} else if (utf8cmp(tag, "scrolls") == 0) {
			rconv_stepmania_free_all_scroll_speed_changes(chart->scrolls_len, chart->scrolls);
			chart->scrolls = rconv_stepmania_parse_RconvStepmaniaScrollSpeedChange_list_entries(content, &chart->scrolls_len);
		} else if (utf8cmp(tag, "fakes") == 0) {
			rconv_stepmania_free_all_fake_sections(chart->fakes_len, chart->fakes);
			chart->fakes = rconv_stepmania_parse_RconvStepmaniaFakeSection_list_entries(content, &chart->fakes_len);
		} else if (utf8cmp(tag, "labels") == 0) {
			rconv_stepmania_free_all_labels(chart->labels_len, chart->labels);
			chart->labels = rconv_stepmania_parse_RconvStepmaniaLabel_list_entries(content, &chart->labels_len);
		} else {
			printf("unknown tag '%s'!\n", tag);
		}

		free(tag);
		if (free_content) {
			free(content);
		}
	}

	return chart;
}

RconvStepmaniaChartFile*
rconv_stepmania_parse_from_string(char* data)
{
	// if (data == NULL) {
	// 	return NULL;
	// }

	// size_t size = strlen(data);
	// FILE* fp;
	// RconvStepmaniaChartFile* out = NULL;

	// #ifdef __linux__
	// 	fp = = open_memstream(&data, &size);
	// 	out = rconv_stepmania_parse(fp);

	// 	fclose(fp);
	// #elif _WIN32
	// 	fmem* buffer = malloc(sizeof(fmem));
	// 	fmem_init(buffer);
	// 	fmem_mem(buffer, *data, size);

	// 	fp = fmem_open(buffer, "r");
	// 	out = rconv_stepmania_parse(fp);

	// 	fclose(fp);
	// 	fmem_term(buffer);
	// #endif

	// return out;
	return NULL;
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

	RconvStepmaniaChartFile* out = rconv_stepmania_parse(fp);
	fclose(fp);

	return out;
}