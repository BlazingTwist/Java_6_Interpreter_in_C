#include "../../include/config.h"
#include "../../include/interpreter/opcodes.h"
#include "../../include/interpreter/instructionhelper.h"
#include "../../include/definition/accessflags.h"
#include "../../include/util/allocator.h"
#include <stdio.h>
#include <stdlib.h>

void op_aConst_null(ExecutionContext *context) {
    pushOperand(context, NULL);
}

void op_new(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    U2 index = instructionGetShort(instruction, 1);
    const CP_Info *classNameUtf8 = cp_class_resolveClassName(&frame->thisClassFile->constPoolEntries[index], frame->thisClassFile);

    const ClassFile *class = getClassFile_byName_utf8(context->classLoader, classNameUtf8);
    ensureStaticInstance(context, class);
    pushOperand(context, createObjectRef(class, getFieldCount(context, class)));
}

void op_newArray(ExecutionContext *context) {
    HeapPointer arrSizeOperand = popOperand(context);
    U4 arrayLength = primitiveInt_getValue(arrSizeOperand);
    pushOperand(context, createPrimitiveArray_4Byte(arrayLength));
    decrementReferenceCount(arrSizeOperand);
}

void op_aNewArray(ExecutionContext *context) {
    HeapPointer arrSizeOperand = popOperand(context);
    U4 arrayLength = primitiveInt_getValue(arrSizeOperand);
    pushOperand(context, createReferenceArray(arrayLength));
    decrementReferenceCount(arrSizeOperand);
}

void op_arrayLength(ExecutionContext *context) {
    HeapPointer operand = popOperand(context);
    pushOperand(context, createPrimitive_int(operand->numDataBytes));
    decrementReferenceCount(operand);
}

void op_ldc(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    U1 index = instruction.bytes[1];
    const CP_Info *constValue = &frame->thisClassFile->constPoolEntries[index];
    switch (constValue->tag) {
        case CP_String: {
            U2 utf8Index = cp_string_getStringIndex(constValue);
            pushOperand(context, createObjectRef_string(context, &frame->thisClassFile->constPoolEntries[utf8Index]));
            break;
        }
        case CP_Integer: {
            I4 iValue = cp_integer_getValue(constValue);
            pushOperand(context, createPrimitive_int(iValue));
            break;
        }
        case CP_Float: {
            float fValue = cp_float_getValue(constValue);
            pushOperand(context, createPrimitive_float(fValue));
            break;
        }
        default:
            printf("unsupported tag: %d\n", constValue->tag);
            exit(-1);
    }
}

void op_dup(ExecutionContext *context) {
    HeapPointer operand = context->operandStack[context->currentOperandStackDepth - 1];
    incrementReferenceCount(operand);
    pushOperand(context, operand);
}

void op_dup_x1(ExecutionContext *context) {
    HeapPointer topOperand = popOperand(context);
    HeapPointer secondOperand = popOperand(context);
    incrementReferenceCount(topOperand);
    pushOperand(context, topOperand);
    pushOperand(context, secondOperand);
    pushOperand(context, topOperand);
}

void op_dup2(ExecutionContext *context) {
    HeapPointer topOperand = popOperand(context);
    if (topOperand->objectType != OBJ_long && topOperand->objectType != OBJ_double) {
        HeapPointer secondOperand = popOperand(context);
        incrementReferenceCount(topOperand);
        incrementReferenceCount(secondOperand);
        pushOperand(context, secondOperand);
        pushOperand(context, topOperand);
        pushOperand(context, secondOperand);
        pushOperand(context, topOperand);
    } else {
        incrementReferenceCount(topOperand);
        pushOperand(context, topOperand);
        pushOperand(context, topOperand);
    }
}

void op_invokeSpecial(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    const CP_Info *methodRef = &frame->thisClassFile->constPoolEntries[instructionGetShort(instruction, 1)];
    const ClassFile *targetClass = cp_methodRef_resolveClassFile(methodRef, context->classLoader, frame->thisClassFile);
    const MethodInfo *interfaceMethodInfo = cp_methodRef_resolveMethod(methodRef, targetClass, frame->thisClassFile);
    pushMethod(context, targetClass, interfaceMethodInfo);
}

void op_invokeVirtual(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    const CP_Info *methodRef = &frame->thisClassFile->constPoolEntries[instructionGetShort(instruction, 1)];
    const ClassFile *interfaceClass = cp_methodRef_resolveClassFile(methodRef, context->classLoader, frame->thisClassFile);
    const MethodInfo *interfaceMethod = cp_methodRef_resolveMethod(methodRef, interfaceClass, frame->thisClassFile);

    HeapPointer opInstance = context->operandStack[context->currentOperandStackDepth - interfaceMethod->numParameters];
    const ClassFile *instanceClass = getClassFile_byIndex(context->classLoader, objectRef_getClassLoaderFileIndex(opInstance));

    pushVirtualMethod(context, instanceClass, instanceClass,
                      &interfaceClass->constPoolEntries[interfaceMethod->nameIndex],
                      &interfaceClass->constPoolEntries[interfaceMethod->descriptorIndex]);
}

