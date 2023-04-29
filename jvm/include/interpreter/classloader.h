#pragma once

#include "../definition/integral_types.h"
#include "../definition/classfile.h"
#include "sourcefileinfo.h"

typedef struct ClassLoader {
    U4 maxNumClassFiles;
    U4 numClassFiles;
    ClassFile *classFiles;
    SourceFileInfo **sourceFiles;
} ClassLoader;

ClassLoader* initClassLoader(U4 maxNumClassFiles);

void releaseClassLoader(ClassLoader *classLoader);

const ClassFile *loadClassFile(ClassLoader *classLoader, SourceFileInfo *sourceFile);

const ClassFile *getClassFile_byIndex(const ClassLoader *classLoader, U4 index);

const ClassFile *getClassFile_byName(const ClassLoader *classLoader, const char *cString);

const ClassFile *getClassFile_byName_utf8(const ClassLoader *classLoader, const CP_Info *utf8ClassName);

const CP_Info *cp_class_resolveClassName(const CP_Info *info, const ClassFile *cpClass);

const CP_Info *cp_fieldRef_resolveClassNameUtf8(const CP_Info *info, const ClassFile *cpClass);

const CP_Info *cp_fieldRef_resolveFieldNameUtf8(const CP_Info *info, const ClassFile *cpClass);

const ClassFile *cp_methodRef_resolveClassFile(const CP_Info *info, const ClassLoader *classLoader, const ClassFile *thisClass);

const MethodInfo *cp_methodRef_resolveMethod(const CP_Info *info, const ClassFile *interfaceClass, const ClassFile *cpClass);

const CP_Info *cp_nameAndType_resolveNameUtf8(const CP_Info *info, const ClassFile *cpClass);