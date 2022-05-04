#include "./list.h"

RconvList*
rconv_list()
{
    RconvList* list = malloc(sizeof(RconvList));
	list->head = NULL;
    list->tail = NULL;
	list->size = 0;

    return list;
}

void
rconv_list_add(RconvList* list, int element)
{
    RconvListEntry* e = malloc(sizeof(RconvListEntry));
    e->entryPtr = element;
    e->next = NULL;

    if (&list->head == NULL) {
        list->head = e;
        list->tail = e;
    } else {
        list->tail->next = e;
        list->tail = e;
    }

    list->size++;
}

void
rconv_list_unshift(RconvList* list, int element)
{
    RconvListEntry* e = malloc(sizeof(RconvListEntry));
    e->entryPtr = element;
    e->next = NULL;

    if (&list->head == NULL) {
        list->head = e;
        list->tail = e;
    } else {
        e->next = list->head;
        list->head = e;
    }

    list->size++;
}

int
rconv_list_shift(RconvList* list, int element)
{
    if (list->head == NULL) {
        return NULL;
    }

    RconvListEntry* e = list->head;
    list->head = e->next;
    list->size--;

    int tmp = e->entryPtr;
    free(e);

    return tmp;
}

int
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

    int tmp = e->entryPtr;
    free(e);

    return tmp;
}

void
rconv_list_remove(RconvList* list, int index)
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

    free(e);
}

int
rconv_list_get(RconvList* list, int index)
{
    if (list->head == NULL) {
        return NULL;
    }

    RconvListEntry* e = list->head;

    for (int i = 1; i <= index; i++) {
        e = e->next;
    }

    return e->entryPtr;
}

void
rconv_list_free(RconvList* list)
{
    RconvListEntry* e = list->head;

    for (int i = 1; i < list->size; i++) {
        RconvListEntry* tmp = e->next;
        free(e);
        e = tmp;
    }

    free(e);
}

int*
rconv_list_to_array(RconvList* list, int* size)
{
	if (list->size == 0) {
		*size = 0;
		return NULL;
	}

	int* target = malloc(list->size * sizeof(int));
    RconvListEntry* e = list->head;
    int i = 0;

    do {
        *(target + (i * sizeof(int))) = e->entryPtr;
        e = e->next;
    } while (i < list->size);

	return target;
}
