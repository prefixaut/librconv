#include "memo.h"

#include <stdlib.h>

typedef struct {
	int len;
	RconvMemoToken* tokens;
} RconvMemoTokenElement;

int
_rconv_memo_hold_offset(int position)
{
	switch (position) {
	case HOLD_UP: return -4;
	case HOLD_DOWN: return 4;
	case HOLD_LEFT: return -1;
	case HOLD_RIGHT: return 1;
	default: return 0;
	}
}

RconvMemoTokenElement*
_rconv_memo_parse_tokens(char* data[], int from, int to)
{
	RconvMemoTokenElement* elem = malloc(1 * sizeof(RconvMemoTokenElement));
	return elem;
}
