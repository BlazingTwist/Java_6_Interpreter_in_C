#include "../../include/config.h"
#include "../../include/interpreter/executioncontext.h"
#include "../../include/interpreter/opcodes.h"
#include "../../include/interpreter/instructionhelper.h"
#include "../../include/util/allocator.h"
#include <stdio.h>

ExecutionContext *initExecutionContext(ClassLoader *classLoader, U4 maxFrameStack, U4 maxOperandStack) {
    ExecutionFrame *executionFrameStack = allocateBytes(sizeof(ExecutionFrame) * maxFrameStack);
    HeapPointer*operandStack = allocateBytes(sizeof(HeapPointer) * maxOperandStack);
    HeapPointer*staticInstances = allocateBytes(sizeof(HeapPointer) * classLoader->maxNumClassFiles);

    for (int i = 0; i < maxOperandStack; ++i) {
        operandStack[i] = NULL;
    }
    for (int i = 0; i < classLoader->maxNumClassFiles; ++i) {
        staticInstances[i] = NULL;
    }

    ExecutionContext *context = allocateBytes(sizeof(ExecutionContext));
    context->classLoader = classLoader;
    context->maxFrameStackDepth = maxFrameStack;
    context->currentFrameStackDepth = 0;
    context->executionFrameStack = executionFrameStack;
    context->maxOperandStackDepth = maxOperandStack;
    context->currentOperandStackDepth = 0;
    context->operandStack = operandStack;
    context->numStaticInstances = classLoader->maxNumClassFiles;
    context->staticInstances = staticInstances;
    return context;
}

void releaseExecutionContext(ExecutionContext *context) {
    if (context->currentFrameStackDepth > 0) {
        printf("WARNING: frame stack was not empty!\n");
        for (int i = context->currentFrameStackDepth; i > 0; --i) {
            popExecutionFrame(context);
        }
    }
    deAllocateBytes(context->executionFrameStack);
    context->executionFrameStack = NULL;

    for (int i = 0; i < context->classLoader->maxNumClassFiles; ++i) {
        object_deallocate(context->staticInstances[i]);
        context->staticInstances[i] = NULL;
    }
    deAllocateBytes(context->staticInstances);
    context->staticInstances = NULL;

    for (int i = 0; i < context->currentOperandStackDepth; ++i) {
        object_deallocate(context->operandStack[i]);
        context->operandStack[i] = NULL;
    }
    deAllocateBytes(context->operandStack);
    context->operandStack = NULL;
    context->currentOperandStackDepth = 0;

    deAllocateBytes(context);
}


void ensureStaticInstance(ExecutionContext *context, const ClassFile *classFile) {
    const U4 classLoaderIndex = classFile->classLoaderIndex;
    if (context->staticInstances[classLoaderIndex] != NULL) {
        return;
    }

    HeapPointer instance = createObjectRef(classFile, getFieldCount(context, classFile));
    context->staticInstances[classLoaderIndex] = instance;

    const MethodInfo *staticInitializerMethod = findMethod(classFile, "<clinit>", "()V");
    if (staticInitializerMethod == NULL) {
        return; // this is fine, not all classes have static initializers.
    }

    U4 targetFrameStackDepth = context->currentFrameStackDepth;
    pushMethod(context, classFile, staticInitializerMethod);
    while (true) {
        while (true) {
            if (!executeNextInstruction(context, &context->executionFrameStack[context->currentFrameStackDepth - 1])) {
                break;
            }
        }

        popExecutionFrame(context);

        if (context->currentFrameStackDepth <= targetFrameStackDepth) {
            break;
        }
    }
}

void pushOperand(ExecutionContext *context, HeapPointer heapPointer) {
    if (context->currentOperandStackDepth >= context->maxOperandStackDepth) {
        printf("exceeded max operand stack of '%lu'!\n", context->maxOperandStackDepth);
        exit(-1);
    }
    context->operandStack[context->currentOperandStackDepth] = heapPointer;
    context->currentOperandStackDepth++;
}

