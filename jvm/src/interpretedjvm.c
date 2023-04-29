#include "../include/interpretedjvm.h"
#include "../include/util/allocator.h"
#include "../include/util/sourcefiledecoder.h"
#include "../include/interpreter/instructionhelper.h"
#include <stdio.h>
#include <stdlib.h>

InterpretedJVM prepareJVM(const char **sourceFiles[], int numFiles) {
    InterpretedJVM jvm = {};
    jvm.classLoader = initClassLoader(numFiles);

    for (int i = 0; i < numFiles; i++) {
        SourceFileInfo *sourceFile = readToSourceFile(sourceFiles[i]);
        loadClassFile(jvm.classLoader, sourceFile);
    }

    jvm.context = initExecutionContext(jvm.classLoader, 128, 256);

#ifdef PRINT_MEM_STATS
    printf("Finished initializing JVM. ");
    printMemStats();
#endif

    return jvm;
}

void releaseJVM(InterpretedJVM *jvm) {
    releaseExecutionContext(jvm->context);
    jvm->context = NULL;
    releaseClassLoader(jvm->classLoader);
    jvm->classLoader = NULL;

#ifdef PRINT_MEM_STATS
    printf("Release JVM. ");
    printMemStats();
#endif
}

void push_U4(InterpretedJVM *jvm, U4 value) {
    pushOperand(jvm->context, createPrimitive_int(value));
}

void push_Object(InterpretedJVM *jvm, HeapPointer value) {
    pushOperand(jvm->context, value);
}

void push_Method(InterpretedJVM *jvm, const char *className, const char *methodName, const char *descriptor) {
    const ClassFile *targetClass = getClassFile_byName(jvm->classLoader, className);
    const MethodInfo *entryPoint = findMethod(targetClass, methodName, descriptor);
    pushMethod(jvm->context, targetClass, entryPoint);
}

void _executeAllMethods(InterpretedJVM *jvm) {
    while (true) {
        if (!execute(jvm->context)) {
            return;
        }
    }
}

void executeAllMethods_NoReturn(InterpretedJVM *jvm) {
    _executeAllMethods(jvm);
    if (jvm->context->currentOperandStackDepth != 0) {
        printf("Expected EntryPoint to have no return value. Found %lu",
               jvm->context->currentOperandStackDepth);
        exit(-1);
    }
}

HeapPointer executeAllMethods_SingleReturn(InterpretedJVM *jvm) {
    _executeAllMethods(jvm);
    if (jvm->context->currentOperandStackDepth != 1) {
        printf("Expected EntryPoint to have exactly one return value. Found %lu",
               jvm->context->currentOperandStackDepth);
        exit(-1);
    }

    HeapPointer returnValue = popOperand(jvm->context);
    if (returnValue->activeReferences != 1) {
        printf("unexpected amount of active references on return value! %d", returnValue->activeReferences);
        exit(-1);
    }
    return returnValue;
}
