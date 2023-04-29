#pragma once

#include "stdlib.h"

void *allocateBytes(size_t numBytes);

void deAllocateBytes(void *data);

void printMemStats();