void op_invokeStatic(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    const CP_Info *methodRef = &frame->thisClassFile->constPoolEntries[instructionGetShort(instruction, 1)];
    const ClassFile *targetClass = cp_methodRef_resolveClassFile(methodRef, context->classLoader, frame->thisClassFile);
    const MethodInfo *method = cp_methodRef_resolveMethod(methodRef, targetClass, frame->thisClassFile);

#ifdef PRINT_INVOKE_STATIC
    U1* nameString = cp_utf8_getCString(&targetClass->constPoolEntries[method->nameIndex]);
    printf("invoke static method name: %s\n", nameString);
    deAllocateBytes(nameString);
#endif

    if ((method->accessFlags & ACC_STATIC) != ACC_STATIC) {
        printf("invokeStatic target method is not static!\n");
        exit(-1);
    }

    pushMethod(context, targetClass, method);
}

void op_baLoad(ExecutionContext *context) {
    HeapPointer indexOperand = popOperand(context);
    U4 index = primitiveInt_getValue(indexOperand);
    HeapPointer arrayRef = popOperand(context);
    assertObjectType(OBJ_PrimitiveArray, arrayRef);
    pushOperand(context, createPrimitive_int(arrayRef->data[index]));

    decrementReferenceCount(indexOperand);
    decrementReferenceCount(arrayRef);
}

void op_caLoad(ExecutionContext *context) {
    U4 index = popIntOperand(context);
    HeapPointer arrayRef = popOperand(context);
    assertObjectType(OBJ_PrimitiveArray, arrayRef);
    pushOperand(context, createPrimitive_int(arrayRef->data[index]));

    decrementReferenceCount(arrayRef);
}

void op_iaLoad(ExecutionContext *context) {
    HeapPointer indexOperand = popOperand(context);
    U4 index = primitiveInt_getValue(indexOperand);
    HeapPointer arrayRef = popOperand(context);
    assertObjectType(OBJ_PrimitiveArray, arrayRef);
    pushOperand(context, createPrimitive_int(arrayRef->data[index]));

    decrementReferenceCount(indexOperand);
    decrementReferenceCount(arrayRef);
}

void op_iShL(ExecutionContext *context) {
    HeapPointer op2 = popOperand(context);
    HeapPointer op1 = popOperand(context);
    U4 value2 = primitiveInt_getValue(op2) & 0b11111;
    U4 value1 = primitiveInt_getValue(op1);
    pushOperand(context, createPrimitive_int(value1 << value2));

    decrementReferenceCount(op2);
    decrementReferenceCount(op1);
}

void op_iuShR(ExecutionContext *context) {
    HeapPointer op2 = popOperand(context);
    HeapPointer op1 = popOperand(context);
    U4 value2 = primitiveInt_getValue(op2) & 0b11111;
    U4 value1 = primitiveInt_getValue(op1);
    pushOperand(context, createPrimitive_int(value1 >> value2));

    decrementReferenceCount(op2);
    decrementReferenceCount(op1);
}

void op_iOr(ExecutionContext *context) {
    HeapPointer op2 = popOperand(context);
    HeapPointer op1 = popOperand(context);
    U4 value2 = primitiveInt_getValue(op2);
    U4 value1 = primitiveInt_getValue(op1);
    pushOperand(context, createPrimitive_int(value1 | value2));

    decrementReferenceCount(op2);
    decrementReferenceCount(op1);
}

void op_i2b() {
    // do nothing
}

void op_i2c() {
    // do nothing
}

void op_iAdd(ExecutionContext *context) {
    U4 value2 = popIntOperand(context);
    U4 value1 = popIntOperand(context);
    pushOperand(context, createPrimitive_int(value1 + value2));
}

void op_iSub(ExecutionContext *context) {
    U4 value2 = popIntOperand(context);
    U4 value1 = popIntOperand(context);
    pushOperand(context, createPrimitive_int(value1 - value2));
}

void op_iMul(ExecutionContext *context) {
    U4 value2 = popIntOperand(context);
    U4 value1 = popIntOperand(context);
    pushOperand(context, createPrimitive_int(value1 * value2));
}

void op_iAnd(ExecutionContext *context) {
    U4 value2 = popIntOperand(context);
    U4 value1 = popIntOperand(context);
    pushOperand(context, createPrimitive_int(value1 & value2));
}

