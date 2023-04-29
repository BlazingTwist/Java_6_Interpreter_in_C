#include "../../include/interpreter/sourcefileinfo.h"
#include "../../include/util/utf8_utils.h"
#include "../../include/util/allocator.h"
#include "../../include/definition/accessflags.h"
#include "../../include/interpreter/instructionhelper.h"
#include <stdio.h>

ByteReader constructByteReader(U4 position, U4 byteStart, U4 byteEnd, const U1 *bytes) {
    ByteReader reader = {position, byteStart, byteEnd, bytes};
    return reader;
}

ByteReader constructByteReaderForClassFile(const SourceFileInfo sourceFile) {
    U4 numBytes = sourceFile.numBytes;
    ByteReader classFileReader = constructByteReader(0, 0, numBytes, sourceFile.bytes);
    return classFileReader;
}

CP_Info readCPInfo(ByteReader *reader) {
    U1 tag = readU1(reader);
    U4 numBytes;
    if (tag != CP_Utf8) {
        numBytes = cp_getNumBytes(tag);
    } else {
        U2 stringByteCount = readU2(reader);
        reader->position -= 2;
        numBytes = stringByteCount + 2;
    }

    const U1 *data = readByteArray(reader, numBytes);
    CP_Info result = {tag, data};
    return result;
}

CP_Info createCPPadding() {
    CP_Info result = {CP_WidePadding, NULL};
    return result;
}

Attribute readAttribute(ByteReader *reader, const CP_Info *constantPoolEntries) {
    Attribute result = {ATTR_Unknown, 0, 0, NULL};
    result.nameIndex = readU2(reader);
    result.length = readU4(reader);
    if (result.length > 0) {
        result.info = readByteArray(reader, result.length);
    }

    const CP_Info *nameUtf8Entry = &constantPoolEntries[result.nameIndex];
    U2 numUtf8Bytes = cp_utf8_getLength(nameUtf8Entry);
    const U1 *utf8Bytes = cp_utf8_getBytes(nameUtf8Entry);
    if (utf8Matches("Code", utf8Bytes, numUtf8Bytes)) {
        result.attributeIdentifier = ATTR_Code;
    } else if (utf8Matches("ConstantValue", utf8Bytes, numUtf8Bytes)) {
        result.attributeIdentifier = ATTR_ConstantValue;
    } else if (utf8Matches("Exceptions", utf8Bytes, numUtf8Bytes)) {
        result.attributeIdentifier = ATTR_Exceptions;
    }

    return result;
}

ExceptionTableEntry readExceptionTableEntry(ByteReader *reader) {
    ExceptionTableEntry exception = {0, 0, 0, 0};
    exception.startPC = readU2(reader);
    exception.endPC = readU2(reader);
    exception.handlerPC = readU2(reader);
    exception.catchType = readU2(reader);
    return exception;
}

CodeAttribute *readCodeAttribute(ByteReader *reader, const CP_Info *constantPoolEntries) {
    CodeAttribute *result = allocateBytes(sizeof(CodeAttribute));

    result->maxStack = readU2(reader);
    result->maxLocals = readU2(reader);

    U4 numCodeBytes = readU4(reader);
    ByteReader codeReader = readBytes(reader, numCodeBytes);
    U4 numInstructions = 0;
    for (int i = 0; i < numCodeBytes;) {
        U1 instructionBytes = getInstructionByteSize(codeReader.bytes[codeReader.byteStart + i]);
        numInstructions++;
        i += instructionBytes;
    }
    result->numInstructions = numInstructions;
    if (numInstructions <= 0) {
        printf("CodeAttribute was present, but has no instructions!\n");
        exit(-1);
    }
    result->instructions = allocateBytes(sizeof(Instruction) * numInstructions);
    for (int i = 0; i < numInstructions; ++i) {
        U1 numInstructionBytes = getInstructionByteSize(codeReader.bytes[codeReader.position]);
        const U1 *instructionBytes = readByteArray(&codeReader, numInstructionBytes);
        Instruction instruction = {numInstructionBytes, instructionBytes};
        result->instructions[i] = instruction;
    }

    if (codeReader.position != codeReader.byteEnd) {
        printf("codeReader was not exhausted, resulting method is missing instructions!\n");
        exit(-1);
    }

    result->numExceptions = readU2(reader);
    if (result->numExceptions > 0) {
        result->exceptions = allocateBytes(sizeof(ExceptionTableEntry) * result->numExceptions);
        for (int i = 0; i < result->numExceptions; ++i) {
            result->exceptions[i] = readExceptionTableEntry(reader);
        }
    } else {
        result->exceptions = NULL;
    }

    result->numAttributes = readU2(reader);
    if (result->numAttributes > 0) {
        result->attributes = allocateBytes(sizeof(Attribute) * result->numAttributes);
        for (int i = 0; i < result->numAttributes; ++i) {
            result->attributes[i] = readAttribute(reader, constantPoolEntries);
        }
    } else {
        result->attributes = NULL;
    }

    return result;
}

