#include "tokenizer.h"

RconvToken*
_rconv_stepmania_parse_token(RconvDataDescriptor* dd, size_t start, size_t length)
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
    token->content = rconv_trim(content);

    return token;
}

RconvToken*
_rconv_stepmania_new_property_end()
{
    RconvToken* token = calloc(1, sizeof(RconvToken));
    token->type = RCONV_STEPMANIA_TOKEN_PROPERTY_END;
    return token;
}

RconvToken*
_rconv_stepmania_new_value_separator(size_t flags)
{
    RconvToken* token = calloc(1, sizeof(RconvToken));
    token->type = RCONV_STEPMANIA_TOKEN_VALUE_SEPARATOR;
    token->flags = flags;
    return token;
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
    bool escape_next = false;
    bool used_single = false;
    size_t meta_count = 0;
    size_t snap_count = 0;

    while (!rconv_dd_is_eof(dd)) {
        char c = rconv_dd_read_char(dd);

        if (escape_next) {
            escape_next = false;
            continue;
        }

        if (state == RCONV_STEPMANIA_TOKENIZER_STATE_IN_STRING) {
            if ((used_single && c == '\'') || (!used_single && c == '"')) {
                size_t old_pos = dd->position;
                size_t length = old_pos - start - 1;
                RconvToken* token = _rconv_stepmania_parse_token(dd, start, length);
                token->type = RCONV_TOKEN_STRING;
                token->flags = RCONV_TOKENFLAG_STRING_USED_QUOTES;
                if (used_single) {
                    token->flags |= RCONV_TOKENFLAG_STRING_USED_QUOTES;
                }
                rconv_list_add(list, token);

                start = old_pos;
                state = prev_state;
                continue;
            }

            continue;
        }

        if (state == RCONV_STEPMANIA_TOKENIZER_STATE_CLEAN) {
            if (c == '#') {
                start = dd->position;

                RconvToken* token = calloc(1, sizeof(RconvToken));
                token->type = RCONV_STEPMANIA_TOKEN_PROPERTY_START;
                rconv_list_add(list, token);

                state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_PROPERTY_NAME;
                continue;
            }

            continue;
        }

        if (state == RCONV_STEPMANIA_TOKENIZER_STATE_IN_PROPERTY_NAME) {
            if (c == ':') {
                size_t old_pos = dd->position;
                size_t length = old_pos - start - 1;
                RconvToken* token = _rconv_stepmania_parse_token(dd, start, length);
                token->type = RCONV_STEPMANIA_TOKEN_PROPERTY_NAME;
                utf8lwr(token->content);
                rconv_list_add(list, token);

                start = old_pos;

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
            if (c == '\\') {
                escape_next = true;
                continue;
            }

            if (c == '\'' || c == '"') {
                used_single = c == '\'';
                prev_state = state;
                state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_STRING;
                continue;
            }

            if (c == '=' || c == ',' || c == ':' || c == ';') {
                size_t old_pos = dd->position;
                size_t length = old_pos - start - 1;
                RconvToken* token = _rconv_stepmania_parse_token(dd, start, length);
                token->type = RCONV_TOKEN_STRING;
                rconv_list_add(list, token);

                if (c == '=' || c == ',' || c == ':') {
                    size_t sep_flags;

                    if (c == '=') {
                        sep_flags = RCONV_TOKENFLAG_STEPMANIA_VALUE_SEPARATOR_EQUALS;
                    } else if (c == ',') {
                        sep_flags = RCONV_TOKENFLAG_STEPMANIA_VALUE_SEPARATOR_COMMA;
                    } else {
                        sep_flags = RCONV_TOKENFLAG_STEPMANIA_VALUE_SEPARATOR_COLON;
                    }

                    rconv_list_add(list, _rconv_stepmania_new_value_separator(sep_flags));
                    start = old_pos;
                    state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_VALUE;
                } else {
                    rconv_list_add(list, _rconv_stepmania_new_property_end());
                    start = 0;
                    state = RCONV_STEPMANIA_TOKENIZER_STATE_CLEAN;
                }
                continue;
            }

            continue;
        }

        if (state == RCONV_STEPMANIA_TOKENIZER_STATE_IN_NOTES_META) {
            if (c == ',') {
                size_t old_pos = dd->position;
                size_t length = old_pos - start - 1;
                RconvToken* token = _rconv_stepmania_parse_token(dd, start, length);
                token->type = RCONV_TOKEN_STRING;
                rconv_list_add(list, token);
                rconv_list_add(list, _rconv_stepmania_new_value_separator(RCONV_TOKENFLAG_STEPMANIA_VALUE_SEPARATOR_COMMA));
                
                start = old_pos;
                continue;
            }

            if (c == ':') {
                size_t old_pos = dd->position;
                size_t length = old_pos - start - 1;
                RconvToken* token = _rconv_stepmania_parse_token(dd, start, length);
                token->type = RCONV_TOKEN_STRING;
                rconv_list_add(list, token);

                start = old_pos;
                meta_count++;

                if (meta_count >= 5) {
                    snap_count = 0;
                    state = RCONV_STEPMANIA_TOKENIZER_STATE_IN_NOTES;
                    continue;
                }

                continue;
            }

            continue;
        }

        if (state == RCONV_STEPMANIA_TOKENIZER_STATE_IN_NOTES) {
            if (c == ';') {
                rconv_list_add(list, _rconv_stepmania_new_property_end());
                state = RCONV_STEPMANIA_TOKENIZER_STATE_CLEAN;
                continue;
            }

            if (c == ',') {
                snap_count = 0;
                continue;
            }

            if (c == '\n') {
                snap_count++;
                continue;
            }

            if (c == RCONV_STEPMANIA_NOTE_EMPTY
                || c == RCONV_STEPMANIA_NOTE_NOTE
                || c == RCONV_STEPMANIA_NOTE_HOLD
                || c == RCONV_STEPMANIA_NOTE_HEND
                || c == RCONV_STEPMANIA_NOTE_ROLL
                || c == RCONV_STEPMANIA_NOTE_MINE
                || c == RCONV_STEPMANIA_NOTE_LIFT
                || c == RCONV_STEPMANIA_NOTE_FAKE
            ) {
                char* tmp = malloc(sizeof(char) * 2);
                tmp[0] = c;
                tmp[1] = '\0';
                RconvToken* token = calloc(1, sizeof(RconvToken));
                token->type = RCONV_STEPMANIA_TOKEN_NOTE;
                token->content = tmp;
                rconv_list_add(list, token);
                continue;
            }

            continue;
        }

        continue;
    }

    RconvListEntry* e = list->head;
    while (e != NULL) {
        rconv_print_token((RconvToken*) e->value);
        e = e->next;
    }

    RconvToken** out = rconv_list_to_token_array(list, length);
    rconv_list_free(list);

    return out;
}