HeapPointer createObjectRef_string(ExecutionContext *context, const CP_Info *utf8) {
    const ClassFile *stringClass = getClassFile_byName(context->classLoader, "java/lang/String");
    ensureStaticInstance(context, stringClass);

    HeapPointer stringInstance = createObjectRef(stringClass, stringClass->numFields);

    HeapPointer charArray = createPrimitiveArray_4Byte(cp_utf8_getLength(utf8));
    const U1 *stringBytes = cp_utf8_getBytes(utf8);
    for (int i = 0; i < charArray->numDataBytes; ++i) {
        charArray->data[i] = stringBytes[i];
    }
    objectRef_putFieldValue(stringInstance, findFieldIndexInClass(stringClass, "value"), charArray);

    objectRef_putFieldValue(stringInstance, findFieldIndexInClass(stringClass, "count"),
                            createPrimitive_int(charArray->numDataBytes));

    return stringInstance;
}

HeapPointer popOperand(ExecutionContext *context) {
    context->currentOperandStackDepth--;
    HeapPointer operand = context->operandStack[context->currentOperandStackDepth];
    return operand;
}

U4 popIntOperand(ExecutionContext *context) {
    HeapPointer op = popOperand(context);
    U4 result = primitiveInt_getValue(op);
    decrementReferenceCount(op);
    return result;
}

void popExecutionFrame(ExecutionContext *context) {
    ExecutionFrame *frame = &context->executionFrameStack[context->currentFrameStackDepth - 1];
    for (int i = 0; i < frame->codeAttribute->maxLocals; ++i) {
        decrementReferenceCount(frame->locals[i]);
    }
    deAllocateBytes(frame->locals);
    frame->locals = NULL;

    context->currentFrameStackDepth--;

#ifdef PRINT_METHOD_STACK
    printf("<<< Exit | operands: %lu\n", context->currentOperandStackDepth);
#endif
}

void pushMethod(ExecutionContext *context, const ClassFile *classFile, const MethodInfo *methodInfo) {
    if (handleSystemMethod(context, classFile, methodInfo)) {
        return;
    }

#ifdef PRINT_METHOD_STACK
    U2 classNameIndex = cp_class_getNameIndex(&classFile->constPoolEntries[classFile->thisClassIndex]);
    U1 *classNameCString = cp_utf8_getCString(&classFile->constPoolEntries[classNameIndex]);
    U1 *methodNameCString = cp_utf8_getCString(&classFile->constPoolEntries[methodInfo->nameIndex]);
    printf(">>> Enter: %s::%s\n", classNameCString, methodNameCString);
    deAllocateBytes(classNameCString);
    deAllocateBytes(methodNameCString);
#endif

    assertHasCodeAttribute(classFile, methodInfo);
    U2 numParameters = methodInfo->numParameters;
    ExecutionFrame *frame = context->executionFrameStack + context->currentFrameStackDepth;
    context->currentFrameStackDepth++;

    initExecutionFrame(frame, context->currentOperandStackDepth - numParameters, classFile, methodInfo);
    for (int i = numParameters - 1; i >= 0; --i) {
        frame->locals[i] = popOperand(context); // does not change number of existing references
    }
}

void pushVirtualMethod(ExecutionContext *context, const ClassFile *instanceClass, const ClassFile *currentClass,
                       const CP_Info *methodNameUtf8, const CP_Info *methodDescriptorUtf8) {
    const MethodInfo *virtualMethodInfo = findMethod_utf8(currentClass, methodNameUtf8, methodDescriptorUtf8);
    if (virtualMethodInfo != NULL) {
        pushMethod(context, currentClass, virtualMethodInfo);
        return;
    }

    if (currentClass->superClassIndex != 0) {
        U2 classNameIndex = cp_class_getNameIndex(&currentClass->constPoolEntries[currentClass->superClassIndex]);
        const ClassFile *superClassFile = getClassFile_byName_utf8(context->classLoader,
                                                                   &currentClass->constPoolEntries[classNameIndex]);
        pushVirtualMethod(context, instanceClass, superClassFile, methodNameUtf8, methodDescriptorUtf8);
        return;
    }

    U2 classNameIndex = cp_class_getNameIndex(&instanceClass->constPoolEntries[instanceClass->thisClassIndex]);
    U1 *classNameCString = cp_utf8_getCString(&instanceClass->constPoolEntries[classNameIndex]);
    U1 *methodNameCString = cp_utf8_getCString(methodNameUtf8);
    printf("Unable to find virtual method: '%s::%s'\n", classNameCString, methodNameCString);
    deAllocateBytes(classNameCString);
    deAllocateBytes(methodNameCString);
    exit(-1);
}

