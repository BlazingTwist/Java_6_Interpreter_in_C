#pragma once

#include "executioncontext.h"
#include "executionframe.h"

void op_aConst_null(ExecutionContext *context);

void op_new(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_newArray(ExecutionContext *context);

void op_aNewArray(ExecutionContext *context);

void op_arrayLength(ExecutionContext *context);

void op_ldc(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_dup(ExecutionContext *context);

void op_dup_x1(ExecutionContext *context);

void op_dup2(ExecutionContext *context);

void op_invokeSpecial(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_invokeVirtual(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_invokeStatic(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_baLoad(ExecutionContext *context);

void op_caLoad(ExecutionContext *context);

void op_iaLoad(ExecutionContext *context);

void op_iShL(ExecutionContext *context);

void op_iuShR(ExecutionContext *context);

void op_iOr(ExecutionContext *context);

void op_i2b();

void op_i2c();

void op_iAdd(ExecutionContext *context);

void op_iSub(ExecutionContext *context);

void op_iMul(ExecutionContext *context);

void op_iAnd(ExecutionContext *context);

void op_getField(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_putField(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_getStatic(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_putStatic(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_iaStore(ExecutionContext *context);

void op_baStore(ExecutionContext *context);

void op_caStore(ExecutionContext *context);

void branch(ExecutionFrame *frame, U2 branchOffset);

void op_if_iCmp_eq(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_if_iCmp_ne(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_if_iCmp_lt(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_if_iCmp_le(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_if_iCmp_gt(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_if_iCmp_ge(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_ifNonNull(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_ifEq(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_ifNE(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_ifLT(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_ifGE(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_ifGT(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_ifLE(ExecutionContext *context, ExecutionFrame *frame, Instruction instruction);

void op_iInc(ExecutionFrame *frame, Instruction instruction);

void op_pop(ExecutionContext *context);

void op_pop2(ExecutionContext *context);
