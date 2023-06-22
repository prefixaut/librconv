#include "utils.h"

void
rconv_ck_decimal_equal(RconvDecimal* value, int integer, int fraction)
{
    ck_assert_ptr_nonnull(value);
    ck_assert_int_eq(value->integer, integer);
    ck_assert_int_eq(value->fraction, fraction);
}
