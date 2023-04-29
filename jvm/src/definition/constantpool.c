#include "../../include/definition/constantpool.h"
#include "../../include/util/utf8_utils.h"
#include "../../include/util/allocator.h"
#include <stdio.h>
#include <stdlib.h>

U2 cp_class_getNameIndex(const CP_Info *info) {
    cp_check_tag(CP_Class, info->tag);
    U2 result = info->data[0];
    result = (result << 8) | info->data[1];
    return result;
}

double cp_double_getValue(const CP_Info *info) {
    cp_check_tag(CP_Double, info->tag);
    U8 result_bits = info->data[0];
    result_bits = (result_bits << 8) | info->data[1];
    result_bits = (result_bits << 8) | info->data[2];
    result_bits = (result_bits << 8) | info->data[3];
    result_bits = (result_bits << 8) | info->data[4];
    result_bits = (result_bits << 8) | info->data[5];
    result_bits = (result_bits << 8) | info->data[6];
    result_bits = (result_bits << 8) | info->data[7];
    return (double) result_bits;
}

U2 cp_fieldRef_getClassIndex(const CP_Info *info) {
    cp_check_tag(CP_FieldRef, info->tag);
    U2 result = info->data[0];
    result = (result << 8) | info->data[1];
    return result;
}

U2 cp_fieldRef_getNameAndTypeIndex(const CP_Info *info) {
    cp_check_tag(CP_FieldRef, info->tag);
    U2 result = info->data[2];
    result = (result << 8) | info->data[3];
    return result;
}

float cp_float_getValue(const CP_Info *info) {
    cp_check_tag(CP_Float, info->tag);
    U4 result_bits = info->data[0];
    result_bits = (result_bits << 8) | info->data[1];
    result_bits = (result_bits << 8) | info->data[2];
    result_bits = (result_bits << 8) | info->data[3];
    return (float) result_bits;
}

I4 cp_integer_getValue(const CP_Info *info) {
    cp_check_tag(CP_Integer, info->tag);
    U4 result_bits = info->data[0];
    result_bits = (result_bits << 8) | info->data[1];
    result_bits = (result_bits << 8) | info->data[2];
    result_bits = (result_bits << 8) | info->data[3];
    return (I4) result_bits;
}

U2 cp_interfaceMethodRef_getClassIndex(const CP_Info *info) {
    cp_check_tag(CP_InterfaceMethodRef, info->tag);
    U2 result = info->data[0];
    result = (result << 8) | info->data[1];
    return result;
}

U2 cp_interfaceMethodRef_getNameAndTypeIndex(const CP_Info *info) {
    cp_check_tag(CP_InterfaceMethodRef, info->tag);
    U2 result = info->data[2];
    result = (result << 8) | info->data[3];
    return result;
}

I8 cp_long_getValue(const CP_Info *info) {
    cp_check_tag(CP_Long, info->tag);
    U8 result_bits = info->data[0];
    result_bits = (result_bits << 8) | info->data[1];
    result_bits = (result_bits << 8) | info->data[2];
    result_bits = (result_bits << 8) | info->data[3];
    result_bits = (result_bits << 8) | info->data[4];
    result_bits = (result_bits << 8) | info->data[5];
    result_bits = (result_bits << 8) | info->data[6];
    result_bits = (result_bits << 8) | info->data[7];
    return (I8) result_bits;
}

U2 cp_methodRef_getClassIndex(const CP_Info *info) {
    cp_check_tag(CP_MethodRef, info->tag);
    U2 result = info->data[0];
    result = (result << 8) | info->data[1];
    return result;
}

U2 cp_methodRef_getNameAndTypeIndex(const CP_Info *info) {
    cp_check_tag(CP_MethodRef, info->tag);
    U2 result = info->data[2];
    result = (result << 8) | info->data[3];
    return result;
}

U2 cp_nameAndType_getNameIndex(const CP_Info *info) {
    cp_check_tag(CP_NameAndType, info->tag);
    U2 result = info->data[0];
    result = (result << 8) | info->data[1];
    return result;
}

U2 cp_nameAndType_getDescriptorIndex(const CP_Info *info) {
    cp_check_tag(CP_NameAndType, info->tag);
    U2 result = info->data[2];
    result = (result << 8) | info->data[3];
    return result;
}

U2 cp_string_getStringIndex(const CP_Info *info) {
    cp_check_tag(CP_String, info->tag);
    U2 result = info->data[0];
    result = (result << 8) | info->data[1];
    return result;
}

U2 cp_utf8_getLength(const CP_Info *info) {
    cp_check_tag(CP_Utf8, info->tag);
    U2 result = info->data[0];
    result = (result << 8) | info->data[1];
    return result;
}

const U1 *cp_utf8_getBytes(const CP_Info *info) {
    cp_check_tag(CP_Utf8, info->tag);
    return info->data + 2;
}

bool cp_isWide(const CP_Info *info) {
    return info->tag == CP_Long | info->tag == CP_Double;
}

void cp_check_tag(int expected, int actual) {
    if (expected != actual) {
        printf("expected CP tag '%d', but actual tag is '%d'\n", expected, actual);
        exit(-1);
    }
}

U4 cp_getNumBytes(U1 tag) {
    switch (tag) {
        case CP_Class:
            return 2;
        case CP_FieldRef:
            return 4;
        case CP_MethodRef:
            return 4;
        case CP_InterfaceMethodRef:
            return 4;
        case CP_String:
            return 2;
        case CP_Integer:
            return 4;
        case CP_Float:
            return 4;
        case CP_Long:
            return 8;
        case CP_Double:
            return 8;
        case CP_NameAndType:
            return 4;
        case CP_Utf8: {
            printf("Utf8 is not supported for cp_getNumBytes!\n");
            exit(-1);
        }
        default: {
            printf("Unknown tag: %d\n", tag);
            exit(-1);
        }
    }
}

bool cp_utf8_equals(const CP_Info *infoLeft, const CP_Info *infoRight) {
    U2 numBytesLeft = cp_utf8_getLength(infoLeft);
    U2 numBytesRight = cp_utf8_getLength(infoRight);
    if (numBytesLeft != numBytesRight) {
        return false;
    }
    return utf8Equals(cp_utf8_getBytes(infoLeft), cp_utf8_getBytes(infoRight), numBytesLeft);
}

U1 *cp_utf8_getCString(const CP_Info *info) {
    U2 numBytes = cp_utf8_getLength(info);
    const U1 *utf8Bytes = cp_utf8_getBytes(info);

    U2 numChars = numBytes + 1; // '\0'
    U1 *utf8AsCString = allocateBytes(sizeof(U1) * numChars);
    for (int i = 0; i < numBytes; ++i) {
        utf8AsCString[i] = utf8Bytes[i];
    }
    utf8AsCString[numBytes] = '\0';

    return utf8AsCString;
}

bool cp_cString_utf8_equals(const char *cString, const CP_Info *utf8) {
    U2 numUtf8Bytes = cp_utf8_getLength(utf8);
    const U1 *utf8Bytes = cp_utf8_getBytes(utf8);
    return utf8Matches(cString, utf8Bytes, numUtf8Bytes);
}
