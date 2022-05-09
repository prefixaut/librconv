#pragma once

#include <stdbool.h>
#include <string.h>

#define _RCONV_TOKEN_LIST = "口-||-^v<>①②③④⑤⑥⑦⑨⑨⑩⑪⑫⑬⑭⑮⑯⑰⑱⑲⑳ⒶⒷⒸⒹⒺⒻⒼⒽⒾⒿⓀⓁⓂⓃⓄⓅⓆⓇⓈⓉⓊⓋⓌⓍⓎⓏ";

typedef struct {
	int length;
	int part_index;
	int row;
} RconvMemoSnap;

typedef struct {
	int position;
	int time;
	int part;
	bool is_hold;
	int animation_start;
	int release_time;
	int release_part;
	int release_section;
} RconvMemoNote;

typedef struct {
	int index;
	float bpm;
	int part_count;
	int timing_len;
	int* timings;
	int snap_len;
	RconvMemoSnap* snaps;
	int note_len;
	RconvMemoNote* notes;
} RconvMemoSection;

typedef struct {
	char* song_title;
	char* artist;
	char* difficulty;
	int level;
	int bpm[2];
	int bpm_range[4];
	int section_len;
	RconvMemoSection* sections;
} RconvMemoChart;

typedef enum {
	EMPTY_NOTE = 0,
	EMPTY_TIME = 1,
	SEPARATOR = 2,
	HOLD_VERT = 3,
	HOLD_HORZ = 4,
	HOLD_UP = 5,
	HOLD_DOWN = 6,
	HOLD_LEFT = 7,
	HOLD_RIGHT = 8,
	POSITIONS = 9,
} RconvMemoToken;

RconvMemoChart
rconv_memo_parse(char* data[]);

char*
rconv_memo_parse_difficulty(char* data[]);
