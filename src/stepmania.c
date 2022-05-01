#include "./stepmania.h"

RconvFormattingParameters rconv_stepmania_formatting_parameters(RconvStepmaniaChartFile* chart)
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

bool rconv_stepmania_whitespace(char c)
{
	return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

char* rconv_stepmania_parse_tags(char* data, char* tag, char* content, int offset)
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
				int len = (offset - start + 1);
				*tag = malloc(len * sizeof(char));
				memcpy(tag, data + offset, len - 1);
				tag[len] = '\0';
				state = 2;
				start = offset + 1;
			}
			offset++;
			continue;
		}

		if (state == 2) {
			if (data[offset] == ';') {
				int len = (offset - start + 1);
				*content = malloc(len * sizeof(char));
				memcpy(content, data + offset, len - 1);
				content[len] = '\0';
				offset++;
				break;
			}
			offset++;
		}
	}

	return offset;
}

bool rconv_stepmania_is_yes(char* content)
{
	utf8lwr(content);
	return utf8cmp(content, "yes") == 0 || utf8cmp(content, "1") == 0 || utf8cmp(content, "es") == 0 || utf8cmp(content, "omes") == 0;
}

void rconv_stepmania_parse_background_changes(char* data, int* len, RconvStepmaniaBackgroundChange* changes)
{
	RconvList* list;
	rconv_list(list, 5);

	RconvStepmaniaBackgroundChange* change = (RconvStepmaniaBackgroundChange*) malloc(sizeof(RconvStepmaniaBackgroundChange));

	int state = 0;
	int idx = 0;
	int size = strlen(data);
	int start = 0;
	int offset = 0;

	while (offset < size) {
		if (state == 0) {
			if (rconv_stepmania_whitespace(data[offset])) {
				offset++;
				continue;
			}
			start = offset;
			state = 1;
		}

		if (data[offset] == '=') {
			state = 1;
		} else if (data[offset] == ',') {
			state = 2;
		} else {
			offset++;
			continue;
		}

		char* val = malloc((offset - start + 1) * sizeof(char));
		strncpy(val, data + start, offset - start);

		if (idx == 0) {
			mpd_set_string(change->beat, val, NULL);
		} else if (idx == 1) {
			change->path = val;
		} else if (idx == 2) {
			mpd_set_string(change->update_rate, val, NULL);
		} else if (idx == 3) {
			change->crossfade = rconv_parse_bool(val);
		} else if (idx == 4) {
			change->stretch_rewind = rconv_parse_bool(val);
		} else if (idx == 5) {
			change->stretch_no_loop = rconv_parse_bool(val);
		} else if (idx == 6) {
			change->effect = val;
		} else if (idx == 7) {
			change->file2 = val;
		} else if (idx == 8) {
			change->transition = val;
		} else if (idx == 9) {
			// TODO: parse color
		} else if (idx == 10) {
			// TODO: parse color
		}

		offset++;

		if (state == 1) {
			idx++;
			continue;
		}

		idx = 0;
		rconv_list_add(list, change);
		RconvStepmaniaBackgroundChange* change = (RconvStepmaniaBackgroundChange*) malloc(sizeof(RconvStepmaniaBackgroundChange));
	}

	changes = (RconvStepmaniaBackgroundChange*) rconv_list_to_array(list, len);
	if (changes == -1) {
		changes = NULL;
	}
	rconv_list_free(list);
}

RconvStepmaniaChartFile* rconv_stepmania_parse(char* data)
{
	RconvStepmaniaChartFile* chart = (RconvStepmaniaChartFile*) malloc(sizeof(RconvStepmaniaChartFile));
	int offset = 0;
	char* tag = NULL;
	char* content = NULL;

	while (true) {
		offset = rconv_stepmania_parse_tags(data, tag, content, offset);
		if (tag == NULL || content == NULL) {
			break;
		}

		if (utf8cmp(tag, "title") == 0) {
			chart->title = content;
		} else if (utf8cmp(tag, "subtitle") == 0) {
			chart->subtitle = content;
		} else if (utf8cmp(tag, "artist") == 0) {
			chart->artist = content;
		} else if (utf8cmp(tag, "titletranslit") == 0) {
			chart->title_transliterated = content;
		} else if (utf8cmp(tag, "subtitletranslit") == 0) {
			chart->subtitle_transliterated = content;
		} else if (utf8cmp(tag, "artisttranslit") == 0) {
			chart->artist_transliterated = content;
		} else if (utf8cmp(tag, "genre") == 0) {
			chart->genre = content;
		} else if (utf8cmp(tag, "credit") == 0) {
			chart->credit = content;
		} else if (utf8cmp(tag, "banner") == 0) {
			chart->banner = content;
		} else if (utf8cmp(tag, "background") == 0) {
			chart->background = content;
		} else if (utf8cmp(tag, "lyricspath") == 0) {
			chart->lyrics = content;
		} else if (utf8cmp(tag, "cdtitle") == 0) {
			chart->cd_title = content;
		} else if (utf8cmp(tag, "music") == 0) {
			chart->music = content;
		} else if (utf8cmp(tag, "displaybmp") == 0) {
			chart->display_bpm = content;
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
		}
	}
}
