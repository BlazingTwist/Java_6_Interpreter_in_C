#pragma once

#include "../definition/integral_types.h"
#include "classloader.h"
#include "executionframe.h"

typedef struct ExecutionContext {
    ClassLoader *classLoader;

    U4 maxFrameStackDepth;
    U4 currentFrameStackDepth;
    struct ExecutionFrame *executionFrameStack;

    U4 maxOperandStackDepth;
    U4 currentOperandStackDepth;
    HeapPointer*operandStack;

    U4 numStaticInstances;
    HeapPointer*staticInstances;
} ExecutionContext;

ExecutionContext *initExecutionContext(ClassLoader *classLoader, U4 maxFrameStack, U4 maxOperandStack);

void releaseExecutionContext(ExecutionContext *context);

void ensureStaticInstance(ExecutionContext *context, const ClassFile *classFile);

void pushOperand(ExecutionContext *context, HeapPointer heapPointer);

HeapPointer createObjectRef_string(ExecutionContext *context, const CP_Info *utf8);

HeapPointer popOperand(ExecutionContext *context);

U4 popIntOperand(ExecutionContext *context);

void popExecutionFrame(ExecutionContext *context);

void pushMethod(ExecutionContext *context, const ClassFile *classFile, const MethodInfo *methodInfo);

void pushVirtualMethod(ExecutionContext *context, const ClassFile *instanceClass, const ClassFile *currentClass,
                       const CP_Info *methodNameUtf8, const CP_Info *methodDescriptorUtf8);

bool handleSystemMethod(ExecutionContext *context, const ClassFile *classFile, const MethodInfo *methodInfo);

void pushFrameLocal(ExecutionContext *context, ExecutionFrame *frame, U2 index);

void storeFrameLocal(ExecutionContext *context, ExecutionFrame *frame, U2 index);

bool executeNextInstruction(ExecutionContext *context, ExecutionFrame *frame);

bool execute(ExecutionContext *context);
