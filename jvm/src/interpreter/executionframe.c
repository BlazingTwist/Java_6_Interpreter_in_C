#include "../../include/interpreter/executionframe.h"
#include "../../include/util/allocator.h"
#include <stdio.h>
#include <stdlib.h>

void assertHasCodeAttribute(const ClassFile *classFile, const MethodInfo *methodInfo) {
    CodeAttribute *codeAttribute = methodInfo->codeAttribute;
    if (codeAttribute == NULL) {
        U1 *methodName = cp_utf8_getCString(&classFile->constPoolEntries[methodInfo->nameIndex]);
        printf("cannot execute method '%s'. Method has no codeAttribute.\n", methodName);
        deAllocateBytes(methodName);
        exit(-1);
    }
}

void initExecutionFrame(ExecutionFrame *frame,
                        U4 currentOperandStackDepth,
                        const ClassFile *thisClassFile,
                        const MethodInfo *thisMethodInfo) {

    const CodeAttribute *codeAttribute = thisMethodInfo->codeAttribute;
    HeapPointer*locals = allocateBytes(sizeof(HeapPointer) * codeAttribute->maxLocals);
    for (int i = 0; i < codeAttribute->maxLocals; ++i) {
        locals[i] = NULL;
    }
    frame->programCounter = 0;
    frame->locals = locals;
    frame->initialOperandStackDepth = currentOperandStackDepth;
    frame->thisClassFile = thisClassFile;
    frame->thisMethodInfo = thisMethodInfo;
    frame->codeAttribute = codeAttribute;
}
