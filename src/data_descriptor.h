#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    RCONV_DD_TYPE_FILE,
    RCONV_DD_TYPE_STRING,
} RconvDataDescriptorType;

typedef struct {
    RconvDataDescriptorType type;

    size_t position;

    char* file_path;
    FILE* file_ptr;

    char* str_ptr;
    size_t str_len;
} RconvDataDescriptor;

RconvDataDescriptor*
rconv_dd_new_from_file(char* file_path);

RconvDataDescriptor*
rconv_dd_new_from_string(char* content);

void
rconv_dd_free(RconvDataDescriptor* dd);

bool
rconv_dd_is_eof(RconvDataDescriptor* dd);

bool
rconv_dd_set_position(RconvDataDescriptor* dd, size_t position);

void
rconv_dd_read(RconvDataDescriptor* dd, char* target, size_t length);

char
rconv_dd_read_char(RconvDataDescriptor* dd);
