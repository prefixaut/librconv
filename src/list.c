#include "./list.h"

RconvList*
rconv_list()
{
	RconvList* list = (RconvList*) malloc(sizeof(RconvList));
	if (list == NULL) {
		return NULL;
	}

	list->head = NULL;
	list->tail = NULL;
	list->size = 0;

	return list;
}

void
rconv_list_add(RconvList* list, void* element)
{
	RconvListEntry* new_entry = (RconvListEntry*) malloc(sizeof(RconvListEntry));
	if (new_entry == NULL) {
		return;
	}

	new_entry->value = element;
	new_entry->next = NULL;

	if (list->head == NULL) {
		list->head = new_entry;
		list->tail = new_entry;
	} else {
		list->tail->next = new_entry;
		list->tail = new_entry;
	}

	list->size++;
}

void
rconv_list_unshift(RconvList* list, void* element)
{
	RconvListEntry* new_entry = (RconvListEntry*) malloc(sizeof(RconvListEntry));
	if (new_entry == NULL) {
		return;
	}

	new_entry->value = element;
	new_entry->next = NULL;

	if (list->head == NULL) {
		list->head = new_entry;
		list->tail = new_entry;
	} else {
		new_entry->next = list->head;
		list->head = new_entry;
	}

	list->size++;
}

void*
rconv_list_shift(RconvList* list, void* element)
{
	if (list->head == NULL) {
		return NULL;
	}

	RconvListEntry* entry = list->head;
	list->head = entry->next;
	list->size--;

	void* tmp = entry->value;
	free(entry);

	return tmp;
}

void*
rconv_list_pop(RconvList* list)
{
	if (list->tail == NULL) {
		return NULL;
	}

	RconvListEntry* entry = list->tail;

	if (list->tail == list->head) {
		list->tail = NULL;
		list->head = NULL;
		list->size = 0;
	} else {
		RconvListEntry* newTail = list->head;
		for (int i = 1; i < list->size - 1; i++) {
			newTail = newTail->next;
		}
		list->tail = newTail;
		list->size--;
	}

	void* tmp = entry->value;
	free(entry);

	return tmp;
}

void
rconv_list_remove(RconvList* list, int index)
{
	if (list->tail == NULL) {
		return;
	}

	RconvListEntry* entry = list->tail;

	if (list->tail == list->head) {
		list->tail = NULL;
		list->head = NULL;
		list->size = 0;
	} else {
		RconvListEntry* newTail = list->head;
		for (int i = 1; i < index; i++) {
			newTail = newTail->next;
		}
		list->tail = newTail;
		list->size--;
	}

	free(entry);
}

void*
rconv_list_get(RconvList* list, int index)
{
	if (list->head == NULL) {
		return NULL;
	}

	RconvListEntry* entry = list->head;

	for (int i = 1; i <= index; i++) {
		entry = entry->next;
	}

	return entry->value;
}

void
rconv_list_free(RconvList* list)
{
	RconvListEntry* entry = list->head;

	for (size_t i = 1; i < list->size; i++) {
		RconvListEntry* tmp = entry->next;
		free(entry);
		entry = tmp;
	}

	free(entry);
}

RCONV_LIST_TO_ARRAY_GEN_NAMED(char, string)
