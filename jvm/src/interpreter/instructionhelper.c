#include "../../include/interpreter/instructionhelper.h"
#include "../../include/util/allocator.h"
#include <stdio.h>
#include <stdlib.h>

U1 getInstructionByteSize(U1 opCode) {
    switch (opCode) {
        case 0x32:
        case 0x53:
        case 0x5f:
        case 0x56:
        case 0x35:
        case 0xb1:
        case 0x58:
        case 0x57:
        case 0x00:
        case 0xc3:
        case 0xc2:
        case 0x83:
        case 0x7d:
        case 0x65:
        case 0x42:
        case 0x41:
        case 0x40:
        case 0x3f:
        case 0x7b:
        case 0x79:
        case 0xad:
        case 0x71:
        case 0x81:
        case 0x75:
        case 0x69:
        case 0x21:
        case 0x20:
        case 0x1f:
        case 0x1e:
        case 0x16:
        case 0x6d:
        case 0x0a:
        case 0x09:
        case 0x94:
        case 0x50:
        case 0x7f:
        case 0x2f:
        case 0x61:
        case 0x88:
        case 0x89:
        case 0x8a:
        case 0x82:
        case 0x7c:
        case 0x64:
        case 0x3e:
        case 0x3d:
        case 0x3c:
        case 0x3b:
        case 0x7a:
        case 0x78:
        case 0xac:
        case 0x70:
        case 0x80:
        case 0x74:
        case 0x68:
        case 0x1d:
        case 0x1c:
        case 0x1b:
        case 0x1a:
        case 0x6c:
        case 0x08:
        case 0x07:
        case 0x06:
        case 0x05:
        case 0x04:
        case 0x03:
        case 0x02:
        case 0x4f:
        case 0x7e:
        case 0x2e:
        case 0x60:
        case 0x93:
        case 0x85:
        case 0x86:
        case 0x87:
        case 0x92:
        case 0x91:
        case 0x66:
        case 0x46:
        case 0x45:
        case 0x44:
        case 0x43:
        case 0xae:
        case 0x72:
        case 0x76:
        case 0x6a:
        case 0x25:
        case 0x24:
        case 0x23:
        case 0x22:
        case 0x6e:
        case 0x0d:
        case 0x0c:
        case 0x0b:
        case 0x96:
        case 0x95:
        case 0x51:
        case 0x30:
        case 0x62:
        case 0x8c:
        case 0x8b:
        case 0x8d:
        case 0x5e:
        case 0x5d:
        case 0x5c:
        case 0x5b:
        case 0x5a:
        case 0x59:
        case 0x67:
        case 0x4a:
        case 0x49:
        case 0x48:
        case 0x47:
        case 0xaf:
        case 0x73:
        case 0x77:
        case 0x6b:
        case 0x29:
        case 0x28:
        case 0x27:
        case 0x26:
        case 0x6f:
        case 0x0f:
        case 0x0e:
        case 0x98:
        case 0x97:
        case 0x52:
        case 0x31:
        case 0x63:
        case 0x8f:
        case 0x8e:
        case 0x90:
        case 0x55:
        case 0x34:
        case 0x54:
        case 0x33:
        case 0xbf:
        case 0x4e:
        case 0x4d:
        case 0x4c:
        case 0x4b:
        case 0xbe:
        case 0xb0:
        case 0x2d:
        case 0x2c:
        case 0x2b:
        case 0x2a:
        case 0x1:
            return 1;
        case 0x19:
        case 0x10:
        case 0x3a:
        case 0xa9:
        case 0xbc:
        case 0x37:
        case 0x12:
        case 0x36:
        case 0x15:
        case 0x38:
        case 0x17:
        case 0x39:
        case 0x18:
            return 2;
        case 0xbd:
        case 0x11:
        case 0xb3:
        case 0xb5:
        case 0xbb:
        case 0x14:
        case 0x13:
        case 0xa8:
        case 0xb6:
        case 0xb8:
        case 0xb7:
        case 0xc1:
        case 0x84:
        case 0xc6:
        case 0xc7:
        case 0x9e:
        case 0x9d:
        case 0x9c:
        case 0x9b:
        case 0x9a:
        case 0x99:
        case 0xa4:
        case 0xa3:
        case 0xa2:
        case 0xa1:
        case 0xa0:
        case 0x9f:
        case 0xa6:
        case 0xa5:
        case 0xa7:
        case 0xb2:
        case 0xb4:
        case 0xc0:
            return 3;
        case 0xc5 :
            return 4;
        case 0xc8:
        case 0xc9:
        case 0xb9:
            return 5;
        case 0xab:
            printf("'lookup switch' instruction is not supported.\n");
            exit(-1);
        case 0xaa :
            printf("'table switch' instruction is not supported.\n");
            exit(-1);
        case 0xc4 :
            printf("'wide' instruction is not supported.\n");
            exit(-1);
        default:
            printf("unknown opCode: '%d'\n", opCode);
            exit(-1);
    }
}

U4 getFieldCount(ExecutionContext *context, const ClassFile *classFile) {
    U4 thisClassFields = classFile->numFields;
    if (classFile->superClassIndex == 0) {
        return thisClassFields;
    }
    U2 classNameIndex = cp_class_getNameIndex(&classFile->constPoolEntries[classFile->superClassIndex]);
    const ClassFile *superClassFile = getClassFile_byName_utf8(context->classLoader, &classFile->constPoolEntries[classNameIndex]);
    return thisClassFields + getFieldCount(context, superClassFile);
}

