#pragma once

/*
 * Libraries
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utf8.h>
// #include <fmem.h>

/*
 * Own modules
 */
#include "common.h"
#include "floats.h"
#include "list.h"

/*
 * Definitions
 */
#define RCONV_STEPMANIA_SPECIAL_NOTE_START 'D'

#define RCONV_STEPMANIA_CLEAR_LIST_NAMED(type, name)        \
	type** items = rconv_list_to_##name##_array(list, len); \
	if (*items == NULL) {                                   \
		*items = NULL;                                      \
	}                                                       \
	rconv_list_free(list);                                  \
	return items;

#define RCONV_STEPMANIA_CLEAR_LIST(type) RCONV_STEPMANIA_CLEAR_LIST_NAMED(type, type)

#define RCONV_STEPMANIA_PARSE_LIST_ENTRIES(type, newFn, idxFn, checkFn, freeFn) \
	type** rconv_stepmania_parse_##type##_list_entries(char* data, int* len)    \
	{                                                                           \
		printf("parsing entries for %s!\n", #type);                             \
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
				element = newFn();                                              \
			}                                                                   \
                                                                                \
			if (state == 2) {                                                   \
				if (checkFn(element, elem_idx)) {                               \
					rconv_list_add(list, element);                              \
					elem_idx++;                                                 \
				} else {                                                        \
					freeFn(element);                                            \
				}                                                               \
				element = newFn();                                              \
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
				if (c == '=') {                                                 \
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
			if (content == NULL) {                                              \
				printf("break cuz content null in %s!\n", #type);               \
				break;                                                          \
			}                                                                   \
			idxFn(element, idx, content);                                       \
                                                                                \
			idx++;                                                              \
		}                                                                       \
		printf("type to list end: list(%d)\n", list->size);                     \
		type** items = rconv_list_to_##type##_array(list, len);                 \
		rconv_list_free(list);                                                  \
		return items;                                                           \
	}

/*
 * Types
 */
typedef enum {
	EMPTY = '0',
	NOTE = '1',
	HOLD = '2',
	HOLD_END = '3',
	ROLL = '4',
	MINE = 'M',
	LIFT = 'L',
	FAKE = 'F',
} RconvStepmaniaNoteType;

typedef enum {
	DANCE_SINGLE,
	DANCE_DOUBLE,
	DANCE_COUPLE,
	DANCE_ROUTINE,
	DANCE_SOLO,
	PUMP_SINGLE,
	PUMP_HALF_DOUBLE,
	PUMP_DOUBLE,
	PUMP_COUPLE,
} RconvStepmaniaChartType;

typedef enum {
	BEGINNER,
	EASY,
	MEDIUM,
	HARD,
	CHALLANGE,
	EDIT,
} RconvStepmaniaDifficulty;

typedef float RconvStepmaniaColor[4];

typedef float RconvStepmaniaRadarValues[5];

typedef struct {
	RconvFloat* beat;
	RconvFloat* bpm;
} RconvStepmaniaBpmChange;

typedef struct {
	RconvFloat* beat;
	RconvFloat* duration;
} RconvStepmaniaStop;

typedef struct {
	RconvFloat* beat;
	RconvFloat* duration;
} RconvStepmaniaDelay;

typedef struct {
	RconvFloat* beat;
	int numerator;
	int denominator;
} RconvStepmaniaTimeSignature;

typedef struct {
	char* instrument;
	char* file;
} RconvStepmaniaInstrumentTrack;

typedef struct {
	RconvFloat* beat;
	int count;
} RconvStepmaniaTickCount;

typedef struct {
	RconvFloat* beat;
	char* path;
	RconvFloat* update_rate;
	bool crossfade;
	bool stretch_rewind;
	bool stretch_no_loop;
	char* effect;
	char* file2;
	char* transition;
	RconvStepmaniaColor* color1;
	RconvStepmaniaColor* color2;
} RconvStepmaniaBackgroundChange;

typedef struct {
	char* name;
	char* player;
	int approach_rate;
	RconvFloat* magnitude;
	bool is_percent;
} RconvStepmaniaModifier;

typedef struct {
	RconvFloat* length;
	int mods_len;
	char** mods;
} RconvStepmaniaAttack;

typedef struct {
	RconvFloat* time;
	RconvFloat* length;
	int mods_len;
	RconvStepmaniaModifier** mods;
} RconvStepmaniaTimedAttack;

typedef struct {
	RconvFloat* beat;
	int hit;
	int miss;
} RconvStepmaniaComboChange;

typedef struct {
	RconvFloat* beat;
	RconvFloat* ratio;
	RconvFloat* duration;
	bool in_seconds;
} RconvStepmaniaSpeedChange;

typedef struct {
	RconvFloat* beat;
	RconvFloat* factor;
} RconvStepmaniaScrollSpeedChange;

typedef struct {
	RconvFloat* beat;
	RconvFloat* duration;
} RconvStepmaniaFakeSection;

typedef struct {
	RconvFloat* beat;
	char* content;
} RconvStepmaniaLabel;

typedef struct {
	RconvStepmaniaNoteType type;
	int snap;
	int column;
	int attack_len;
	RconvStepmaniaAttack** attack;
	int key_sound;
	int modifiers_len;
	RconvStepmaniaModifier** modifiers;
	int release_beat;
	int release_snap;
	bool release_lift;
} RconvStepmaniaNote;

typedef struct {
	int index;
	int snap_size;
	int notes_len;
	RconvStepmaniaNote** notes;
} RconvStepmaniaBeat;

typedef struct {
	RconvStepmaniaChartType chart_type;
	char* description;
	RconvStepmaniaDifficulty difficulty;
	int level;
	RconvStepmaniaRadarValues radar_values;
	int beats_len;
	RconvStepmaniaBeat** beats;
} RconvStepmaniaNoteData;

RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaBpmChange)
RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaStop)
RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaDelay)
RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaTimeSignature)
RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaInstrumentTrack)
RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaTickCount)
RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaBackgroundChange)
RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaModifier)
RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaAttack)
RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaTimedAttack)
RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaComboChange)
RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaSpeedChange)
RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaScrollSpeedChange)
RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaFakeSection)
RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaLabel)
RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaNote)
RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaBeat)
RCONV_LIST_TO_ARRAY_GEN_H(RconvStepmaniaNoteData)

