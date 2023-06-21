#pragma once

#include "compiler.h"

static Exception vmAllocate(QuarkVM *vm)
{
    if (vm->stackSize < 1) return EX_STACK_UNDERFLOW;

    vm->stack[vm->stackSize - 1].asPtr = malloc(vm->stack[vm->stackSize - 1].asI64);
    return EX_OK;
}

static Exception vmFree(QuarkVM *vm)
{
    if (vm->stackSize < 1) return EX_STACK_UNDERFLOW;

    free(vm->stack[vm->stackSize - 1].asPtr);
    --vm->stackSize;

    return EX_OK;
}

static Exception vmPrintF64(QuarkVM *vm)
{
    if (vm->stackSize < 1) return EX_STACK_UNDERFLOW;

    printf("%lf\n", vm->stack[vm->stackSize - 1].asF64);
    --vm->stackSize;

    return EX_OK;
}

static Exception vmPrintI64(QuarkVM *vm)
{
    if (vm->stackSize < 1) return EX_STACK_UNDERFLOW;

    printf("%" PRId64 "\n", vm->stack[vm->stackSize - 1].asI64);
    --vm->stackSize;

    return EX_OK;
}

static Exception vmPrintPtr(QuarkVM *vm)
{
    if (vm->stackSize < 1) return EX_STACK_UNDERFLOW;

    printf("%p\n", vm->stack[vm->stackSize - 1].asPtr);
    --vm->stackSize;

    return EX_OK;
}
