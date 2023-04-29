#pragma once

#include "integral_types.h"
#include "attribute.h"

typedef struct Instruction {
    U1 numBytes;
    const U1 *bytes;
} Instruction;

typedef struct ExceptionTableEntry {
    U2 startPC;
    U2 endPC;
    U2 handlerPC;
    U2 catchType;
} ExceptionTableEntry;

typedef struct CodeAttribute {
    U2 maxStack;
    U2 maxLocals;
    U4 numInstructions;
    Instruction *instructions;
    U2 numExceptions;
    ExceptionTableEntry *exceptions;
    U2 numAttributes;
    Attribute *attributes;
} CodeAttribute;
