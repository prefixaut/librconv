#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    RCONV_DD_TYPE_FILE,
    RCONV_DD_TYPE_MEMORY,
} RconvDataDescriptorType;

typedef struct {
    RconvDataDescriptorType type;

    size_t position;
    size_t length;

    FILE* file_ptr;
    void* mem_ptr;
} RconvDataDescriptor;

RconvDataDescriptor*
rconv_dd_new_from_file(FILE* fp);

RconvDataDescriptor*
rconv_dd_new_from_memory(void* memory, size_t length);

RconvDataDescriptor*
rconv_dd_new_from_string(char* content);

void
rconv_dd_free(RconvDataDescriptor* dd);

bool
rconv_dd_is_eof(RconvDataDescriptor* dd);

bool
rconv_dd_set_position(RconvDataDescriptor* dd, size_t position);

size_t
rconv_dd_read(RconvDataDescriptor* dd, void* target, size_t length);

size_t
rconv_dd_read_string(RconvDataDescriptor* dd, char* target, size_t length);

char
rconv_dd_read_char(RconvDataDescriptor* dd);

char*
rconv_dd_read_utf8char(RconvDataDescriptor* dd, int* length);
