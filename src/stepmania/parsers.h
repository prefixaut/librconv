#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <utf8.h>

/*
 * Own modules
 */
#include "../common.h"
#include "../floats.h"
#include "../list.h"
#include "types.h"

#define RCONV_STEPMANIA_CLEAR_LIST_NAMED(type, name)        \
	type** items = rconv_list_to_##name##_array(list, len); \
	if (*items == NULL) {                                   \
		*items = NULL;                                      \
	}                                                       \
	rconv_list_free(list);                                  \
	return items;

#define RCONV_STEPMANIA_CLEAR_LIST(type) RCONV_STEPMANIA_CLEAR_LIST_NAMED(type, type)

#define RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN(type, valSep, name, plural)      \
	RCONV_LIST_TO_ARRAY_GEN_NAMED(type, name)                                   \
	type** rconv_stepmania_parse_##plural(char* data, int* len)                 \
	{                                                                           \
		if (data == NULL) {                                                     \
			*len = 0;                                                           \
			return NULL;                                                        \
		}                                                                       \
                                                                                \
		RconvList* list = rconv_list();                                         \
		type* element = NULL;                                                   \
		int elem_idx = 0;                                                       \
		int state = 0;                                                          \
		int idx = 0;                                                            \
		int start = 0;                                                          \
		int offset = 0;                                                         \
		char* content = NULL;                                                   \
		size_t data_len = strlen(data);                                         \
                                                                                \
		while (true) {                                                          \
			if (element == NULL) {                                              \
				element = rconv_stepmania_new_##name();                         \
			}                                                                   \
                                                                                \
			if (state == 2) {                                                   \
				if (rconv_stepmania_verify_##name##_entry(element, elem_idx)) { \
					rconv_list_add(list, element);                              \
					elem_idx++;                                                 \
				} else {                                                        \
					rconv_stepmania_free_##name(element);                       \
				}                                                               \
				element = rconv_stepmania_new_##name();                         \
				state = 0;                                                      \
				idx = 0;                                                        \
			}                                                                   \
                                                                                \
			int start = -1;                                                     \
			int last_not_whitespace = -1;                                       \
                                                                                \
			while (offset < data_len) {                                         \
				const char c = *(data + offset);                                \
				if (state == 0) {                                               \
					if (rconv_is_whitespace(c)) {                               \
						offset++;                                               \
						continue;                                               \
					}                                                           \
					start = offset;                                             \
					state = 1;                                                  \
				}                                                               \
                                                                                \
				if (c == valSep) {                                              \
					state = 1;                                                  \
					break;                                                      \
                                                                                \
				} else if (c == ',') {                                          \
					state = 2;                                                  \
					break;                                                      \
				}                                                               \
                                                                                \
				if (start == -1) {                                              \
					start = offset;                                             \
				}                                                               \
				offset++;                                                       \
				if (!rconv_is_whitespace(c)) {                                  \
					last_not_whitespace = offset;                               \
				}                                                               \
			}                                                                   \
                                                                                \
			if (start > -1) {                                                   \
				content = rconv_substr(data, start, last_not_whitespace);       \
				offset++;                                                       \
			} else {                                                            \
				content = NULL;                                                 \
			}                                                                   \
                                                                                \
			if (state == 1) {                                                   \
				state = 0;                                                      \
			}                                                                   \
                                                                                \
			rconv_stepmania_handle_##name##_entry(element, idx, content);       \
                                                                                \
			idx++;                                                              \
                                                                                \
			if (offset >= data_len) {                                           \
				break;                                                          \
			}                                                                   \
		}                                                                       \
                                                                                \
		if (rconv_stepmania_verify_##name##_entry(element, elem_idx)) {         \
			rconv_list_add(list, element);                                      \
			elem_idx++;                                                         \
		} else {                                                                \
			rconv_stepmania_free_##name(element);                               \
		}                                                                       \
                                                                                \
		type** items = rconv_list_to_##name##_array(list, len);                 \
		rconv_list_free(list);                                                  \
		return items;                                                           \
	}

#define RCONV_STEPMANIA_PARSE_DEFAULT_LIST_ENTRIES_GEN(type, name, plural) RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN(type, ',', name, plural)

#define RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN_H(type, name, plural)                   \
	RCONV_LIST_TO_ARRAY_GEN_H_NAMED(type, name)                                        \
	void rconv_stepmania_handle_##name##_entry(type* element, int idx, char* content); \
	type** rconv_stepmania_parse_##plural(char* data, int* len);

RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN_H(RconvStepmaniaBpmChange, bpm_change, bpm_changes)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN_H(RconvStepmaniaStop, stop, stops)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN_H(RconvStepmaniaTimedAttack, timed_attack, timed_attacks)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN_H(RconvStepmaniaDelay, delay, delays)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN_H(RconvStepmaniaTimeSignature, time_signature, time_signatures)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN_H(RconvStepmaniaInstrumentTrack, instrument_track, instrument_tracks)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN_H(RconvStepmaniaTickCount, tick_count, tick_counts)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN_H(RconvStepmaniaBackgroundChange, background_change, background_changes)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN_H(RconvStepmaniaModifier, modifier, modifiers)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN_H(RconvStepmaniaComboChange, combo_change, combo_changes)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN_H(RconvStepmaniaSpeedChange, speed_change, speed_changes)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN_H(RconvStepmaniaScrollSpeedChange, scroll_speed_change, scroll_speed_changes)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN_H(RconvStepmaniaFakeSection, fake_section, fake_sections)
RCONV_STEPMANIA_PARSE_LIST_ENTRIES_GEN_H(RconvStepmaniaLabel, label, labels)

RCONV_LIST_TO_ARRAY_GEN_H_NAMED(RconvStepmaniaNoteData, note_data)
RCONV_LIST_TO_ARRAY_GEN_H_NAMED(RconvStepmaniaTimedAttack, timed_attack)

void
rconv_stepmania_parse_note_data(char* content, RconvList* list);
