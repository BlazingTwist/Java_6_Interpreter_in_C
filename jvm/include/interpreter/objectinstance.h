#pragma once

#include "../definition/integral_types.h"
#include "../definition/classfile.h"

#pragma region ObjectType_Identifiers
#define OBJ_int 0
#define OBJ_long 1
#define OBJ_float 2
#define OBJ_double 3
#define OBJ_ObjectRef 4
#define OBJ_PrimitiveArray 5
#define OBJ_ReferenceArray 6
#pragma endregion

#define HeapPointer ObjectInstance*

typedef struct ObjectInstance {
    U1 activeReferences;
    U1 objectType;
    U4 numDataBytes;
    UPtr *data;
} ObjectInstance;

HeapPointer createPrimitive_int(U4 value);

HeapPointer createPrimitive_float(float value);

HeapPointer createObjectRef(const ClassFile *classFile, U4 fieldCount);

HeapPointer createPrimitiveArray_4Byte(U4 numValues);

HeapPointer createPrimitiveArray_byteArr(U4 numValues, const U1 *values);

HeapPointer createReferenceArray(U4 numValues);

void incrementReferenceCount(HeapPointer heapPointer);

void decrementReferenceCount(HeapPointer heapPointer);

void object_deallocate(HeapPointer heapPointer);

U4 primitiveInt_getValue(HeapPointer heapPointer);

float primitiveFloat_getValue(HeapPointer heapPointer);

U4 objectRef_getClassLoaderFileIndex(HeapPointer heapPointer);

HeapPointer objectRef_getFieldValue(HeapPointer thisInstance, int fieldIndex);

void objectRef_putFieldValue(HeapPointer container, int fieldIndex, HeapPointer value);

void refArray_setAtIndex(HeapPointer array, int index, HeapPointer value);

char *primitiveArray_toCString(const HeapPointer heapPointer);

void assertObjectType(U1 expectedType, const HeapPointer heapPointer);

void assertObjectType_isAnyArray(const HeapPointer heapPointer);
