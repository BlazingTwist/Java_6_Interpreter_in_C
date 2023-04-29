#pragma once

#include "integral_types.h"
#include "constantpool.h"
#include "fieldinfo.h"
#include "methodinfo.h"
#include "attribute.h"

typedef struct ClassFile {
    U4 classLoaderIndex;
    U4 magic;
    U2 minorVersion;
    U2 majorVersion;
    U2 constPoolCount;
    CP_Info *constPoolEntries;
    U2 accessFlags;
    U2 thisClassIndex;
    U2 superClassIndex;
    U2 numInterfaces;
    U2 *interfaceIndices;
    U2 numFields;
    FieldInfo *fields;
    U2 numMethods;
    MethodInfo *methods;
    U2 numAttributes;
    Attribute *attributes;
} ClassFile;
