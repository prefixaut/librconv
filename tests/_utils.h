#pragma once

#include <check.h>

#define rconv_ck_decimal_equal(value, expected_integer, expected_fraction) \
    ck_assert_ptr_nonnull(value); \
    ck_assert_int_eq(value->integer, expected_integer); \
    if (expected_fraction < 0) { \
        ck_assert_int_eq(value->negative, true); \
        ck_assert_int_eq(value->fraction, expected_fraction * -1); \
    } else { \
        ck_assert_int_eq(value->fraction, expected_fraction); \
    }
