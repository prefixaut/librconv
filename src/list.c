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
	RconvListEntry* e = (RconvListEntry*) malloc(sizeof(RconvListEntry));
	if (e == NULL) {
		return;
	}

	e->value = element;
	e->next = NULL;

	if (list->head == NULL) {
		list->head = e;
		list->tail = e;
	} else {
		list->tail->next = e;
		list->tail = e;
	}

	list->size++;
}

void
rconv_list_unshift(RconvList* list, void* element)
{
	RconvListEntry* e = (RconvListEntry*) malloc(sizeof(RconvListEntry));
	if (e == NULL) {
		return;
	}

	e->value = element;
	e->next = NULL;

	if (list->head == NULL) {
		list->head = e;
		list->tail = e;
	} else {
		e->next = list->head;
		list->head = e;
	}

	list->size++;
}

void*
rconv_list_shift(RconvList* list, void* element)
{
	if (list->head == NULL) {
		return NULL;
	}

	RconvListEntry* e = list->head;
	list->head = e->next;
	list->size--;

	void* tmp = e->value;
	free(e);

	return tmp;
}

void*
rconv_list_pop(RconvList* list)
{
	if (list->tail == NULL) {
		return NULL;
	}

	RconvListEntry* e = list->tail;

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

	void* tmp = e->value;
	free(e);

	return tmp;
}

void
rconv_list_remove(RconvList* list, int index)
{
	if (list->tail == NULL) {
		return;
	}

	RconvListEntry* e = list->tail;

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

	free(e);
}

void*
rconv_list_get(RconvList* list, int index)
{
	if (list->head == NULL) {
		return NULL;
	}

	RconvListEntry* e = list->head;

	for (int i = 1; i <= index; i++) {
		e = e->next;
	}

	return e->value;
}

void
rconv_list_free(RconvList* list)
{
	RconvListEntry* e = list->head;

	for (size_t i = 1; i < list->size; i++) {
		RconvListEntry* tmp = e->next;
		free(e);
		e = tmp;
	}

	free(e);
}

RCONV_LIST_TO_ARRAY_GEN_NAMED(char, string)
