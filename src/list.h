#pragma once

#include <limits.h>
#include <stdlib.h>

#define RCONV_LIST_TO_ARRAY_GEN_H_NAMED(type, name) type** rconv_list_to_##name##_array(RconvList* list, int* array_length);

#define RCONV_LIST_TO_ARRAY_GEN_NAMED(type, name)                           \
	type** rconv_list_to_##name##_array(RconvList* list, int* array_length) \
	{                                                                       \
		if (list->size == 0) {                                              \
			*array_length = 0;                                              \
			return NULL;                                                    \
		}                                                                   \
                                                                            \
		type** target = (type**) malloc(list->size * sizeof(type*));        \
		if (target == NULL) {                                               \
			*array_length = 0;                                              \
			return NULL;                                                    \
		}                                                                   \
		RconvListEntry* e = list->head;                                     \
		int i = 0;                                                          \
                                                                            \
		do {                                                                \
			if (e == NULL) {                                                \
				break;                                                      \
			}                                                               \
			target[i] = (type*) e->value;                                   \
			e = e->next;                                                    \
			i++;                                                            \
		} while (i < list->size);                                           \
		*array_length = i;                                                  \
		if (e != NULL) {                                                    \
			*array_length += 1;                                             \
		}                                                                   \
                                                                            \
		return target;                                                      \
	}

#define RCONV_LIST_TO_ARRAY_GEN_H(type) RCONV_LIST_TO_ARRAY_GEN_H_NAMED(type, type)

#define RCONV_LIST_TO_ARRAY_GEN(type) RCONV_LIST_TO_ARRAY_GEN_NAMED(type, type)

typedef struct RconvListEntry {
	void* value;
	struct RconvListEntry* next;
} RconvListEntry;

typedef struct {
	RconvListEntry* head;
	RconvListEntry* tail;
	int size;
} RconvList;

RCONV_LIST_TO_ARRAY_GEN_H_NAMED(char, string)

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

void**
rconv_list_to_array(RconvList* list, int* size);
