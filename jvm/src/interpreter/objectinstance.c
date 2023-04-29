#include "../../include/config.h"
#include "../../include/interpreter/objectinstance.h"
#include "../../include/util/allocator.h"
#include <stdio.h>

HeapPointer createPrimitive_int(U4 value) {
    HeapPointer result = allocateBytes(sizeof(ObjectInstance));
#ifdef PRINT_MEM_STATS
    numHeapPointersCreated++;
#endif
    result->activeReferences = 1;
    result->objectType = OBJ_int;
    result->numDataBytes = value; // jank af bro
    result->data = NULL;
    return result;
}

HeapPointer createPrimitive_float(float value) {
    HeapPointer result = allocateBytes(sizeof(ObjectInstance));
#ifdef PRINT_MEM_STATS
    numHeapPointersCreated++;
#endif
    result->activeReferences = 1;
    result->objectType = OBJ_float;
    result->numDataBytes = *(U4 *) &value; // jank af bro
    result->data = NULL;
    return result;
}

HeapPointer createObjectRef(const ClassFile *classFile, U4 fieldCount) {
    HeapPointer result = allocateBytes(sizeof(ObjectInstance));
#ifdef PRINT_MEM_STATS
    numHeapPointersCreated++;
#endif
    result->activeReferences = 1;
    result->objectType = OBJ_ObjectRef;
    result->numDataBytes = fieldCount + 1;
    result->data = allocateBytes(sizeof(UPtr) * result->numDataBytes);
    result->data[0] = classFile->classLoaderIndex;
    for (int i = 1; i < result->numDataBytes; i++) {
        result->data[i] = 0;
    }
    return result;
}

HeapPointer createPrimitiveArray_4Byte(U4 numValues) {
    HeapPointer result = allocateBytes(sizeof(ObjectInstance));
#ifdef PRINT_MEM_STATS
    numHeapPointersCreated++;
#endif
    result->activeReferences = 1;
    result->objectType = OBJ_PrimitiveArray;
    result->numDataBytes = numValues;
    if (numValues > 0) {
        result->data = allocateBytes(sizeof(UPtr) * numValues);
        for (int i = 0; i < numValues; ++i) {
            result->data[i] = 0;
        }
    } else {
        result->data = NULL;
    }
    return result;
}

HeapPointer createPrimitiveArray_byteArr(U4 numValues, const U1 *values) {
    HeapPointer result = allocateBytes(sizeof(ObjectInstance));
#ifdef PRINT_MEM_STATS
    numHeapPointersCreated++;
#endif
    result->activeReferences = 1;
    result->objectType = OBJ_PrimitiveArray;
    result->numDataBytes = numValues;

    if (numValues > 0) {
        result->data = allocateBytes(sizeof(UPtr) * numValues);
        for (int i = 0; i < numValues; ++i) {
            result->data[i] = values[i];
        }
    } else {
        result->data = NULL;
    }
    return result;
}

HeapPointer createReferenceArray(U4 numValues) {
    HeapPointer result = allocateBytes(sizeof(ObjectInstance));
#ifdef PRINT_MEM_STATS
    numHeapPointersCreated++;
#endif
    result->activeReferences = 1;
    result->objectType = OBJ_ReferenceArray;
    result->numDataBytes = numValues;

    if (numValues > 0) {
        result->data = allocateBytes(sizeof(UPtr) * numValues);
        for (int i = 0; i < numValues; ++i) {
            result->data[i] = 0;
        }
    } else {
        result->data = NULL;
    }
    return result;
}

void incrementReferenceCount(HeapPointer heapPointer) {
    if (heapPointer == NULL) {
        return;
    }
    if (heapPointer->activeReferences >= 0xff) {
        printf("number of references on object exceeded maximum\n");
        exit(-1);
    }
    heapPointer->activeReferences++;
}

void decrementReferenceCount(HeapPointer heapPointer) {
    if (heapPointer == NULL) {
        return;
    }
    if (heapPointer->activeReferences == 0) {
        printf("tried to deallocate dead object!");
        exit(-1);
    }
    heapPointer->activeReferences--;
    if (heapPointer->activeReferences <= 0) {
        object_deallocate(heapPointer);
    }
}

void object_deallocate(HeapPointer heapPointer) {
    if (heapPointer == NULL) {
        return;
    }
#ifdef PRINT_MEM_STATS
    numHeapPointersReleased++;
#endif

    switch (heapPointer->objectType) {
        case OBJ_ReferenceArray:
            for (int i = 0; i < heapPointer->numDataBytes; ++i) {
                decrementReferenceCount((HeapPointer) heapPointer->data[i]);
            }
            break;
        case OBJ_ObjectRef:
            for (int i = 1; i < heapPointer->numDataBytes; ++i) {
                decrementReferenceCount((HeapPointer) heapPointer->data[i]);
            }
            break;
    }

    if (heapPointer->data != NULL) {
        deAllocateBytes(heapPointer->data);
        heapPointer->data = NULL;
    }
    deAllocateBytes(heapPointer);
}

U4 primitiveInt_getValue(HeapPointer heapPointer) {
    if (heapPointer == NULL) {
        return 0;
    }
    assertObjectType(OBJ_int, heapPointer);
    return heapPointer->numDataBytes;
}

float primitiveFloat_getValue(HeapPointer heapPointer) {
    if (heapPointer == NULL) {
        return 0;
    }
    assertObjectType(OBJ_float, heapPointer);
    U4 value = heapPointer->numDataBytes;
    return *(float *) &value;
}

U4 objectRef_getClassLoaderFileIndex(HeapPointer heapPointer) {
    assertObjectType(OBJ_ObjectRef, heapPointer);
    return heapPointer->data[0];
}

HeapPointer objectRef_getFieldValue(HeapPointer thisInstance, int fieldIndex) {
    HeapPointer fieldPointer = (HeapPointer) thisInstance->data[fieldIndex + 1];
    return fieldPointer;
}

void objectRef_putFieldValue(HeapPointer container, int fieldIndex, HeapPointer value) {
    HeapPointer oldOperand = (HeapPointer) container->data[fieldIndex + 1];
    decrementReferenceCount(oldOperand);
    container->data[fieldIndex + 1] = (UPtr) value;
}

void refArray_setAtIndex(HeapPointer array, int index, HeapPointer value) {
    decrementReferenceCount((HeapPointer) array->data[index]);
    array->data[index] = (UPtr) value;
}

char *primitiveArray_toCString(const HeapPointer heapPointer) {
    assertObjectType(OBJ_PrimitiveArray, heapPointer);
    char *cString = allocateBytes(heapPointer->numDataBytes + 1);
    for (int i = 0; i < heapPointer->numDataBytes; ++i) {
        cString[i] = (char) heapPointer->data[i];
    }
    cString[heapPointer->numDataBytes] = '\0';
    return cString;
}

void assertObjectType(U1 expectedType, const HeapPointer heapPointer) {
    if (heapPointer->objectType != expectedType) {
        printf("expected objectType: %d, but got '%d\n", expectedType, heapPointer->objectType);
        exit(-1);
    }
}

void assertObjectType_isAnyArray(const HeapPointer heapPointer) {
    if (heapPointer->objectType != OBJ_PrimitiveArray && heapPointer->objectType != OBJ_ReferenceArray) {
        printf("expected objectType: primitiveArray or referenceArray, but got '%d\n", heapPointer->objectType);
        exit(-1);
    }
}