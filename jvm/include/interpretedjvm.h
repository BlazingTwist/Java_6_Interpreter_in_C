#pragma once

#include "config.h"
#include "interpreter/objectinstance.h"
#include "interpreter/classloader.h"
#include "interpreter/executioncontext.h"

typedef struct InterpretedJVM {
    ClassLoader *classLoader;
    ExecutionContext *context;
} InterpretedJVM;

InterpretedJVM prepareJVM(const char **sourceFiles[], int numFiles);

void releaseJVM(InterpretedJVM *jvm);

void push_U4(InterpretedJVM *jvm, U4 value);

void push_Object(InterpretedJVM *jvm, HeapPointer value);

void push_Method(InterpretedJVM *jvm, const char *className, const char *methodName, const char *descriptor);

void executeAllMethods_NoReturn(InterpretedJVM *jvm);

HeapPointer executeAllMethods_SingleReturn(InterpretedJVM *jvm);
