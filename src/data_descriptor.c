#include "data_descriptor.h"

RconvDataDescriptor*
rconv_dd_new_from_file(char* file_path)
{
    FILE* fp = fopen(file_path, "r");
    if (fp == NULL) {
        return NULL;
    }

    RconvDataDescriptor* dd = malloc(sizeof(RconvDataDescriptor));
    dd->type = RCONV_DD_TYPE_FILE;
    dd->file_path = file_path;
    dd->file_ptr = fp;
    dd->str_ptr = NULL;
    dd->str_len = 0;
    dd->position = 0;

    return dd;
}

RconvDataDescriptor*
rconv_dd_new_from_string(char* content)
{
    if (content == NULL) {
        return NULL;
    }

    RconvDataDescriptor* dd = malloc(sizeof(RconvDataDescriptor));
    dd->type = RCONV_DD_TYPE_STRING;
    dd->file_path = NULL;
    dd->file_ptr = NULL;
    dd->str_ptr = content;
    dd->str_len = strlen(content);
    dd->position = 0;

    return dd;
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

    if (dd->type == RCONV_DD_TYPE_FILE) {
        return feof(dd->file_ptr) == 0;
    } else if (dd->type == RCONV_DD_TYPE_STRING) {
        return dd->position >= dd->str_len;
    }

    return true;
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
    } else if (dd->type == RCONV_DD_TYPE_STRING) {
        if (position > dd->str_len) {
            return false;
        }
        dd->position = position;
        return true;
    }

    return false;
}

void
rconv_dd_read(RconvDataDescriptor* dd, char* target, size_t length)
{
    if (dd == NULL || target == NULL || rconv_dd_is_eof(dd)) {
        return;
    }

    if (dd->type == RCONV_DD_TYPE_FILE) {
        fread(target, sizeof(char), length + 1, dd->file_ptr);
        dd->position += length;
        return;
    } else if (dd->type == RCONV_DD_TYPE_STRING) {
        size_t new_pos = dd->position + length;
        size_t new_len = length;

        // Check the bounds of the read. If it would read out of bounds, resize
        // the read to be inbounds.
        if (new_pos > dd->str_len) {
            size_t overflow = new_pos - dd->str_len;
            new_len -= overflow;
            new_pos = dd->str_len;
        }

        if (new_len > 0) {
            strncpy(target, dd->str_ptr + dd->position, new_len);
        }
        target[new_len + 1] = '\0';
        dd->position = new_pos;

        return;
    }
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
    } else if (dd->type == RCONV_DD_TYPE_STRING) {
        char c = dd->str_ptr[dd->position];
        dd->position++;
        return c;
    }

    return EOF;
}
