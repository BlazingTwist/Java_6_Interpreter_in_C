#include "../../include/util/hex_utils.h"

#include <stdio.h>
#include <stdlib.h>

U1 readHexChar(char c) {
    const char *strValue = "0123456789abcdef";
    for (int i = 0; i < 16; i++) {
        if (c == strValue[i]) {
            return i;
        }
    }
    printf("unknown char: '%c'\n", c);
    exit(-1);
}

U1 readHexByte(const char *string, int position) {
    return (readHexChar(string[position]) << 4) | readHexChar(string[position + 1]);
}

U4 readHexU4(const char *string, int position) {
    U4 result = readHexByte(string, position);
    result = (result << 8) | readHexByte(string, position + 2);
    result = (result << 8) | readHexByte(string, position + 4);
    result = (result << 8) | readHexByte(string, position + 6);
    return result;
}
