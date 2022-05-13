#pragma once

#include <stdlib.h>
#include <limits.h>

#define RCONV_LIST_TO_ARRAY_GEN_H(type) type** rconv_list_to_array_##type(RconvList* list, int* array_length);

#define RCONV_LIST_TO_ARRAY_GEN(type) type** rconv_list_to_array_##type(RconvList* list, int* array_length) { \
if (list->size == 0) { \
	*array_length = 0; \
	return NULL; \
} \
\
int size = sizeof(##type*); \
##type** target = (##type**) malloc(list->size * size); \
RconvListEntry* e = list->head; \
int i = 0; \
\
do { \
	*(target + (i * size)) = (##type*) e->value; \
	e = e->next; \
} while (i < list->size); \
\
return target; \
}

typedef struct RconvListEntry {
	void* value;
	struct RconvListEntry* next;
} RconvListEntry;

typedef struct {
	RconvListEntry* head;
	RconvListEntry* tail;
	size_t size;
} RconvList;

RCONV_LIST_TO_ARRAY_GEN_H(char)

RconvList*
rconv_list();

void
rconv_list_add(RconvList* list, void* element);

void
rconv_list_unshift(RconvList* list, void* element);

void*
rconv_list_shift(RconvList* list, void* element);

void*
rconv_list_pop(RconvList* list);

void
rconv_list_remove(RconvList* list, int index);

void*
rconv_list_get(RconvList* list, int index);

void
rconv_list_free(RconvList* list);

int*
rconv_list_to_array(RconvList* list, int* size);
