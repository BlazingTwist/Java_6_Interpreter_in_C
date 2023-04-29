#include "../../include/util/bytereader.h"

#include "stdlib.h"
#include "stdio.h"

U1 readU1(ByteReader *reader) {
    assert_canRead(reader, 1);
    U1 result = reader->bytes[reader->position];
    reader->position += 1;
    return result;
}

U2 readU2(ByteReader *reader) {
    assert_canRead(reader, 2);
    U2 result = reader->bytes[reader->position];
    result = (result << 8) | reader->bytes[reader->position + 1];
    reader->position += 2;
    return result;
}

U4 readU4(ByteReader *reader) {
    assert_canRead(reader, 4);
    U4 result = reader->bytes[reader->position];
    result = (result << 8) | reader->bytes[reader->position + 1];
    result = (result << 8) | reader->bytes[reader->position + 2];
    result = (result << 8) | reader->bytes[reader->position + 3];
    reader->position += 4;
    return result;
}

U8 readU8(ByteReader *reader) {
    assert_canRead(reader, 8);
    U8 result = reader->bytes[reader->position];
    result = (result << 8) | reader->bytes[reader->position + 1];
    result = (result << 8) | reader->bytes[reader->position + 2];
    result = (result << 8) | reader->bytes[reader->position + 3];
    result = (result << 8) | reader->bytes[reader->position + 4];
    result = (result << 8) | reader->bytes[reader->position + 5];
    result = (result << 8) | reader->bytes[reader->position + 6];
    result = (result << 8) | reader->bytes[reader->position + 7];
    reader->position += 8;
    return result;
}

float readFloat(ByteReader *reader) {
    return (float) readU4(reader);
}

double readDouble(ByteReader *reader) {
    return (double) readU8(reader);
}

ByteReader readBytes(ByteReader *reader, U4 numBytes) {
    assert_canRead(reader, numBytes);
    ByteReader result = {
            reader->position,
            reader->position,
            reader->position + numBytes,
            reader->bytes
    };
    reader->position += numBytes;
    return result;
}

const U1 *readByteArray(ByteReader *reader, U4 numBytes) {
    assert_canRead(reader, numBytes);
    const U1 *result = reader->bytes + reader->position;
    reader->position += numBytes;
    return result;
}

void assert_canRead(ByteReader *reader, U4 numBytes) {
    if ((reader->position + numBytes) > reader->byteEnd) {
        printf("No more bytes left to read!\n");
        exit(-1);
    }
}
