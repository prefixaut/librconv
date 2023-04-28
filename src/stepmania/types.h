#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <utf8.h>

/*
 * Own modules
 */
#include "../common.h"
#include "../decimals.h"
#include "../list.h"

#define RCONV_STEPMANIA_FRACTION_PRECISION 5

#define RCONV_STEPMANIA_TYPE_GEN_H(type, name) \
	RCONV_LIST_TO_ARRAY_GEN_H_NAMED(type, name) \
	type* rconv_stepmania_new_##name(); \
	void rconv_stepmania_free_##name(type* element); \
	bool rconv_stepmania_verify_##name##_entry(type* element, int idx);

#define RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(type, name, plural) \
	RCONV_STEPMANIA_TYPE_GEN_H(type, name) \
	void rconv_stepmania_free_all_##plural(int len, type** elements);

/*
 * Types
 */
typedef enum {
	RCONV_STEPMANIA_NOTE_EMPTY = '0',
	RCONV_STEPMANIA_NOTE_NOTE = '1',
	RCONV_STEPMANIA_NOTE_HOLD = '2',
	RCONV_STEPMANIA_NOTE_HEND = '3',
	RCONV_STEPMANIA_NOTE_ROLL = '4',
	RCONV_STEPMANIA_NOTE_MINE = 'M',
	RCONV_STEPMANIA_NOTE_LIFT = 'L',
	RCONV_STEPMANIA_NOTE_FAKE = 'F',
} RconvStepmaniaNoteType;

typedef enum {
	RCONV_STEPMANIA_CHART_TYPE_DANCE_SINGLE,
	RCONV_STEPMANIA_CHART_TYPE_DANCE_DOUBLE,
	RCONV_STEPMANIA_CHART_TYPE_DANCE_COUPLE,
	RCONV_STEPMANIA_CHART_TYPE_DANCE_ROUTINE,
	RCONV_STEPMANIA_CHART_TYPE_DANCE_SOLO,
	RCONV_STEPMANIA_CHART_TYPE_PUMP_SINGLE,
	RCONV_STEPMANIA_CHART_TYPE_PUMP_HALF_DOUBLE,
	RCONV_STEPMANIA_CHART_TYPE_PUMP_DOUBLE,
	RCONV_STEPMANIA_CHART_TYPE_PUMP_COUPLE,
} RconvStepmaniaChartType;

typedef enum {
	RCONV_STEPMANIA_DIFFICULTY_BEGINNER,
	RCONV_STEPMANIA_DIFFICULTY_EASY,
	RCONV_STEPMANIA_DIFFICULTY_MEDIUM,
	RCONV_STEPMANIA_DIFFICULTY_HARD,
	RCONV_STEPMANIA_DIFFICULTY_CHALLANGE,
	RCONV_STEPMANIA_DIFFICULTY_EDIT,
} RconvStepmaniaDifficulty;

typedef float RconvStepmaniaColor[4];

typedef float RconvStepmaniaRadarValues[5];

typedef struct {
	RconvDecimal* beat;
	RconvDecimal* bpm;
} RconvStepmaniaBpmChange;

typedef struct {
	RconvDecimal* beat;
	RconvDecimal* duration;
} RconvStepmaniaStop;

typedef struct {
	RconvDecimal* beat;
	RconvDecimal* duration;
} RconvStepmaniaDelay;

typedef struct {
	RconvDecimal* beat;
	int numerator;
	int denominator;
} RconvStepmaniaTimeSignature;

typedef struct {
	char* instrument;
	char* file;
} RconvStepmaniaInstrumentTrack;

typedef struct {
	RconvDecimal* beat;
	int count;
} RconvStepmaniaTickCount;

typedef struct {
	RconvDecimal* beat;
	char* path;
	RconvDecimal* update_rate;
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
	long approach_rate;
	RconvDecimal* magnitude;
	bool is_percent;
} RconvStepmaniaModifier;

typedef struct {
	RconvDecimal* length;
	int mods_len;
	char** mods;
} RconvStepmaniaAttack;

typedef struct {
	RconvDecimal* time;
	RconvDecimal* length;
	int mods_len;
	RconvStepmaniaModifier** mods;
} RconvStepmaniaTimedAttack;

typedef struct {
	RconvDecimal* beat;
	int hit;
	int miss;
} RconvStepmaniaComboChange;

typedef struct {
	RconvDecimal* beat;
	RconvDecimal* ratio;
	RconvDecimal* duration;
	bool in_seconds;
} RconvStepmaniaSpeedChange;

typedef struct {
	RconvDecimal* beat;
	RconvDecimal* factor;
} RconvStepmaniaScrollSpeedChange;

typedef struct {
	RconvDecimal* beat;
	RconvDecimal* duration;
} RconvStepmaniaFakeSection;

typedef struct {
	RconvDecimal* beat;
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
	RconvDecimal* sample_start;
	RconvDecimal* sample_length;
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
	RconvDecimal* offset;
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

RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaBpmChange, bpm_change, bpm_changes)
RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaStop, stop, stops)
RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaDelay, delay, delays)
RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaTimeSignature, time_signature, time_signatures)
RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaInstrumentTrack, instrument_track, instrument_tracks)
RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaTickCount, tick_count, tick_counts)
RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaBackgroundChange, background_change, background_changes)
RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaModifier, modifier, modifiers)
RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaAttack, attack, attacks)
RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaTimedAttack, timed_attack, timed_attacks)
RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaComboChange, combo_change, combo_changes)
RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaSpeedChange, speed_change, speed_changes)
RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaScrollSpeedChange, scroll_speed_change, scroll_speed_changes)
RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaFakeSection, fake_section, fake_sections)
RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaLabel, label, labels)
RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaNote, note, notes)
RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaBeat, beat, beats)
RCONV_STEPMANIA_PLURAL_TYPE_GEN_H(RconvStepmaniaNoteData, note_data, note_data)
RCONV_STEPMANIA_TYPE_GEN_H(RconvStepmaniaChartFile, chart_file)

RconvStepmaniaColor*
rconv_stepmania_new_color(float red, float green, float blue, float alpha);
