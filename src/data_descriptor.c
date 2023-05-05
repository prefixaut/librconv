#include "data_descriptor.h"

RconvDataDescriptor*
rconv_dd_new_from_file(FILE* fp)
{
    if (fp == NULL) {
        return NULL;
    }

    size_t position = ftell(fp);
    fseek(fp, 0, SEEK_END);
    size_t length = ftell(fp);
    fseek(fp, position, SEEK_SET);

    RconvDataDescriptor* dd = malloc(sizeof(RconvDataDescriptor));
    dd->type = RCONV_DD_TYPE_FILE;
    dd->file_ptr = fp;
    dd->position = position;
    dd->length = length;
    dd->mem_ptr = NULL;

    return dd;
}

RconvDataDescriptor*
rconv_dd_new_from_memory(void* memory, size_t length)
{
    if (memory == NULL) {
        return NULL;
    }

    RconvDataDescriptor* dd = malloc(sizeof(RconvDataDescriptor));
    dd->type = RCONV_DD_TYPE_MEMORY;
    dd->mem_ptr = memory;
    dd->position = 0;
    dd->length = length;
    dd->file_ptr = NULL;

    return dd;
}

RconvDataDescriptor*
rconv_dd_new_from_string(char* content)
{
    if (content == NULL) {
        return NULL;
    }

    return rconv_dd_new_from_memory(content, strlen(content));
}

void
rconv_dd_free(RconvDataDescriptor* dd)
{
    if (dd == NULL) {
        return;
    }

    if (dd->type == RCONV_DD_TYPE_FILE && dd->file_ptr != NULL) {
        fclose(dd->file_ptr);
    }

    free(dd);
}

bool
rconv_dd_is_eof(RconvDataDescriptor* dd)
{
    if (dd == NULL) {
        return true;
    }

    return dd->position >= dd->length;
}

bool
rconv_dd_set_position(RconvDataDescriptor* dd, size_t position)
{
    if (dd == NULL) {
        return false;
    }

    if (dd->type == RCONV_DD_TYPE_FILE) {
        bool changed = fseek(dd->file_ptr, position, SEEK_SET) == 0;
        if (changed) {
            dd->position = position;
        }
        return changed;

    } else if (dd->type == RCONV_DD_TYPE_MEMORY) {
        if (position > dd->length) {
            return false;
        }
        dd->position = position;
        return true;
    }

    return false;
}

size_t
rconv_dd_read(RconvDataDescriptor* dd, void* target, size_t length)
{
    if (dd == NULL || target == NULL || rconv_dd_is_eof(dd)) {
        return 0;
    }

    size_t new_pos = dd->position + length;
    size_t new_len = length;

    // Check the bounds of the read. If it would read out of bounds, resize
    // the read to be inbounds.
    if (new_pos > dd->length) {
        size_t overflow = new_pos - dd->length;
        new_len -= overflow;
        new_pos = dd->length;
    }

    if (dd->type == RCONV_DD_TYPE_FILE) {
        fread(target, 1, new_len, dd->file_ptr);
        dd->position += new_len;
        return new_len;

    } else if (dd->type == RCONV_DD_TYPE_MEMORY) {
        if (new_len > 0) {
            memcpy(target, dd->mem_ptr + dd->position, new_len);
        }
        dd->position = new_pos;
        return new_len;
    }

    return 0;
}

size_t
rconv_dd_read_string(RconvDataDescriptor* dd, char* target, size_t length)
{
    if (dd == NULL || target == NULL || rconv_dd_is_eof(dd)) {
        return 0;
    }

    size_t new_pos = dd->position + length;
    size_t new_len = length;

    // Check the bounds of the read. If it would read out of bounds, resize
    // the read to be inbounds.
    if (new_pos > dd->length) {
        size_t overflow = new_pos - dd->length;
        new_len -= overflow;
        new_pos = dd->length;
    }

    if (dd->type == RCONV_DD_TYPE_FILE) {
        fread(target, sizeof(char), new_len, dd->file_ptr);
        target[new_len + 1] = '\0';
        dd->position += new_len;
        return new_len;

    } else if (dd->type == RCONV_DD_TYPE_MEMORY) {
        if (new_len > 0) {
            strncpy(target, dd->mem_ptr + dd->position, new_len);
        }
        target[new_len + 1] = '\0';
        dd->position = new_pos;

        return new_len;
    }

    return 0;
}

char
rconv_dd_read_char(RconvDataDescriptor* dd)
{
    if (dd == NULL || rconv_dd_is_eof(dd)) {
        return EOF;
    }

    if (dd->type == RCONV_DD_TYPE_FILE) {
        char c = fgetc(dd->file_ptr);
        dd->position++;
        return c;

    } else if (dd->type == RCONV_DD_TYPE_MEMORY) {
        char c = ((char*) dd->mem_ptr)[dd->position];
        dd->position++;
        return c;
    }

    return EOF;
}

char*
rconv_dd_read_utf8char(RconvDataDescriptor* dd, int* length)
{
    char* out = NULL;
    int len = 0;

    char first = rconv_dd_read_char(dd);

    if (first == EOF) {
        *length = len;
        return NULL;
    }
    
    if (0b11110000 == (0b11110000 & first)) {
        len = 4;
    } else if (0b11100000 == (0b11100000 & first)) {
        len = 3;
    } else if (0b11000000 == (0b11000000 & first)) {
        len = 2;
    } else {
        len = 1;
    }

    if (len > 0) {
        out = malloc(sizeof(char) * (len + 1));
        out[0] = first;
        bool null_set = false;

        for (int i = 1; i < len; i++) {
            char c = rconv_dd_read_char(dd);

            if (c == EOF) {
                out[i] = '\0';
                null_set = true;
                break;
            }

            out[i] = c;
        }

        if (!null_set) {
            out[len] = '\0';
        }
    }
    
    *length = len;
    return out;
}
