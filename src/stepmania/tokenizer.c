#include "tokenizer.h"

RconvToken*
_rconv_stepmania_parse_token(RconvDataDescriptor* dd, size_t start, size_t length, size_t line, size_t column)
{
    // Get the old position and the length we want to read
    size_t old_pos = dd->position;
    // Rewind the position to make it read the proper content
    rconv_dd_set_position(dd, start);
    // Read the content
    char* content = malloc(sizeof(char) * length);
    rconv_dd_read_string(dd, content, length);
    // Set the position back to the old position
    rconv_dd_set_position(dd, old_pos);

    // Create the token from the content
    RconvToken* token = calloc(1, sizeof(RconvToken));
    token->content = content;
    token->line = line;
    token->column = column;

    return token;
}

RconvToken*
_rconv_stepmania_new_property_start_token(size_t line, size_t column)
{
    RconvToken* token = calloc(1, sizeof(RconvToken));
    token->type = RCONV_STEPMANIA_TOKEN_PROPERTY_START;
    token->line = line;
    token->column = column;

    return token;
}

RconvToken*
_rconv_stepmania_new_property_end_token(size_t line, size_t column)
{
    RconvToken* token = calloc(1, sizeof(RconvToken));
    token->type = RCONV_STEPMANIA_TOKEN_PROPERTY_END;
    token->line = line;
    token->column = column;
    return token;
}

RconvToken*
_rconv_stepmania_new_value_start_token(size_t line, size_t column)
{
    RconvToken* token = calloc(1, sizeof(RconvToken));
    token->type = RCONV_STEPMANIA_TOKEN_VALUE_START;
    token->line = line;
    token->column = column;
    return token;
}

RconvToken*
_rconv_stepmania_new_value_separator_token(size_t flags, size_t line, size_t column)
{
    RconvToken* token = calloc(1, sizeof(RconvToken));
    token->type = RCONV_STEPMANIA_TOKEN_VALUE_SEPARATOR;
    token->flags = flags;
    token->line = line;
    token->column = column;
    return token;
}

RconvToken*
_rconv_stepmania_new_snap_increase_token(size_t snap, size_t line, size_t column)
{
    size_t* content = malloc(sizeof(size_t));
    content[0] = snap;

    RconvToken* token = calloc(1, sizeof(RconvToken));
    token->type = RCONV_STEPMANIA_TOKEN_SNAP_INCREASE;
    token->content = content;
    token->line = line;
    token->column = column;
    return token;
}

RconvToken*
_rconv_stepmania_new_snap_reset_token(size_t snap, size_t line, size_t column)
{
    size_t* content = malloc(sizeof(size_t));
    content[0] = snap;

    RconvToken* token = calloc(1, sizeof(RconvToken));
    token->type = RCONV_STEPMANIA_TOKEN_SNAP_RESET;
    token->content = content;
    token->line = line;
    token->column = column;
    return token;
}

RconvToken*
_rconv_stepmania_new_note_token(char c, size_t line, size_t column)
{
    char* tmp = malloc(sizeof(char) * 2);
    tmp[0] = c;
    tmp[1] = '\0';

    RconvToken* token = calloc(1, sizeof(RconvToken));
    token->type = RCONV_STEPMANIA_TOKEN_NOTE;
    token->content = tmp;
    token->line = line;
    token->column = column;

    return token;
}

void
_rconv_stepmania_set_token_type(RconvToken* token)
{
    if (token->content == NULL) {
        token->type = RCONV_TOKEN_STRING;
        return;
	}

    token->type = rconv_detect_common_token_value_type((char*) token->content);
}