void op_getField(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    U2 index = instructionGetShort(instruction, 1);
    const CP_Info *fieldRef = &frame->thisClassFile->constPoolEntries[index];
    const CP_Info *classNameUtf8 = cp_fieldRef_resolveClassNameUtf8(fieldRef, frame->thisClassFile);
    const CP_Info *fieldNameUtf8 = cp_fieldRef_resolveFieldNameUtf8(fieldRef, frame->thisClassFile);

    const ClassFile *class = getClassFile_byName_utf8(context->classLoader, classNameUtf8);
    U4 fieldIndex = findFieldIndexInHierarchy_utf8(context, class, fieldNameUtf8);
    HeapPointer instance = popOperand(context);
    HeapPointer fieldValue = objectRef_getFieldValue(instance, fieldIndex);
    pushOperand(context, fieldValue);
    incrementReferenceCount(fieldValue);
    decrementReferenceCount(instance);
}

void op_putField(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    U2 index = instructionGetShort(instruction, 1);
    const CP_Info *fieldRef = &frame->thisClassFile->constPoolEntries[index];
    const CP_Info *classNameUtf8 = cp_fieldRef_resolveClassNameUtf8(fieldRef, frame->thisClassFile);
    const CP_Info *fieldNameUtf8 = cp_fieldRef_resolveFieldNameUtf8(fieldRef, frame->thisClassFile);

    const ClassFile *class = getClassFile_byName_utf8(context->classLoader, classNameUtf8);
    U4 fieldIndex = findFieldIndexInHierarchy_utf8(context, class, fieldNameUtf8);
    HeapPointer fieldValue = popOperand(context);
    HeapPointer instance = popOperand(context);
    objectRef_putFieldValue(instance, fieldIndex, fieldValue);
    // field value reference is transferred to obj instance, no change
    decrementReferenceCount(instance);

#ifdef PRINT_PUT_FIELD
    U1* classNameString = cp_utf8_getCString(classNameUtf8);
    U1* nameString = cp_utf8_getCString(fieldNameUtf8);
    printf("putField: %s . %s\n", classNameString, nameString);
    deAllocateBytes(nameString);
    deAllocateBytes(classNameString);
#endif
}

void op_getStatic(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    U2 index = instructionGetShort(instruction, 1);
    const CP_Info *fieldRef = &frame->thisClassFile->constPoolEntries[index];
    const CP_Info *classNameUtf8 = cp_fieldRef_resolveClassNameUtf8(fieldRef, frame->thisClassFile);
    const CP_Info *fieldNameUtf8 = cp_fieldRef_resolveFieldNameUtf8(fieldRef, frame->thisClassFile);

    const ClassFile *class = getClassFile_byName_utf8(context->classLoader, classNameUtf8);
    ensureStaticInstance(context, class);
    U4 fieldIndex = findFieldIndexInHierarchy_utf8(context, class, fieldNameUtf8);

    HeapPointer staticInstance = context->staticInstances[class->classLoaderIndex];
    HeapPointer fieldValue = objectRef_getFieldValue(staticInstance, fieldIndex);
    pushOperand(context, fieldValue);
    incrementReferenceCount(fieldValue);
}

void op_putStatic(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    U2 index = instructionGetShort(instruction, 1);
    const CP_Info *fieldRef = &frame->thisClassFile->constPoolEntries[index];
    const CP_Info *classNameUtf8 = cp_fieldRef_resolveClassNameUtf8(fieldRef, frame->thisClassFile);
    const CP_Info *fieldNameUtf8 = cp_fieldRef_resolveFieldNameUtf8(fieldRef, frame->thisClassFile);

    const ClassFile *class = getClassFile_byName_utf8(context->classLoader, classNameUtf8);
    ensureStaticInstance(context, class);
    U4 fieldIndex = findFieldIndexInHierarchy_utf8(context, class, fieldNameUtf8);

    HeapPointer staticInstance = context->staticInstances[class->classLoaderIndex];
    HeapPointer valueOperand = popOperand(context);
    objectRef_putFieldValue(staticInstance, fieldIndex, valueOperand);
}

void op_iaStore(ExecutionContext *context) {
    U4 value = popIntOperand(context);
    U4 index = popIntOperand(context);
    HeapPointer array = popOperand(context);

    assertObjectType(OBJ_PrimitiveArray, array);
    array->data[index] = value;
    decrementReferenceCount(array);
}

void op_baStore(ExecutionContext *context) {
    U4 value = popIntOperand(context);
    U4 index = popIntOperand(context);
    HeapPointer array = popOperand(context);

    assertObjectType(OBJ_PrimitiveArray, array);
    array->data[index] = value;
    decrementReferenceCount(array);
}

