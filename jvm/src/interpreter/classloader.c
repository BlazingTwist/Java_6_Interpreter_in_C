#include "../../include/interpreter/classloader.h"
#include "../../include/util/allocator.h"
#include "../../include/interpreter/instructionhelper.h"
#include <stdio.h>

ClassLoader *initClassLoader(U4 numSourceFiles) {
    ClassLoader *result = allocateBytes(sizeof(ClassLoader));
    result->maxNumClassFiles = numSourceFiles;
    result->numClassFiles = 0;
    result->classFiles = allocateBytes(sizeof(ClassFile) * numSourceFiles);
    result->sourceFiles = allocateBytes(sizeof(SourceFileInfo *) * numSourceFiles);
    return result;
}

void releaseClassLoader(ClassLoader *classLoader) {
    for (int i = 0; i < classLoader->numClassFiles; ++i) {
        releaseClassFile(&classLoader->classFiles[i]);
        releaseSourceFile(classLoader->sourceFiles[i]);
        deAllocateBytes(classLoader->sourceFiles[i]);
        classLoader->sourceFiles[i] = NULL;
    }
    deAllocateBytes(classLoader->classFiles);
    classLoader->classFiles = NULL;
    deAllocateBytes(classLoader->sourceFiles);
    classLoader->sourceFiles = NULL;
    deAllocateBytes(classLoader);
}

const ClassFile *loadClassFile(ClassLoader *classLoader, SourceFileInfo *sourceFile) {
    ByteReader reader = constructByteReaderForClassFile(*sourceFile);
    U4 classFileIndex = classLoader->numClassFiles;
    ClassFile classFile = readClassFile(classFileIndex, &reader);
    classLoader->classFiles[classFileIndex] = classFile;
    classLoader->sourceFiles[classFileIndex] = sourceFile;
    classLoader->numClassFiles++;
    return &classLoader->classFiles[classFileIndex];
}

const ClassFile *getClassFile_byIndex(const ClassLoader *classLoader, U4 index) {
    U4 numClassFiles = classLoader->numClassFiles;
    if (index >= numClassFiles) {
        printf("ClassFile-Index out of bounds, index %lu exceeds numClassFiles %lu\n", index, numClassFiles);
        exit(-1);
    }
    return &classLoader->classFiles[index];
}

const ClassFile *getClassFile_byName(const ClassLoader *classLoader, const char *cString) {
    U4 numClassFiles = classLoader->numClassFiles;
    for (int i = 0; i < numClassFiles; ++i) {
        const ClassFile *matchCandidate = getClassFile_byIndex(classLoader, i);
        U2 uft8NameIndex = cp_class_getNameIndex(&matchCandidate->constPoolEntries[matchCandidate->thisClassIndex]);
        const CP_Info *candidateClassName = &matchCandidate->constPoolEntries[uft8NameIndex];
        if (cp_cString_utf8_equals(cString, candidateClassName)) {
            return matchCandidate;
        }
    }

    printf("Class not found in ClassLoader. ClassName: '%s'\n", cString);
    exit(-1);
}

const ClassFile *getClassFile_byName_utf8(const ClassLoader *classLoader, const CP_Info *utf8ClassName) {
    U4 numClassFiles = classLoader->numClassFiles;
    for (int i = 0; i < numClassFiles; ++i) {
        const ClassFile *matchCandidate = getClassFile_byIndex(classLoader, i);
        U2 uft8NameIndex = cp_class_getNameIndex(&matchCandidate->constPoolEntries[matchCandidate->thisClassIndex]);
        const CP_Info *candidateClassName = &matchCandidate->constPoolEntries[uft8NameIndex];
        if (cp_utf8_equals(candidateClassName, utf8ClassName)) {
            return matchCandidate;
        }
    }

    U1 *classNameCString = cp_utf8_getCString(utf8ClassName);
    printf("Class not found in ClassLoader. ClassName: '%s'\n", classNameCString);
    deAllocateBytes(classNameCString);
    exit(-1);
}

const CP_Info *cp_class_resolveClassName(const CP_Info *info, const ClassFile *cpClass) {
    return &cpClass->constPoolEntries[cp_class_getNameIndex(info)];
}

const CP_Info *cp_fieldRef_resolveClassNameUtf8(const CP_Info *info, const ClassFile *cpClass) {
    const CP_Info *classRef = &cpClass->constPoolEntries[cp_fieldRef_getClassIndex(info)];
    return cp_class_resolveClassName(classRef, cpClass);
}

const CP_Info *cp_fieldRef_resolveFieldNameUtf8(const CP_Info *info, const ClassFile *cpClass) {
    const CP_Info *nameAndType = &cpClass->constPoolEntries[cp_fieldRef_getNameAndTypeIndex(info)];
    return cp_nameAndType_resolveNameUtf8(nameAndType, cpClass);
}

const ClassFile *
cp_methodRef_resolveClassFile(const CP_Info *info, const ClassLoader *classLoader, const ClassFile *thisClass) {
    CP_Info *classRef = &thisClass->constPoolEntries[cp_methodRef_getClassIndex(info)];
    CP_Info *classNameUtf8 = &thisClass->constPoolEntries[cp_class_getNameIndex(classRef)];
    return getClassFile_byName_utf8(classLoader, classNameUtf8);
}

const MethodInfo *
cp_methodRef_resolveMethod(const CP_Info *info, const ClassFile *interfaceClass, const ClassFile *cpClass) {
    CP_Info *nameAndType = &cpClass->constPoolEntries[cp_methodRef_getNameAndTypeIndex(info)];
    CP_Info *nameUtf8 = &cpClass->constPoolEntries[cp_nameAndType_getNameIndex(nameAndType)];
    CP_Info *descriptorUtf8 = &cpClass->constPoolEntries[cp_nameAndType_getDescriptorIndex(nameAndType)];
    return findMethod_utf8(interfaceClass, nameUtf8, descriptorUtf8);
}

const CP_Info *cp_nameAndType_resolveNameUtf8(const CP_Info *info, const ClassFile *cpClass) {
    return &cpClass->constPoolEntries[cp_nameAndType_getNameIndex(info)];
}
