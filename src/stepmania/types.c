#include "types.h"

/*
 * ==================== COLORS ====================
 */

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

	bool has_set = !rconv_decimal_is_zero(elem->bpm);

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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

	bool has_set = !rconv_decimal_is_zero(elem->duration);

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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

	bool has_set = !rconv_decimal_is_zero(elem->duration);

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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

	bool has_set = elem->path != NULL || !rconv_decimal_is_zero(elem->update_rate) || elem->crossfade || elem->stretch_rewind
				   || elem->stretch_no_loop || elem->effect != NULL || elem->file2 != NULL || elem->transition != NULL
				   || elem->color1 != NULL || elem->color2 != NULL;

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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

/*
 * ==================== MODIFIERS ====================
 */

RconvStepmaniaModifier*
rconv_stepmania_new_modifier()
{
	RconvStepmaniaModifier* out = (RconvStepmaniaModifier*) calloc(1, sizeof(RconvStepmaniaModifier));
	out->approach_rate = 1;
	out->magnitude = rconv_decimal_new_from_double(RCONV_STEPMANIA_FRACTION_PRECISION, 100.0);
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

	bool has_set = !rconv_decimal_is_zero(elem->duration) && !rconv_decimal_is_zero(elem->ratio);

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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

	bool has_set = !rconv_decimal_is_zero(elem->factor);

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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

	bool has_set = rconv_decimal_is_zero(elem->duration);

	if (idx == 0) {
		return has_set;
	}

	return elem->beat != NULL && has_set;
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