bool handleSystemMethod(ExecutionContext *context, const ClassFile *classFile, const MethodInfo *methodInfo) {
    U2 classNameIndex = cp_class_getNameIndex(&classFile->constPoolEntries[classFile->thisClassIndex]);
    const CP_Info *classNameUtf8 = &classFile->constPoolEntries[classNameIndex];
    const CP_Info *methodNameUtf8 = &classFile->constPoolEntries[methodInfo->nameIndex];
    const CP_Info *methodDescriptorUtf8 = &classFile->constPoolEntries[methodInfo->descriptorIndex];

    if (cp_cString_utf8_equals("pgp/utils/Printer", classNameUtf8)) {
        if (cp_cString_utf8_equals("print", methodNameUtf8)
            && cp_cString_utf8_equals("(Ljava/lang/String;)V", methodDescriptorUtf8)) {

            HeapPointer stringOperand = popOperand(context);
            const ClassFile *operandClassFile = getClassFile_byIndex(context->classLoader,
                                                                     objectRef_getClassLoaderFileIndex(stringOperand));
            U4 valueFieldIndex = findFieldIndexInHierarchy(context, operandClassFile, "value");
            HeapPointer stringValue = objectRef_getFieldValue(stringOperand, valueFieldIndex);
            char *cStringValue = primitiveArray_toCString(stringValue);
            printf("Print output from jvm: >>>\n");
            printf("%s\n", cStringValue);
            printf("<<<\n");
            deAllocateBytes(cStringValue);
            decrementReferenceCount(stringOperand);
            return true;
        }
    } else if (cp_cString_utf8_equals("java/lang/System", classNameUtf8)) {
        if (cp_cString_utf8_equals("arraycopy", methodNameUtf8)
            && cp_cString_utf8_equals("(Ljava/lang/Object;ILjava/lang/Object;II)V", methodDescriptorUtf8)) {

            HeapPointer op_length = popOperand(context);
            HeapPointer op_targetOffset = popOperand(context);
            HeapPointer op_targetArray = popOperand(context);
            HeapPointer op_sourceOffset = popOperand(context);
            HeapPointer op_sourceArray = popOperand(context);

            assertObjectType_isAnyArray(op_sourceArray);
            assertObjectType_isAnyArray(op_targetArray);
            if (op_sourceArray->objectType != op_targetArray->objectType) {
                printf("arraycopy array type mismatch %d vs %d", op_sourceArray->objectType,
                       op_targetArray->objectType);
                exit(-1);
            }

            U4 sourceOffset = primitiveInt_getValue(op_sourceOffset);
            U4 targetOffset = primitiveInt_getValue(op_targetOffset);
            U4 copyLength = primitiveInt_getValue(op_length);

            if (op_sourceArray->objectType == OBJ_ReferenceArray) {
                for (int i = 0; i < copyLength; ++i) {
                    HeapPointer op = (HeapPointer) op_sourceArray->data[i + sourceOffset];
                    refArray_setAtIndex(op_targetArray, i + targetOffset, op);
                    incrementReferenceCount(op);
                }
            } else {
                for (int i = 0; i < copyLength; ++i) {
                    op_targetArray->data[i + targetOffset] = op_sourceArray->data[i + sourceOffset];
                }
            }

            decrementReferenceCount(op_length);
            decrementReferenceCount(op_targetOffset);
            decrementReferenceCount(op_targetArray);
            decrementReferenceCount(op_sourceOffset);
            decrementReferenceCount(op_sourceArray);
            return true;
        }
    } else if (cp_cString_utf8_equals("java/lang/Class", classNameUtf8)) {
        if (cp_cString_utf8_equals("getPrimitiveClass", methodNameUtf8)
            && cp_cString_utf8_equals("(Ljava/lang/String;)Ljava/lang/Class;", methodDescriptorUtf8)) {

            decrementReferenceCount(popOperand(context));
            op_aConst_null(context);
            return true;
        }
    }
    return false;
}

