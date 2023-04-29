#pragma once

#include "../definition/integral_types.h"
#include "../definition/classfile.h"
#include "objectinstance.h"

typedef struct ExecutionFrame {
    U4 programCounter;
    struct ObjectInstance **locals; // HeapPointer array

    U4 initialOperandStackDepth;
    const ClassFile *thisClassFile;
    const MethodInfo *thisMethodInfo;
    const CodeAttribute *codeAttribute;
} ExecutionFrame;

void assertHasCodeAttribute(const ClassFile *classFile, const MethodInfo *methodInfo);

void initExecutionFrame(ExecutionFrame *frame,
                        U4 currentOperandStackDepth,
                        const ClassFile *thisClassFile,
                        const MethodInfo *thisMethodInfo);