typedef struct {
	char* title;
	char* subtitle;
	char* artist;
	char* title_transliterated;
	char* subtitle_transliterated;
	char* artist_transliterated;
	char* genre;
	char* credit;
	char* banner;
	char* background;
	char* lyrics;
	char* cd_title;
	char* music;
	int instrument_tracks_len;
	RconvStepmaniaInstrumentTrack** instrument_tracks;
	RconvFloat* sample_start;
	RconvFloat* sample_length;
	char* display_bpm;
	bool selectable;
	int background_changes_len;
	RconvStepmaniaBackgroundChange** background_changes;
	int background_changes2_len;
	RconvStepmaniaBackgroundChange** background_changes2;
	int background_changes3_len;
	RconvStepmaniaBackgroundChange** background_changes3;
	int animations_len;
	RconvStepmaniaBackgroundChange** animations;
	int foreground_changes_len;
	RconvStepmaniaBackgroundChange** foreground_changes;
	int key_sounds_len;
	char** key_sounds;
	RconvFloat* offset;
	int stops_len;
	RconvStepmaniaStop** stops;
	int bpms_len;
	RconvStepmaniaBpmChange** bpms;
	int time_signatures_len;
	RconvStepmaniaTimeSignature** time_signatures;
	int attacks_len;
	RconvStepmaniaTimedAttack** attacks;
	int delays_len;
	RconvStepmaniaDelay** delays;
	int tick_counts_len;
	RconvStepmaniaTickCount** tick_counts;
	int note_data_len;
	RconvStepmaniaNoteData** note_data;
	int combos_len;
	RconvStepmaniaComboChange** combos;
	int speeds_len;
	RconvStepmaniaSpeedChange** speeds;
	int scrolls_len;
	RconvStepmaniaScrollSpeedChange** scrolls;
	int fakes_len;
	RconvStepmaniaFakeSection** fakes;
	int labels_len;
	RconvStepmaniaLabel** labels;
} RconvStepmaniaChartFile;

/*
 * Functions
 */
RconvStepmaniaChartFile*
rconv_stepmania_parse(FILE* fp);

RconvStepmaniaChartFile*
rconv_stepmania_parse_from_string(char* data);

RconvStepmaniaChartFile*
rconv_stepmania_parse_from_file(char* file_path);

RconvFormattingParameters
rconv_stepmania_formatting_parameters(RconvStepmaniaChartFile* chart);

RconvStepmaniaColor*
rconv_stepmania_new_color(float red, float green, float blue, float alpha);

RconvStepmaniaBpmChange*
rconv_stepmania_new_bpm_change();

void
rconv_stepmania_free_bpm_change(RconvStepmaniaBpmChange* bpm_change);

void
rconv_stepmania_free_all_bpm_changes(int len, RconvStepmaniaBpmChange** bpm_changes);

RconvStepmaniaStop*
rconv_stepmania_new_stop();

void
rconv_stepmania_free_stop(RconvStepmaniaStop* stop);

void
rconv_stepmania_free_all_stops(int len, RconvStepmaniaStop** stops);

RconvStepmaniaDelay*
rconv_stepmania_new_delay();

