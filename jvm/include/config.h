#pragma once

/**
 * Track allocations throughout the JVMs lifetime and print them occasionally.
 */
#define PRINT_MEM_STATS

/**
 * Print method invokations and returns.
 */
//#define PRINT_METHOD_STACK

/**
 * Print opcode of executed instructions.
 */
//#define PRINT_OPCODES

/**
 * Print info whenever a field is assigned.
 */
//#define PRINT_PUT_FIELD



#ifdef PRINT_MEM_STATS

#include "definition/integral_types.h"

extern U4 totalBytesAllocated;
extern int numAllocations;
extern int numDeAllocations;
extern int numHeapPointersCreated;
extern int numHeapPointersReleased;
#endif