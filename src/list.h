#pragma once

#ifndef _INC_RCONV_LIST
#	define _INC_RCONV_LIST

#	include <limits.h>

typedef struct {
    int entryPtr;
    int next;
} RconvListEntry;

typedef struct {
	RconvListEntry* head;
    RconvListEntry* tail;
	size_t size;
} RconvList;

RconvList*
rconv_list();

void
rconv_list_add(RconvList* list, int element);

void
rconv_list_unshift(RconvList* list);

int
rconv_list_shift(RconvList* list, int element);

int
rconv_list_pop(RconvList* list);

void
rconv_list_remove(RconvList* list, int index);

int
rconv_list_get(RconvList* list, int index);

void
rconv_list_free(RconvList* list);

int
rconv_list_to_array(RconvList* list, int* size);

#endif