void
rconv_stepmania_free_delay(RconvStepmaniaDelay* delay);

void
rconv_stepmania_free_all_delays(int len, RconvStepmaniaDelay** delays);

RconvStepmaniaTimeSignature*
rconv_stepmania_new_time_signature();

void
rconv_stepmania_free_time_signature(RconvStepmaniaTimeSignature* time_signature);

void
rconv_stepmania_free_all_time_signatures(int len, RconvStepmaniaTimeSignature** time_signatures);

RconvStepmaniaInstrumentTrack*
rconv_stepmania_new_instrument_track();

void
rconv_stepmania_free_instrument_track(RconvStepmaniaInstrumentTrack* instrument_track);

void
rconv_stepmania_free_all_instrument_tracks(int len, RconvStepmaniaInstrumentTrack** instrument_tracks);

RconvStepmaniaTickCount*
rconv_stepmania_new_tick_count();

void
rconv_stepmania_free_tick_count(RconvStepmaniaTickCount* tick_count);

void
rconv_stepmania_free_all_tick_counts(int len, RconvStepmaniaTickCount** tick_counts);

RconvStepmaniaBackgroundChange*
rconv_stepmania_new_background_change();

void
rconv_stepmania_free_background_change(RconvStepmaniaBackgroundChange* background_change);

void
rconv_stepmania_free_all_background_changes(int len, RconvStepmaniaBackgroundChange** background_changes);

RconvStepmaniaModifier*
rconv_stepmania_new_modifier();

void
rconv_stepmania_free_modifier(RconvStepmaniaModifier* modifier);

void
rconv_stepmania_free_all_modifiers(int len, RconvStepmaniaModifier** modifiers);

RconvStepmaniaAttack*
rconv_stepmania_new_attack();

void
rconv_stepmania_free_attack(RconvStepmaniaAttack* attack);

void
rconv_stepmania_free_all_attacks(int len, RconvStepmaniaAttack** attacks);

RconvStepmaniaTimedAttack*
rconv_stepmania_new_timed_attack();

void
rconv_stepmania_free_timed_attack(RconvStepmaniaTimedAttack* timed_attack);

void
rconv_stepmania_free_all_timed_attacks(int len, RconvStepmaniaTimedAttack** timed_attacks);

RconvStepmaniaComboChange*
rconv_stepmania_new_combo_change();

void
rconv_stepmania_free_combo_change(RconvStepmaniaComboChange* combo_change);

void
rconv_stepmania_free_all_combo_changes(int len, RconvStepmaniaComboChange** combo_changes);

RconvStepmaniaSpeedChange*
rconv_stepmania_new_speed_change();

void
rconv_stepmania_free_speed_change(RconvStepmaniaSpeedChange* speed_change);

void
rconv_stepmania_free_all_speed_changes(int len, RconvStepmaniaSpeedChange** speed_changes);

RconvStepmaniaScrollSpeedChange*
rconv_stepmania_new_scroll_speed_change();

void
rconv_stepmania_free_scroll_speed_change(RconvStepmaniaScrollSpeedChange* scroll_speed_change);

void
rconv_stepmania_free_all_scroll_speed_changes(int len, RconvStepmaniaScrollSpeedChange** scroll_speed_changes);

RconvStepmaniaFakeSection*
rconv_stepmania_new_fake_section();

void
rconv_stepmania_free_fake_section(RconvStepmaniaFakeSection* fake_section);

void
rconv_stepmania_free_all_fake_sections(int len, RconvStepmaniaFakeSection** fake_sections);

RconvStepmaniaLabel*
rconv_stepmania_new_label();

void
rconv_stepmania_free_label(RconvStepmaniaLabel* label);

void
rconv_stepmania_free_all_labels(int len, RconvStepmaniaLabel** labels);

RconvStepmaniaNote*
rconv_stepmania_new_note();

void
rconv_stepmania_free_note(RconvStepmaniaNote* note);

void
rconv_stepmania_free_all_notes(int len, RconvStepmaniaNote** notes);

RconvStepmaniaBeat*
rconv_stepmania_new_beat();

void
rconv_stepmania_free_beat(RconvStepmaniaBeat* beat);

void
rconv_stepmania_free_all_beats(int len, RconvStepmaniaBeat** beats);

RconvStepmaniaNoteData*
rconv_stepmania_new_note_data();

void
rconv_stepmania_free_note_data(RconvStepmaniaNoteData* note_data);

void
rconv_stepmania_free_all_note_data(int len, RconvStepmaniaNoteData** note_data);

RconvStepmaniaChartFile*
rconv_stepmania_new_chart_file();

void
rconv_stepmania_free_chart_file(RconvStepmaniaChartFile* chart_file);