void pushFrameLocal(ExecutionContext *context, ExecutionFrame *frame, U2 index) {
    HeapPointer operand = frame->locals[index];
    incrementReferenceCount(operand);
    pushOperand(context, operand);
}

void storeFrameLocal(ExecutionContext *context, ExecutionFrame *frame, U2 index) {
    decrementReferenceCount(frame->locals[index]);
    frame->locals[index] = popOperand(context);
}

bool executeNextInstruction(ExecutionContext *context, ExecutionFrame *frame) {
    Instruction instruction = frame->codeAttribute->instructions[frame->programCounter];
    frame->programCounter++;

#ifdef PRINT_OPCODES
    for (int i = 0; i < instruction.numBytes; ++i) {
        printf("%02X ", instruction.bytes[i]);
    }
    printf(" | operands: %lu\n", context->currentOperandStackDepth);
#endif

    switch (instruction.bytes[0]) {
        case 0x01:
            op_aConst_null(context);
            break;
        case 0x59:
            op_dup(context);
            break;
        case 0x5a:
            op_dup_x1(context);
            break;
        case 0x5c:
            op_dup2(context);
            break;
        case 0xb7:
            op_invokeSpecial(context, frame, instruction);
            break;
        case 0xb6:
            op_invokeVirtual(context, frame, instruction);
            break;
        case 0xb8:
            op_invokeStatic(context, frame, instruction);
            break;
        case 0xbb:
            op_new(context, frame, instruction);
            break;
        case 0xbc:
            op_newArray(context);
            break;
        case 0xbd:
            op_aNewArray(context);
            break;
        case 0xbe:
            op_arrayLength(context);
            break;
        case 0x19:
            pushFrameLocal(context, frame, instruction.bytes[1]);
            break;
        case 0x2a:
            pushFrameLocal(context, frame, 0);
            break;
        case 0x2b:
            pushFrameLocal(context, frame, 1);
            break;
        case 0x2c:
            pushFrameLocal(context, frame, 2);
            break;
        case 0x2d:
            pushFrameLocal(context, frame, 3);
            break;
        case 0x02:
            pushOperand(context, createPrimitive_int(-1));
            break;
        case 0x03:
            pushOperand(context, createPrimitive_int(0));
            break;
        case 0x04:
            pushOperand(context, createPrimitive_int(1));
            break;
        case 0x05:
            pushOperand(context, createPrimitive_int(2));
            break;
        case 0x06:
            pushOperand(context, createPrimitive_int(3));
            break;
        case 0x07:
            pushOperand(context, createPrimitive_int(4));
            break;
        case 0x08:
            pushOperand(context, createPrimitive_int(5));
            break;
        case 0x33:
            op_baLoad(context);
            break;
        case 0x34:
            op_caLoad(context);
            break;
        case 0x2e:
            op_iaLoad(context);
            break;
        case 0x78:
            op_iShL(context);
            break;
        case 0x7c:
            op_iuShR(context);
            break;
        case 0x10:
            pushOperand(context, createPrimitive_int(instruction.bytes[1]));
            break;
        case 0x11:
            pushOperand(context, createPrimitive_int(instructionGetShort(instruction, 1)));
            break;
        case 0x80:
            op_iOr(context);
            break;
        case 0x36: // op_iStore <byte>
            storeFrameLocal(context, frame, instruction.bytes[1]);
            break;
        case 0x3b:
            storeFrameLocal(context, frame, 0);
            break;
        case 0x3c:
            storeFrameLocal(context, frame, 1);
            break;
        case 0x3d:
            storeFrameLocal(context, frame, 2);
            break;
        case 0x3e:
            storeFrameLocal(context, frame, 3);
            break;
        case 0x91:
            op_i2b();
            break;
        case 0x92:
            op_i2c();
            break;
        case 0x15:
            pushFrameLocal(context, frame, instruction.bytes[1]);
            break;
        case 0x1a:
            pushFrameLocal(context, frame, 0);
            break;
        case 0x1b:
            pushFrameLocal(context, frame, 1);
            break;
        case 0x1c:
            pushFrameLocal(context, frame, 2);
            break;
        case 0x1d:
            pushFrameLocal(context, frame, 3);
            break;
        case 0xb4:
            op_getField(context, frame, instruction);
            break;
        case 0xb5:
            op_putField(context, frame, instruction);
            break;
        case 0x3a:
            storeFrameLocal(context, frame, instruction.bytes[1]);
            break;
        case 0x4b:
            storeFrameLocal(context, frame, 0);
            break;
        case 0x4c:
            storeFrameLocal(context, frame, 1);
            break;
        case 0x4d:
            storeFrameLocal(context, frame, 2);
            break;
        case 0x4e:
            storeFrameLocal(context, frame, 3);
            break;
        case 0x9f:
            op_if_iCmp_eq(context, frame, instruction);
            break;
        case 0xa0:
            op_if_iCmp_ne(context, frame, instruction);
            break;
        case 0xa1:
            op_if_iCmp_lt(context, frame, instruction);
            break;
        case 0xa2:
            op_if_iCmp_ge(context, frame, instruction);
            break;
        case 0xa3:
            op_if_iCmp_gt(context, frame, instruction);
            break;
        case 0xa4:
            op_if_iCmp_le(context, frame, instruction);
            break;
        case 0xc7:
            op_ifNonNull(context, frame, instruction);
            break;
        case 0x99:
            op_ifEq(context, frame, instruction);
            break;
        case 0x9a:
            op_ifNE(context, frame, instruction);
            break;
        case 0x9b:
            op_ifLT(context, frame, instruction);
            break;
        case 0x9c:
            op_ifGE(context, frame, instruction);
            break;
        case 0x9d:
            op_ifGT(context, frame, instruction);
            break;
        case 0x9e:
            op_ifLE(context, frame, instruction);
            break;
        case 0xa7:
            branch(frame, instructionGetShort(instruction, 1));
            break;
        case 0x60:
            op_iAdd(context);
            break;
        case 0x64:
            op_iSub(context);
            break;
        case 0x68:
            op_iMul(context);
            break;
        case 0x7e:
            op_iAnd(context);
            break;
        case 0x4f:
            op_iaStore(context);
            break;
        case 0x54:
            op_baStore(context);
            break;
        case 0x55:
            op_caStore(context);
            break;
        case 0x84:
            op_iInc(frame, instruction);
            break;
        case 0xb2:
            op_getStatic(context, frame, instruction);
            break;
        case 0xb3:
            op_putStatic(context, frame, instruction);
            break;
        case 0xb0:
        case 0xac:
            if (context->currentOperandStackDepth != (frame->initialOperandStackDepth + 1)) {
                printf("unexpected stack depth! is %lu, expected %lu\n", context->currentOperandStackDepth,
                       frame->initialOperandStackDepth + 1);
            }
            return false;
        case 0xb1:
            if (context->currentOperandStackDepth != frame->initialOperandStackDepth) {
                printf("unexpected stack depth! is %lu, expected %lu\n", context->currentOperandStackDepth,
                       frame->initialOperandStackDepth);
            }
            return false;
        case 0x57:
            op_pop(context);
            break;
        case 0x58:
            op_pop2(context);
            break;
        case 0x12:
            op_ldc(context, frame, instruction);
            break;
        default:
            printf("opCode '%d' is not yet supported\n", instruction.bytes[0]);
            exit(-1);
    }
    return true;
}

bool execute(ExecutionContext *context) {
    if (!executeNextInstruction(context, &context->executionFrameStack[context->currentFrameStackDepth - 1])) {
        popExecutionFrame(context);
        return context->currentFrameStackDepth > 0;
    }
    return true;
}
