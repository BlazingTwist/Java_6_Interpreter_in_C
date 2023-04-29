#include "../../include/util/utf8_utils.h"
#include <stdbool.h>

bool utf8Equals(const U1 *utf8_left, const U1 *utf8_right, const U2 numBytes) {
    for (int i = 0; i < numBytes; ++i) {
        if (utf8_left[i] != utf8_right[i]) {
            return false;
        }
    }
    return true;
}

bool utf8Matches(const char *string, const U1 *utf8, U4 numUtf8Bytes) {
    for (int i = 0; i < numUtf8Bytes; ++i) {
        if (string[i] == 0) {
            return false;
        }
        if (string[i] != utf8[i]) {
            return false;
        }
    }
    return string[numUtf8Bytes] == 0;
}
