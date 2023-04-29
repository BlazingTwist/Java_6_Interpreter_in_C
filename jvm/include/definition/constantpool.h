#pragma once

#include "integral_types.h"
#include <stdbool.h>

#pragma region Constant_Pool_Tags
#define CP_WidePadding 0
#define CP_Utf8 1
#define CP_Integer 3
#define CP_Float 4
#define CP_Long 5
#define CP_Double 6
#define CP_Class 7
#define CP_String 8
#define CP_FieldRef 9
#define CP_MethodRef 10
#define CP_InterfaceMethodRef 11
#define CP_NameAndType 12
#pragma endregion

typedef struct CP_Info {
    U1 tag;
    const U1 *data;
} CP_Info;

U2 cp_class_getNameIndex(const CP_Info *info);

double cp_double_getValue(const CP_Info *info);

U2 cp_fieldRef_getClassIndex(const CP_Info *info);

U2 cp_fieldRef_getNameAndTypeIndex(const CP_Info *info);

float cp_float_getValue(const CP_Info *info);

I4 cp_integer_getValue(const CP_Info *info);

U2 cp_interfaceMethodRef_getClassIndex(const CP_Info *info);

U2 cp_interfaceMethodRef_getNameAndTypeIndex(const CP_Info *info);

I8 cp_long_getValue(const CP_Info *info);

U2 cp_methodRef_getClassIndex(const CP_Info *info);

U2 cp_methodRef_getNameAndTypeIndex(const CP_Info *info);

U2 cp_nameAndType_getNameIndex(const CP_Info *info);

U2 cp_nameAndType_getDescriptorIndex(const CP_Info *info);

U2 cp_string_getStringIndex(const CP_Info *info);

U2 cp_utf8_getLength(const CP_Info *info);

const U1 *cp_utf8_getBytes(const CP_Info *info);

bool cp_isWide(const CP_Info *info);

void cp_check_tag(int expected, int actual);

U4 cp_getNumBytes(U1 tag);

bool cp_utf8_equals(const CP_Info *infoLeft, const CP_Info *infoRight);

U1 *cp_utf8_getCString(const CP_Info *info);

bool cp_cString_utf8_equals(const char *cString, const CP_Info *utf8);
