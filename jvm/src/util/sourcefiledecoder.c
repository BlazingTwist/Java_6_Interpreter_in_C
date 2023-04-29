#include "../../include/util/sourcefiledecoder.h"
#include "../../include/util/hex_utils.h"
#include "../../include/util/allocator.h"

void transferChunk(const char *encodedChunk, U1 *accumulator, U4 accumulatorOffset, U2 numBytes) {
    for (int i = 0; i < numBytes; ++i) {
        accumulator[i + accumulatorOffset] = readHexByte(encodedChunk, i * 2);
    }
}

SourceFileInfo *readToSourceFile(const char *file[]) {
    U4 numFileBytes = readHexU4(file[0], 0);
    U1 *result = allocateBytes(sizeof(U1) * numFileBytes);

    U4 bytesRead = 0;
    U4 chunkIndex = 1;
    while ((numFileBytes - bytesRead) >= 64) {
        transferChunk(file[chunkIndex], result, bytesRead, 64);
        bytesRead += 64;
        chunkIndex++;
    }

    U4 remainingBytes = numFileBytes - bytesRead;
    if (remainingBytes > 0) {
        transferChunk(file[chunkIndex], result, bytesRead, remainingBytes);
    }

    SourceFileInfo *sourceFile = allocateBytes(sizeof(SourceFileInfo));
    sourceFile->numBytes = numFileBytes;
    sourceFile->bytes = result;
    return sourceFile;
}