FieldInfo readFieldInfo(ByteReader *reader, const CP_Info *constantPoolEntries) {
    FieldInfo result = {0, 0, 0, 0, NULL};
    result.accessFlags = readU2(reader);
    result.nameIndex = readU2(reader);
    result.descriptorIndex = readU2(reader);
    result.numAttributes = readU2(reader);
    if (result.numAttributes > 0) {
        result.attributes = allocateBytes(sizeof(Attribute) * result.numAttributes);
        for (int i = 0; i < result.numAttributes; ++i) {
            result.attributes[i] = readAttribute(reader, constantPoolEntries);
        }
    }
    return result;
}

U2 getTypeStringLength(const U1 *cString, U2 startPosition) {
    if (cString[startPosition] == 'L') {
        for (int i = startPosition + 1;; i++) {
            if (cString[i] == ';') {
                return i - startPosition + 1;
            }
            if (cString[i] == '\0') {
                break;
            }
        }
        printf("failed to find end of class typeString, string: '%s'\n", cString);
        exit(-1);
    }

    if (cString[startPosition] == '[') {
        return 1 + getTypeStringLength(cString, startPosition + 1);
    }

    return 1;
}

U2 parseNumParameters(U2 accessFlags, U2 descriptorIndex, const CP_Info *constantPoolEntries) {
    U2 numParameters = ((accessFlags & ACC_STATIC) == ACC_STATIC) ? 0 : 1;

    U1 *cString = cp_utf8_getCString(&constantPoolEntries[descriptorIndex]);
    if (cString[0] != '(') {
        printf("expected methodDescriptor to begin with '(', but got '%s'\n", cString);
        exit(-1);
    }

    for (int i = 1;;) {
        if (cString[i] == ')') {
            break;
        }
        numParameters++;
        i += getTypeStringLength(cString, i);
    }
    deAllocateBytes(cString);
    return numParameters;
}

MethodInfo readMethodInfo(ByteReader *reader, const CP_Info *constantPoolEntries) {
    MethodInfo result = {0, 0, 0, 0, 0, NULL, NULL};
    result.accessFlags = readU2(reader);
    result.nameIndex = readU2(reader);
    result.descriptorIndex = readU2(reader);
    result.numParameters = parseNumParameters(result.accessFlags, result.descriptorIndex, constantPoolEntries);

    result.numAttributes = readU2(reader);
    if (result.numAttributes > 0) {
        result.attributes = allocateBytes(sizeof(Attribute) * result.numAttributes);
        for (int i = 0; i < result.numAttributes; ++i) {
            Attribute attribute = readAttribute(reader, constantPoolEntries);
            result.attributes[i] = attribute;
            if (attribute.attributeIdentifier == ATTR_Code) {
                ByteReader attributeReader = constructByteReader(0, 0, attribute.length, attribute.info);
                CodeAttribute *codeAttribute = readCodeAttribute(&attributeReader, constantPoolEntries);
                result.codeAttribute = codeAttribute;
            }
        }
    }
    return result;
}