RconvToken**
rconv_stepmania_tokenize(RconvDataDescriptor* dd, int* length)
{
    if (dd == NULL) {
        return NULL;
    }

    RconvList* list = rconv_list();
    RconvStepmaniaTokenizerState state = RCONV_STEPMANIA_TOKENIZER_STATE_CLEAN;
    RconvStepmaniaTokenizerState prev_state = RCONV_STEPMANIA_TOKENIZER_STATE_CLEAN;

    size_t start = 0;
    // Current line and column
    size_t line = 1;
    size_t column = 0;
    int utf8char_len = 0;
    // The start position of the current token
    size_t token_line_start = 0;
    size_t token_column_start = 0;

    bool set_new_position = false;
    bool escape_next = false;
    bool used_single = false;
    bool line_had_note = false;
    size_t meta_count = 0;
    size_t snap_count = 0;

    while (!rconv_dd_is_eof(dd)) {
        char* c = rconv_dd_read_utf8char(dd, &utf8char_len);

        // Weird edge case
        if (c == NULL) {
            break;
        }

        if (c[0] == '\n') {
            line++;
            column = 0;
        } else {
            column++;
        }

        if (set_new_position) {
            token_line_start = line;
            token_column_start = column;
            set_new_position = false;
        }

        if (escape_next) {
            escape_next = false;
            continue;
        }

        if (state == RCONV_STEPMANIA_TOKENIZER_STATE_IN_STRING) {
            if ((used_single && c[0] == '\'') || (!used_single && c[0] == '"')) {
                size_t old_pos = dd->position;
                size_t token_len = old_pos - start - 1;

                RconvToken* token = _rconv_stepmania_parse_token(dd, start, token_len, token_line_start, token_column_start);
                token->content = rconv_trim((char*) token->content);
                token->type = RCONV_TOKEN_STRING;
                token->flags = RCONV_TOKENFLAG_STRING_USED_QUOTES;
                if (used_single) {
                    token->flags |= RCONV_TOKENFLAG_STRING_USED_QUOTES;
                }
                rconv_list_add(list, token);
                rconv_dd_set_position(dd, old_pos);

                start = old_pos;
                state = prev_state;
                set_new_position = true;

                continue;
            }

            continue;
        }

        if (state == RCONV_STEPMANIA_TOKENIZER_STATE_CLEAN) {
            if (c[0] == '#') {
                start = dd->position;

                RconvToken* token = _rconv_stepmania_new_property_start_token(line, column);
                rconv_list_add(list, token);

                state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_PROPERTY_NAME;
                set_new_position = true;

                continue;
            }

            // Since comments start with two slashes, we have to look ahead and see what's there.
            if (c[0] == '/') {
                size_t old_pos = dd->position;
                char next = rconv_dd_read_char(dd);

                if (next == '/') {
                    state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_LINE_COMMENT;
                    start = dd->position;
                    column++;
                    set_new_position = true;
                    continue;
                }

                rconv_dd_set_position(dd, old_pos);
                continue;
            }

            continue;
        }

        if (state == RCONV_STEPMANIA_TOKENIZER_STATE_IN_PROPERTY_NAME) {
            if (c[0] == ':') {
                size_t old_pos = dd->position;
                size_t token_len = old_pos - start - 1;

                RconvToken* token = _rconv_stepmania_parse_token(dd, start, token_len, token_line_start, token_column_start);
                token->content = rconv_trim((char*) token->content);
                _rconv_stepmania_set_token_type(token);
                // Always lowercase property names to not having to worry about case checks later on
                utf8lwr(token->content);
                rconv_list_add(list, token);
                rconv_dd_set_position(dd, old_pos);

                rconv_list_add(list, _rconv_stepmania_new_value_start_token(line, column));

                start = old_pos;
                set_new_position = true;

                if (utf8cmp(((char*) token->content), "notes") == 0) {
                    meta_count = 0;
                    state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_NOTES_META;
                    continue;
                }

                state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_VALUE;
                continue;
            }

            continue;
        }

        if (state == RCONV_STEPMANIA_TOKENIZER_STATE_IN_VALUE) {
            if (c[0] == '\\') {
                escape_next = true;
                continue;
            }

            if (c[0] == '\'' || c[0] == '"') {
                used_single = c[0] == '\'';
                prev_state = state;
                state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_STRING;
                continue;
            }

            if (c[0] == '=' || c[0] == ',' || c[0] == ':' || c[0] == ';') {
                size_t old_pos = dd->position;
                size_t token_len = old_pos - start - 1;

                RconvToken* token = _rconv_stepmania_parse_token(dd, start, token_len, token_line_start, token_column_start);
                token->content = rconv_trim((char*) token->content);
                _rconv_stepmania_set_token_type(token);
                rconv_list_add(list, token);
                rconv_dd_set_position(dd, old_pos);

                if (c[0] == ';') {
                    rconv_list_add(list, _rconv_stepmania_new_property_end_token(line, column));
                    start = 0;
                    state = RCONV_STEPMANIA_TOKENIZER_STATE_CLEAN;
                } else {
                    size_t sep_flags;

                    if (c[0] == '=') {
                        sep_flags = RCONV_TOKENFLAG_STEPMANIA_VALUE_SEPARATOR_EQUALS;
                    } else if (c[0] == ',') {
                        sep_flags = RCONV_TOKENFLAG_STEPMANIA_VALUE_SEPARATOR_COMMA;
                    } else {
                        sep_flags = RCONV_TOKENFLAG_STEPMANIA_VALUE_SEPARATOR_COLON;
                    }

                    rconv_list_add(list, _rconv_stepmania_new_value_separator_token(sep_flags, line, column));
                    start = old_pos;
                    state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_VALUE;
                }

                set_new_position = true;

                continue;
            }

            continue;
        }

        if (state == RCONV_STEPMANIA_TOKENIZER_STATE_IN_NOTES_META) {
            if (c[0] == ',') {
                size_t old_pos = dd->position;
                size_t token_len = old_pos - start - 1;

                RconvToken* token = _rconv_stepmania_parse_token(dd, start, token_len, token_line_start, token_column_start);
                token->content = rconv_trim((char*) token->content);
                _rconv_stepmania_set_token_type(token);
                rconv_list_add(list, token);
                rconv_dd_set_position(dd, old_pos);

                rconv_list_add(list, _rconv_stepmania_new_value_separator_token(RCONV_TOKENFLAG_STEPMANIA_VALUE_SEPARATOR_COMMA, line, column));
                
                start = old_pos;
                set_new_position = true;

                continue;
            }

            if (c[0] == ':') {
                size_t old_pos = dd->position;
                size_t token_len = old_pos - start - 1;

                RconvToken* token = _rconv_stepmania_parse_token(dd, start, token_len, token_line_start, token_column_start);
                token->content = rconv_trim((char*) token->content);
                _rconv_stepmania_set_token_type(token);
                rconv_list_add(list, token);
                rconv_dd_set_position(dd, old_pos);

                rconv_list_add(list, _rconv_stepmania_new_value_separator_token(RCONV_TOKENFLAG_STEPMANIA_VALUE_SEPARATOR_COLON, line, column));

                start = old_pos;
                set_new_position = true;
                meta_count++;

                if (meta_count >= 5) {
                    snap_count = 0;
                    state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_NOTES;
                    line_had_note = false;
                    continue;
                }

                continue;
            }

            continue;
        }

        if (state == RCONV_STEPMANIA_TOKENIZER_STATE_IN_NOTES) {
            if (c[0] == ';') {
                RconvToken* token = _rconv_stepmania_new_property_end_token(token_line_start, token_column_start);
                rconv_list_add(list, token);

                set_new_position = true;
                state = RCONV_STEPMANIA_TOKENIZER_STATE_CLEAN;

                continue;
            }

            if (c[0] == ',') {
                rconv_list_add(list, _rconv_stepmania_new_snap_reset_token(snap_count, line, column));
                snap_count = 0;
                line_had_note = false;
                continue;
            }

            if (c[0] == '\n') {
                if (line_had_note) {
                    rconv_list_add(list, _rconv_stepmania_new_snap_increase_token(snap_count, line, column));
                    snap_count++;
                    line_had_note = false;
                }
                continue;
            }

            if (c[0] == RCONV_STEPMANIA_NOTE_EMPTY
                || c[0] == RCONV_STEPMANIA_NOTE_NOTE
                || c[0] == RCONV_STEPMANIA_NOTE_HOLD
                || c[0] == RCONV_STEPMANIA_NOTE_HEND
                || c[0] == RCONV_STEPMANIA_NOTE_ROLL
                || c[0] == RCONV_STEPMANIA_NOTE_MINE
                || c[0] == RCONV_STEPMANIA_NOTE_LIFT
                || c[0] == RCONV_STEPMANIA_NOTE_FAKE
            ) {
                rconv_list_add(list, _rconv_stepmania_new_note_token(c[0], line, column));

                line_had_note = true;
                set_new_position = true;

                continue;
            }

            continue;
        }

        if (state == RCONV_STEPMANIA_TOKENIZER_STATE_IN_LINE_COMMENT) {
            if (c[0] == '\n') {
                size_t old_pos = dd->position;
                size_t token_len = old_pos - start - 1;

                RconvToken* token = _rconv_stepmania_parse_token(dd, start, token_len, token_line_start, token_column_start);
                token->content = rconv_trim((char*) token->content);
                token->type = RCONV_STEPMANIA_TOKEN_LINE_COMMENT;
                rconv_list_add(list, token);
                rconv_dd_set_position(dd, old_pos);

                state = RCONV_STEPMANIA_TOKENIZER_STATE_CLEAN;
                set_new_position = true;

                continue;
            }

            continue;
        }

        continue;
    }

    RconvToken* eofToken = calloc(sizeof(RconvToken), 1);
    eofToken->type = RCONV_TOKEN_EOF;
    eofToken->line = line;
    eofToken->column = column + 1;
    rconv_list_add(list, eofToken);

    RconvToken** out = rconv_list_to_token_array(list, length);
    rconv_list_free(list);

    return out;
}
