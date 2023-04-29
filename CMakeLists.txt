cmake_minimum_required(VERSION 3.20)
project(C_JVM C)

set(CMAKE_C_STANDARD 99)

set(
        JVM_INCLUDES
        "jvm/include/config.h" "jvm/include/interpretedjvm.h" "jvm/include/definition/accessflags.h"
        "jvm/include/definition/attribute.h" "jvm/include/definition/classfile.h" "jvm/include/definition/codeattribute.h"
        "jvm/include/definition/constantpool.h" "jvm/include/definition/fieldinfo.h" "jvm/include/definition/integral_types.h"
        "jvm/include/definition/methodinfo.h" "jvm/include/interpreter/classloader.h" "jvm/include/interpreter/executioncontext.h"
        "jvm/include/interpreter/executionframe.h" "jvm/include/interpreter/instructionhelper.h" "jvm/include/interpreter/objectinstance.h"
        "jvm/include/interpreter/opcodes.h" "jvm/include/interpreter/sourcefileinfo.h" "jvm/include/util/allocator.h"
        "jvm/include/util/bytereader.h" "jvm/include/util/allocator.h" "jvm/include/util/hex_utils.h"
        "jvm/include/util/sourcefiledecoder.h" "jvm/include/util/utf8_utils.h"
)

set(
        JVM_SOURCES
        "jvm/src/interpretedjvm.c" "jvm/src/definition/constantpool.c" "jvm/src/interpreter/sourcefileinfo.c"
        "jvm/src/interpreter/classloader.c" "jvm/src/interpreter/executioncontext.c" "jvm/src/interpreter/executionframe.c"
        "jvm/src/interpreter/instructionhelper.c" "jvm/src/interpreter/objectinstance.c" "jvm/src/interpreter/opcodes.c"
        "jvm/src/util/allocator.c" "jvm/src/util/bytereader.c" "jvm/src/util/hex_utils.c" "jvm/src/util/sourcefiledecoder.c"
        "jvm/src/util/utf8_utils.c"
)

add_library(LIB_JVM ${JVM_INCLUDES} ${JVM_SOURCES})

add_executable(1_1_tree pgp/ex1/tree.c)
target_link_libraries(1_1_tree PUBLIC LIB_JVM)

add_executable(2_1_anagram pgp/ex2/anagram.c)
target_link_libraries(2_1_anagram PUBLIC LIB_JVM)

add_executable(2_2_concat pgp/ex2/concat.c)
target_link_libraries(2_2_concat PUBLIC LIB_JVM)

add_executable(2_3_dynArray pgp/ex2/dynamic_array.c)
target_link_libraries(2_3_dynArray PUBLIC LIB_JVM)
