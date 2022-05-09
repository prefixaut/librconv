#include "floats.h"

mpd_context_t* context = NULL;

void
rconv_float_init()
{
	context = (mpd_context_t*) malloc(sizeof(mpd_context_t));
	mpd_defaultcontext(context);
}

void
rconv_float_from_string(mpd_t* result, const char* str)
{
	mpd_set_string(result, str, context);
}
