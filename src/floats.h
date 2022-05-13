#pragma once

#include <stdlib.h>
#include <mpdecimal.h>

void
rconv_float_init();

void
rconv_float_from_string(mpd_t* result, const char* str);
