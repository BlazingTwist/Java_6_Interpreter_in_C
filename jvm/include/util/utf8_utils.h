#pragma once

#include <stdbool.h>
#include "../definition/integral_types.h"

bool utf8Equals(const U1 *utf8_left, const U1 *utf8_right, const U2 numBytes);

bool utf8Matches(const char *string, const U1 *utf8, U4 numUtf8Bytes);
