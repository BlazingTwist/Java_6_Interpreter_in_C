#pragma once

#include "../definition/integral_types.h"
#include "../util/bytereader.h"
#include "../definition/constantpool.h"
#include "../definition/methodinfo.h"
#include "../definition/fieldinfo.h"
#include "../definition/classfile.h"

typedef struct SourceFileInfo {
    U4 numBytes;
    U1 *bytes;
} SourceFileInfo;

ByteReader constructByteReader(U4 position, U4 byteStart, U4 byteEnd, const U1 *bytes);

ByteReader constructByteReaderForClassFile(const SourceFileInfo sourceFile);

CP_Info readCPInfo(ByteReader *reader);

CP_Info createCPPadding();

ExceptionTableEntry readExceptionTableEntry(ByteReader *reader);

CodeAttribute *readCodeAttribute(ByteReader *reader, const CP_Info *constantPoolEntries);

Attribute readAttribute(ByteReader *reader, const CP_Info *constantPoolEntries);

FieldInfo readFieldInfo(ByteReader *reader, const CP_Info *constantPoolEntries);

U2 getTypeStringLength(const U1 *cString, U2 startPosition);

U2 parseNumParameters(U2 accessFlags, U2 descriptorIndex, const CP_Info *constantPoolEntries);

MethodInfo readMethodInfo(ByteReader *reader, const CP_Info *constantPoolEntries);

ClassFile readClassFile(U4 classLoaderIndex, ByteReader *reader);

void releaseSourceFile(SourceFileInfo *sourceFile);

void releaseClassFile(ClassFile *classFile);

void releaseClassFile_Field(FieldInfo *fieldInfo);

void releaseClassFile_Method(MethodInfo *methodInfo);

void releaseCodeAttribute(CodeAttribute *codeAttribute);
