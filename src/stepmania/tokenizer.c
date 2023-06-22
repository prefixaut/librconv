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
_rconv_stepmania_new_value_separator_token(char c, size_t line, size_t column)
{
    RconvToken* token = calloc(1, sizeof(RconvToken));
    char* content = malloc(sizeof(char) * 2);
    content[0] = c;
    content[1] = '\0';

    token->type = RCONV_STEPMANIA_TOKEN_VALUE_SEPARATOR;
    token->content = content;

    if (c == RCONV_STEPMANIA_TOKENIZER_CHAR_VALUE_SEPARATOR_EQUALS) {
        token->flags = RCONV_TOKENFLAG_STEPMANIA_VALUE_SEPARATOR_EQUALS;
    } else if (c == RCONV_STEPMANIA_TOKENIZER_CHAR_VALUE_SEPARATOR_COMMA) {
        token->flags = RCONV_TOKENFLAG_STEPMANIA_VALUE_SEPARATOR_COMMA;
    } else {
        token->flags = RCONV_TOKENFLAG_STEPMANIA_VALUE_SEPARATOR_COLON;
    }

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

        if (c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_NEW_LINE) {
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

        switch (state) {
        case RCONV_STEPMANIA_TOKENIZER_STATE_IN_STRING:
            if ((used_single && c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_SINGLE_QUOTE)
                || (!used_single && c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_DOUBLE_QUOTE)
            ) {
                size_t old_pos = dd->position;
                size_t token_len = old_pos - start - 1;

                if (token_len > 0) {
                    RconvToken* token = _rconv_stepmania_parse_token(dd, start, token_len, token_line_start, token_column_start);
                    token->type = RCONV_TOKEN_STRING;
                    token->flags = RCONV_TOKENFLAG_STRING_USED_QUOTES;
                    if (used_single) {
                        token->flags |= RCONV_TOKENFLAG_STRING_USED_QUOTES;
                    }
                    rconv_list_add(list, token);
                    rconv_dd_set_position(dd, old_pos);
                }

                start = old_pos;
                state = prev_state;
                set_new_position = true;
            }

            break;

        case RCONV_STEPMANIA_TOKENIZER_STATE_CLEAN:
            if (c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_PROPERTY_START) {
                start = dd->position;

                RconvToken* token = _rconv_stepmania_new_property_start_token(line, column);
                rconv_list_add(list, token);

                state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_PROPERTY_NAME;
                set_new_position = true;

                break;
            }

            // Since comments start with two slashes, we have to look ahead and see what's there.
            if (c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_COMMENT_START) {
                size_t old_pos = dd->position;
                char next = rconv_dd_read_char(dd);

                if (next == RCONV_STEPMANIA_TOKENIZER_CHAR_COMMENT_START) {
                    state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_LINE_COMMENT;
                    start = dd->position;
                    column++;
                    set_new_position = true;
                    continue;
                }

                rconv_dd_set_position(dd, old_pos);
                break;
            }

            break;

        case RCONV_STEPMANIA_TOKENIZER_STATE_IN_PROPERTY_NAME:
            if (c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_VALUE_START) {
                size_t old_pos = dd->position;
                size_t token_len = old_pos - start - 1;
                bool is_notes = false;

                if (token_len > 0) {
                    RconvToken* token = _rconv_stepmania_parse_token(dd, start, token_len, token_line_start, token_column_start);
                    token->content = rconv_trim((char*) token->content, true);
                    _rconv_stepmania_set_token_type(token);
                    // Always lowercase property names to not having to worry about case checks later on
                    utf8lwr(token->content);
                    rconv_list_add(list, token);
                    rconv_dd_set_position(dd, old_pos);

                    is_notes = utf8cmp(((char*) token->content), "notes") == 0;
                }

                rconv_list_add(list, _rconv_stepmania_new_value_start_token(line, column));

                start = old_pos;
                set_new_position = true;

                if (is_notes) {
                    meta_count = 0;
                    state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_NOTES_META;
                    break;
                }

                state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_VALUE;
                break;
            }

            break;

        case RCONV_STEPMANIA_TOKENIZER_STATE_IN_VALUE:
            if (c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_ESCAPE_VALUE) {
                escape_next = true;
                break;
            }

            if (c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_SINGLE_QUOTE
                || c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_DOUBLE_QUOTE
            ) {
                used_single = c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_SINGLE_QUOTE;
                prev_state = state;
                state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_STRING;
                break;
            }

            if (c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_VALUE_SEPARATOR_EQUALS
                || c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_VALUE_SEPARATOR_COMMA
                || c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_VALUE_SEPARATOR_COLON
                || c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_VALUE_END
            ) {
                size_t old_pos = dd->position;
                size_t token_len = old_pos - start - 1;

                if (token_len > 0) {
                    RconvToken* token = _rconv_stepmania_parse_token(dd, start, token_len, token_line_start, token_column_start);
                    _rconv_stepmania_set_token_type(token);
                    rconv_list_add(list, token);
                    rconv_dd_set_position(dd, old_pos);
                }

                if (c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_VALUE_END) {
                    rconv_list_add(list, _rconv_stepmania_new_property_end_token(line, column));
                    start = 0;
                    state = RCONV_STEPMANIA_TOKENIZER_STATE_CLEAN;
                } else {
                    rconv_list_add(list, _rconv_stepmania_new_value_separator_token(c[0], line, column));
                    start = old_pos;
                    state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_VALUE;
                }

                set_new_position = true;

                break;
            }

            break;

        case RCONV_STEPMANIA_TOKENIZER_STATE_IN_NOTES_META:
            if (c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_RADAR_SEPARTAOR) {
                size_t old_pos = dd->position;
                size_t token_len = old_pos - start - 1;

                if (token_len > 0) {
                    RconvToken* token = _rconv_stepmania_parse_token(dd, start, token_len, token_line_start, token_column_start);
                    token->content = rconv_trim((char*) token->content, true);
                    _rconv_stepmania_set_token_type(token);
                    rconv_list_add(list, token);
                    rconv_dd_set_position(dd, old_pos);
                }

                rconv_list_add(list, _rconv_stepmania_new_value_separator_token(c[0], line, column));
                
                start = old_pos;
                set_new_position = true;

                break;
            }

            if (c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_NOTES_META_SEPARATOR) {
                size_t old_pos = dd->position;
                size_t token_len = old_pos - start - 1;

                if (token_len > 0) {
                    RconvToken* token = _rconv_stepmania_parse_token(dd, start, token_len, token_line_start, token_column_start);
                    token->content = rconv_trim((char*) token->content, true);
                    _rconv_stepmania_set_token_type(token);
                    rconv_list_add(list, token);
                    rconv_dd_set_position(dd, old_pos);
                }

                rconv_list_add(list, _rconv_stepmania_new_value_separator_token(c[0], line, column));

                start = old_pos;
                set_new_position = true;
                meta_count++;

                if (meta_count >= 5) {
                    snap_count = 0;
                    state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_NOTES;
                    line_had_note = false;
                    break;
                }

                break;
            }

            break;

        case RCONV_STEPMANIA_TOKENIZER_STATE_IN_NOTES:
            if (c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_VALUE_END) {
                RconvToken* token = _rconv_stepmania_new_property_end_token(token_line_start, token_column_start);
                rconv_list_add(list, token);

                set_new_position = true;
                state = RCONV_STEPMANIA_TOKENIZER_STATE_CLEAN;

                break;
            }

            if (c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_BEAT_SEPARATOR) {
                rconv_list_add(list, _rconv_stepmania_new_snap_reset_token(snap_count, line, column));
                snap_count = 0;
                line_had_note = false;
                break;
            }

            if (c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_SNAP_SEPARATOR) {
                if (line_had_note) {
                    rconv_list_add(list, _rconv_stepmania_new_snap_increase_token(snap_count, line, column));
                    snap_count++;
                    line_had_note = false;
                }
                break;
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

                break;
            }

            break;

        case RCONV_STEPMANIA_TOKENIZER_STATE_IN_LINE_COMMENT:
            if (c[0] == RCONV_STEPMANIA_TOKENIZER_CHAR_COMMENT_END) {
                size_t old_pos = dd->position;
                size_t token_len = old_pos - start - 1;

                RconvToken* token = _rconv_stepmania_parse_token(dd, start, token_len, token_line_start, token_column_start);
                token->content = rconv_trim((char*) token->content, true);
                token->type = RCONV_STEPMANIA_TOKEN_LINE_COMMENT;
                rconv_list_add(list, token);
                rconv_dd_set_position(dd, old_pos);

                state = RCONV_STEPMANIA_TOKENIZER_STATE_CLEAN;
                set_new_position = true;

                break;
            }

            break;
        
        default:
            break;
        }
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
