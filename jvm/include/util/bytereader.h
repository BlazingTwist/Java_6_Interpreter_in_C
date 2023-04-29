#pragma once

#include "../definition/integral_types.h"

typedef struct ByteReader {
    U4 position;
    const U4 byteStart;
    const U4 byteEnd;
    const U1 *bytes;
} ByteReader;

U1 readU1(ByteReader *reader);

U2 readU2(ByteReader *reader);

U4 readU4(ByteReader *reader);

U8 readU8(ByteReader *reader);

float readFloat(ByteReader *reader);

double readDouble(ByteReader *reader);

ByteReader readBytes(ByteReader *reader, U4 numBytes);

const U1 *readByteArray(ByteReader *reader, U4 numBytes);

void assert_canRead(ByteReader *reader, U4 numBytes);