void op_caStore(ExecutionContext *context) {
    U4 value = popIntOperand(context);
    U4 index = popIntOperand(context);
    HeapPointer array = popOperand(context);

    assertObjectType(OBJ_PrimitiveArray, array);
    array->data[index] = value;
    decrementReferenceCount(array);
}

void branch(ExecutionFrame *frame, U2 branchOffset) {
    I2 signedOffset = *(I2 *) &branchOffset;
    frame->programCounter--;// revert default increment
    if (signedOffset == 0) {
        return;
    }
    if (signedOffset < 0) {
        while (signedOffset < 0) {
            frame->programCounter--;
            U1 bytesUntilPrevOpcode = frame->codeAttribute->instructions[frame->programCounter].numBytes;
            signedOffset += bytesUntilPrevOpcode;
        }
    } else {
        while (signedOffset > 0) {
            U1 bytesUntilNextOpcode = frame->codeAttribute->instructions[frame->programCounter].numBytes;
            frame->programCounter++;
            signedOffset -= bytesUntilNextOpcode;
        }
    }
    if (signedOffset != 0) {
        printf("branch does not point directly at opcode (that's illegal!)\n");
        exit(-1);
    }
}

void op_if_iCmp_eq(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    I4 value2 = (I4)popIntOperand(context);
    I4 value1 = (I4)popIntOperand(context);
    if (value1 == value2) {
        branch(frame, instructionGetShort(instruction, 1));
    }
}

void op_if_iCmp_ne(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    I4 value2 = (I4)popIntOperand(context);
    I4 value1 = (I4)popIntOperand(context);
    if (value1 != value2) {
        branch(frame, instructionGetShort(instruction, 1));
    }
}

void op_if_iCmp_lt(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    I4 value2 = (I4)popIntOperand(context);
    I4 value1 = (I4)popIntOperand(context);
    if (value1 < value2) {
        branch(frame, instructionGetShort(instruction, 1));
    }
}

void op_if_iCmp_le(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    I4 value2 = (I4)popIntOperand(context);
    I4 value1 = (I4)popIntOperand(context);
    if (value1 <= value2) {
        branch(frame, instructionGetShort(instruction, 1));
    }
}

void op_if_iCmp_gt(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    I4 value2 = (I4)popIntOperand(context);
    I4 value1 = (I4)popIntOperand(context);
    if (value1 > value2) {
        branch(frame, instructionGetShort(instruction, 1));
    }
}

void op_if_iCmp_ge(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    I4 value2 = (I4)popIntOperand(context);
    I4 value1 = (I4)popIntOperand(context);
    if (value1 >= value2) {
        branch(frame, instructionGetShort(instruction, 1));
    }
}

void op_ifNonNull(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    HeapPointer op = popOperand(context);
    if (op != NULL) {
        branch(frame, instructionGetShort(instruction, 1));
    }
    decrementReferenceCount(op);
}

void op_ifEq(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    I4 value = (I4)popIntOperand(context);
    if (value == 0) {
        branch(frame, instructionGetShort(instruction, 1));
    }
}

void op_ifNE(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    I4 value = (I4)popIntOperand(context);
    if (value != 0) {
        branch(frame, instructionGetShort(instruction, 1));
    }
}

void op_ifLT(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    I4 value = (I4)popIntOperand(context);
    if (value < 0) {
        branch(frame, instructionGetShort(instruction, 1));
    }
}

void op_ifGE(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    I4 value = (I4)popIntOperand(context);
    if (value >= 0) {
        branch(frame, instructionGetShort(instruction, 1));
    }
}

void op_ifGT(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    I4 value = (I4)popIntOperand(context);
    if (value > 0) {
        branch(frame, instructionGetShort(instruction, 1));
    }
}

void op_ifLE(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction) {
    I4 value = (I4)popIntOperand(context);
    if (value <= 0) {
        branch(frame, instructionGetShort(instruction, 1));
    }
}

void op_iInc(ExecutionFrame *frame, Instruction instruction) {
    U1 index = instruction.bytes[1];
    I1 increment = (I1) instruction.bytes[2];
    HeapPointer operand = frame->locals[index];
    I1 current = (I1) primitiveInt_getValue(operand);
    decrementReferenceCount(operand);

    U4 result = (U4) (current + increment);
    frame->locals[index] = createPrimitive_int(result);
}

void op_pop(ExecutionContext *context) {
    decrementReferenceCount(popOperand(context));
}

void op_pop2(ExecutionContext *context) {
    HeapPointer operand = popOperand(context);
    if (operand->objectType != OBJ_long && operand->objectType != OBJ_double) {
        decrementReferenceCount(popOperand(context));
    }
    decrementReferenceCount(operand);
}
