#pragma once

#include "integral_types.h"
#include "attribute.h"

typedef struct FieldInfo {
    U2 accessFlags;
    U2 nameIndex;
    U2 descriptorIndex;
    U2 numAttributes;
    Attribute *attributes;
} FieldInfo;