ClassFile readClassFile(U4 classLoaderIndex, ByteReader *reader) {
    ClassFile result = {
            classLoaderIndex, 0, 0, 0, 0, NULL, 0,
            0, 0, 0, NULL, 0, NULL, 0,
            NULL, 0, NULL
    };
    result.classLoaderIndex = classLoaderIndex;
    result.magic = readU4(reader);
    result.minorVersion = readU2(reader);
    result.majorVersion = readU2(reader);

    result.constPoolCount = readU2(reader);
    result.constPoolEntries = allocateBytes(sizeof(CP_Info) * result.constPoolCount);
    result.constPoolEntries[0] = createCPPadding();
    for (int i = 1; i < result.constPoolCount; ++i) {
        result.constPoolEntries[i] = readCPInfo(reader);
        if (cp_isWide(&result.constPoolEntries[i])) {
            result.constPoolEntries[i + 1] = createCPPadding();
            i++;
        }
    }

    result.accessFlags = readU2(reader);
    result.thisClassIndex = readU2(reader);
    result.superClassIndex = readU2(reader);

    result.numInterfaces = readU2(reader);
    if (result.numInterfaces > 0) {
        result.interfaceIndices = allocateBytes(sizeof(U2) * result.numInterfaces);
        for (int i = 0; i < result.numInterfaces; ++i) {
            result.interfaceIndices[i] = readU2(reader);
        }
    }

    result.numFields = readU2(reader);
    if (result.numFields > 0) {
        result.fields = allocateBytes(sizeof(FieldInfo) * result.numFields);
        for (int i = 0; i < result.numFields; ++i) {
            result.fields[i] = readFieldInfo(reader, result.constPoolEntries);
        }
    }

    result.numMethods = readU2(reader);
    if (result.numMethods > 0) {
        result.methods = allocateBytes(sizeof(MethodInfo) * result.numMethods);
        for (int i = 0; i < result.numMethods; ++i) {
            result.methods[i] = readMethodInfo(reader, result.constPoolEntries);
        }
    }

    result.numAttributes = readU2(reader);
    if (result.numAttributes > 0) {
        result.attributes = allocateBytes(sizeof(Attribute) * result.numAttributes);
        for (int i = 0; i < result.numAttributes; ++i) {
            result.attributes[i] = readAttribute(reader, result.constPoolEntries);
        }
    }

    return result;
}

void releaseSourceFile(SourceFileInfo *sourceFile) {
    if (sourceFile == NULL) {
        return;
    }
    if (sourceFile->bytes != NULL) {
        deAllocateBytes(sourceFile->bytes);
        sourceFile->bytes = NULL;
    }
}

void releaseClassFile(ClassFile *classFile) {
    if (classFile == NULL) {
        return;
    }
    if (classFile->attributes != NULL) {
        deAllocateBytes(classFile->attributes);
        classFile->attributes = NULL;
    }
    if (classFile->methods != NULL) {
        for (int i = 0; i < classFile->numMethods; ++i) {
            releaseClassFile_Method(&classFile->methods[i]);
        }
        deAllocateBytes(classFile->methods);
        classFile->methods = NULL;
    }
    if (classFile->fields != NULL) {
        for (int i = 0; i < classFile->numFields; ++i) {
            releaseClassFile_Field(&classFile->fields[i]);
        }
        deAllocateBytes(classFile->fields);
        classFile->fields = NULL;
    }
    if (classFile->interfaceIndices != NULL) {
        deAllocateBytes(classFile->interfaceIndices);
        classFile->interfaceIndices = NULL;
    }
    if (classFile->constPoolEntries != NULL) {
        deAllocateBytes(classFile->constPoolEntries);
        classFile->constPoolEntries = NULL;
    }
}

void releaseClassFile_Field(FieldInfo *fieldInfo) {
    if (fieldInfo == NULL) {
        return;
    }
    if (fieldInfo->attributes != NULL) {
        deAllocateBytes(fieldInfo->attributes);
        fieldInfo->attributes = NULL;
    }
}

void releaseClassFile_Method(MethodInfo *methodInfo) {
    if (methodInfo == NULL) {
        return;
    }
    if (methodInfo->attributes != NULL) {
        deAllocateBytes(methodInfo->attributes);
        methodInfo->attributes = NULL;
    }
    if (methodInfo->codeAttribute != NULL) {
        releaseCodeAttribute(methodInfo->codeAttribute);
        deAllocateBytes(methodInfo->codeAttribute);
        methodInfo->codeAttribute = NULL;
    }
}

void releaseCodeAttribute(CodeAttribute *codeAttribute) {
    if (codeAttribute == NULL) {
        return;
    }
    if (codeAttribute->instructions != NULL) {
        deAllocateBytes(codeAttribute->instructions);
        codeAttribute->instructions = NULL;
    }
    if (codeAttribute->exceptions != NULL) {
        deAllocateBytes(codeAttribute->exceptions);
        codeAttribute->exceptions = NULL;
    }
    if (codeAttribute->attributes != NULL) {
        deAllocateBytes(codeAttribute->attributes);
        codeAttribute->attributes = NULL;
    }
}
