#pragma once

#include "integral_types.h"
#include "attribute.h"
#include "codeattribute.h"

typedef struct MethodInfo {
    U2 accessFlags;
    U2 nameIndex;
    U2 descriptorIndex;
    U2 numParameters;
    U2 numAttributes;
    Attribute *attributes;
    CodeAttribute *codeAttribute;
} MethodInfo;
