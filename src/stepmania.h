#pragma once

/*
 * Libraries
 */
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <utf8.h>

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

#define RCONV_STEPMANIA_CLEAR_LIST_NAMED(type,name) \
type** items = rconv_list_to_##name##_array(list, len); \
if (*items == NULL) { \
	*items = NULL; \
} \
rconv_list_free(list); \
return items;

#define RCONV_STEPMANIA_CLEAR_LIST(type) \
RCONV_STEPMANIA_CLEAR_LIST_NAMED(type,type)

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
	char* mods;
} RconvStepmaniaAttack;

typedef struct {
	RconvFloat* time;
	RconvFloat* length;
	int mods_len;
	RconvStepmaniaModifier* mods;
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
	RconvStepmaniaAttack* attack;
	int key_sound;
	int modifiers_len;
	RconvStepmaniaModifier* modifiers;
	int release_beat;
	int release_snap;
	bool release_lift;
} RconvStepmaniaNote;

typedef struct {
	int index;
	int snap_size;
	int notes_len;
	RconvStepmaniaNote* notes;
} RconvStepmaniaBeat;

typedef struct {
	RconvStepmaniaChartType chart_type;
	char* description;
	RconvStepmaniaDifficulty difficulty;
	int level;
	RconvStepmaniaRadarValues radar_values;
	int beats_len;
	RconvStepmaniaBeat* beats;
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
rconv_stepmania_parse(char* data);

RconvFormattingParameters
rconv_stepmania_formatting_parameters(RconvStepmaniaChartFile* chart);
