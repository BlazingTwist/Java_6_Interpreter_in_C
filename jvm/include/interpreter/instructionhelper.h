#pragma once

#include "../definition/integral_types.h"
#include "executioncontext.h"

U1 getInstructionByteSize(U1 opCode);

U4 getFieldCount(ExecutionContext *context, const ClassFile *classFile);

U4 getSuperClassFieldCount(ExecutionContext *context, const ClassFile *classFile);

int findFieldIndexInClass(const ClassFile *classFile, const char *fieldName);

U4 findFieldIndexInHierarchy(ExecutionContext *context, const ClassFile *classFile, const char *fieldName);

int findFieldIndexInClass_utf8(const ClassFile *classFile, const CP_Info *searchUtf8);

U4 findFieldIndexInHierarchy_utf8(ExecutionContext *context, const ClassFile *classFile, const CP_Info *fieldNameUtf8);

bool methodMatches(const ClassFile *classFile, const MethodInfo *methodInfo, const char *methodName, const char *methodDescriptor);

const MethodInfo *findMethod(const ClassFile *classFile, const char *methodName, const char *methodDescriptor);

bool methodMatches_utf8(const ClassFile *classFile, const MethodInfo *methodInfo,
                        const CP_Info *methodNameUtf8, const CP_Info *methodDescriptorUtf8);

const MethodInfo *findMethod_utf8(const ClassFile *classFile, const CP_Info *methodNameUtf8, const CP_Info *methodDescriptorUtf8);

U2 instructionGetShort(Instruction instruction, U1 startIndex);
