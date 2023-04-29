#pragma once

#include "integral_types.h"

#pragma region Attribute_Identifiers
#define ATTR_Unknown 0
#define ATTR_Code 1
#define ATTR_ConstantValue 2
#define ATTR_Exceptions 3
#pragma endregion

typedef struct Attribute {
    U1 attributeIdentifier;
    U2 nameIndex;
    U4 length;
    const U1 *info;
} Attribute;