U4 getSuperClassFieldCount(ExecutionContext *context, const ClassFile *classFile) {
    if (classFile->superClassIndex == 0) {
        return 0;
    }
    U2 classNameIndex = cp_class_getNameIndex(&classFile->constPoolEntries[classFile->superClassIndex]);
    const ClassFile *superClassFile = getClassFile_byName_utf8(context->classLoader, &classFile->constPoolEntries[classNameIndex]);
    return getFieldCount(context, superClassFile);
}

int findFieldIndexInClass(const ClassFile *classFile, const char *fieldName) {
    for (int i = 0; i < classFile->numFields; ++i) {
        const FieldInfo *fieldInfo = &classFile->fields[i];
        const CP_Info *fieldNameUtf8 = &classFile->constPoolEntries[fieldInfo->nameIndex];
        if (cp_cString_utf8_equals(fieldName, fieldNameUtf8)) {
            return i;
        }
    }
    return -1;
}

U4 findFieldIndexInHierarchy(ExecutionContext *context, const ClassFile *classFile, const char *fieldName) {
    int fieldIndex = findFieldIndexInClass(classFile, fieldName);
    if (fieldIndex >= 0) {
        return fieldIndex + getSuperClassFieldCount(context, classFile);
    }
    if (classFile->superClassIndex == 0) {
        printf("field '%s' not found!\n", fieldName);
        exit(-1);
    }
    U2 classNameIndex = cp_class_getNameIndex(&classFile->constPoolEntries[classFile->superClassIndex]);
    const ClassFile *superClassFile = getClassFile_byName_utf8(context->classLoader, &classFile->constPoolEntries[classNameIndex]);
    return findFieldIndexInHierarchy(context, superClassFile, fieldName);
}

int findFieldIndexInClass_utf8(const ClassFile *classFile, const CP_Info *searchUtf8) {
    for (int i = 0; i < classFile->numFields; ++i) {
        const FieldInfo *fieldInfo = &classFile->fields[i];
        const CP_Info *fieldNameUtf8 = &classFile->constPoolEntries[fieldInfo->nameIndex];
        if (cp_utf8_equals(searchUtf8, fieldNameUtf8)) {
            return i;
        }
    }
    return -1;
}

U4 findFieldIndexInHierarchy_utf8(ExecutionContext *context, const ClassFile *classFile, const CP_Info *fieldNameUtf8) {
    int fieldIndex = findFieldIndexInClass_utf8(classFile, fieldNameUtf8);
    if (fieldIndex >= 0) {
        return fieldIndex + getSuperClassFieldCount(context, classFile);
    }
    if (classFile->superClassIndex == 0) {
        U1 *cString = cp_utf8_getCString(fieldNameUtf8);
        printf("field '%s' not found!\n", cString);
        deAllocateBytes(cString);
        exit(-1);
    }
    U2 classNameIndex = cp_class_getNameIndex(&classFile->constPoolEntries[classFile->superClassIndex]);
    const ClassFile *superClassFile = getClassFile_byName_utf8(context->classLoader, &classFile->constPoolEntries[classNameIndex]);
    return findFieldIndexInHierarchy_utf8(context, superClassFile, fieldNameUtf8);
}

bool methodMatches(const ClassFile *classFile, const MethodInfo *methodInfo, const char *methodName, const char *methodDescriptor) {
    if (classFile == NULL || methodInfo == NULL || methodInfo->nameIndex == 0 || methodInfo->descriptorIndex == 0) {
        return false;
    }

    const CP_Info *methodNameUtf8 = &classFile->constPoolEntries[methodInfo->nameIndex];
    const CP_Info *methodDescriptorUtf8 = &classFile->constPoolEntries[methodInfo->descriptorIndex];
    return cp_cString_utf8_equals(methodName, methodNameUtf8) && cp_cString_utf8_equals(methodDescriptor, methodDescriptorUtf8);
}

const MethodInfo *findMethod(const ClassFile *classFile, const char *methodName, const char *methodDescriptor) {
    for (int i = 0; i < classFile->numMethods; ++i) {
        const MethodInfo *method = &classFile->methods[i];
        if (methodMatches(classFile, method, methodName, methodDescriptor)) {
            return method;
        }
    }
    return NULL;
}

bool methodMatches_utf8(const ClassFile *classFile, const MethodInfo *methodInfo,
                        const CP_Info *methodNameUtf8, const CP_Info *methodDescriptorUtf8) {
    if (classFile == NULL || methodInfo == NULL || methodInfo->nameIndex == 0 || methodInfo->descriptorIndex == 0) {
        return false;
    }

    const CP_Info *foundMethodNameUtf8 = &classFile->constPoolEntries[methodInfo->nameIndex];
    const CP_Info *foundMethodDescriptorUtf8 = &classFile->constPoolEntries[methodInfo->descriptorIndex];
    return cp_utf8_equals(foundMethodNameUtf8, methodNameUtf8) && cp_utf8_equals(foundMethodDescriptorUtf8, methodDescriptorUtf8);
}

const MethodInfo *findMethod_utf8(const ClassFile *classFile, const CP_Info *methodNameUtf8, const CP_Info *methodDescriptorUtf8) {
    for (int i = 0; i < classFile->numMethods; ++i) {
        const MethodInfo *method = &classFile->methods[i];
        if (methodMatches_utf8(classFile, method, methodNameUtf8, methodDescriptorUtf8)) {
            return method;
        }
    }
    return NULL;
}

U2 instructionGetShort(Instruction instruction, U1 startIndex) {
    U2 result = instruction.bytes[startIndex];
    result = (result << 8) | instruction.bytes[startIndex + 1];
    return result;
}
