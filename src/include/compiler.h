#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <inttypes.h>

#include "stringview.h"

#define VM_CAPACITY 1024
#define VM_STACK_CAPACITY (VM_CAPACITY * VM_CAPACITY)

int VM_EXECUTION_LIMIT = 64;

typedef enum
{
    EX_OK = 0,
    EX_STACK_OVERFLOW,
    EX_STACK_UNDERFLOW,
    EX_INVALID_INSTRUCTION,
    EX_ILLEGAL_INSTRUCTION_ACCESS,
    EX_ILLEGAL_OPERATION,
    EX_DIVIDE_BY_ZERO,
} Exception;

typedef enum
{
    INST_KAPUT = 0,
    INST_PUT,
    INST_DUP,
    INST_SWAP,
    INST_RELEASE,

    INST_IPLUS,
    INST_IMINUS,
    INST_IMUL,
    INST_IDIV,
    INST_IMOD,

    INST_FPLUS,
    INST_FMINUS,
    INST_FMUL,
    INST_FDIV,
    INST_FMOD,

    INST_JUMP,
    INST_JUMP_IF,
    INST_RETURN,
    INST_INVOKE,
    INST_NATIVE,

    INST_IEQ,
    INST_INEQ,
    INST_IGT,
    INST_ILT,
    INST_IGEQ,
    INST_ILEQ,

    INST_FEQ,
    INST_FNEQ,
    INST_FGT,
    INST_FLT,
    INST_FGEQ,
    INST_FLEQ,

    INST_HALT,
    TOTAL_INST_COUNT,
} InstructionType;

typedef union
{
    int64_t asI64;
    double asF64;
    void *asPtr;
} Word;

typedef struct
{
    InstructionType type;
    Word value;
} Instruction;

typedef struct QuarkVM QuarkVM;

typedef Exception(*NativeVM)(QuarkVM *);

struct QuarkVM
{
    Word stack[VM_STACK_CAPACITY];
    int64_t stackSize;

    Instruction program[VM_CAPACITY];
    int64_t programSize;
    int64_t instructionPointer;

    NativeVM nativeFunctions[VM_CAPACITY];
    int64_t nativeFunctionsSize;

    int halt;
};

typedef struct
{
    StringView label;
    int64_t address;
} Function;

typedef struct
{
    StringView label;
    int64_t address;
} DelayedOperand;

typedef struct
{
    Function functions[VM_CAPACITY];
    DelayedOperand delayedOperands[VM_CAPACITY];
    int64_t functionSize;
    int64_t delayedOperandSize;
} VMTable;

static_assert(sizeof(Word) == 8, "The word size must be 64 bytes");

static const char *exceptionAsCString(Exception exception)
{
    switch (exception)
    {
        case EX_OK:
            return "All OK";
        case EX_STACK_OVERFLOW:
            return "Stack overflow";
        case EX_STACK_UNDERFLOW:
            return "Stack underflow";
        case EX_INVALID_INSTRUCTION:
            return "Invalid instruction";
        case EX_DIVIDE_BY_ZERO:
            return "Dividing by zero";
        case EX_ILLEGAL_INSTRUCTION_ACCESS:
            return "Illegal instruction access";
        case EX_ILLEGAL_OPERATION:
            return "Illegal operation";
        default:
            assert(0 && "[exceptionAsCString]: Unreachable");
    }
}

static void vmDumpStack(FILE *stream, const QuarkVM *quarkVm)
{
    fprintf(stream, "Stack:");

    if (quarkVm->stackSize > 0)
    {
        fprintf(stream, "\n");
        for (int64_t i = 0; i < quarkVm->stackSize; ++i)
            fprintf(stream, "  I64: %" PRId64 ", F64: %lf, PTR: %p\n", quarkVm->stack[i].asI64, quarkVm->stack[i].asF64,
                    quarkVm->stack[i].asPtr);
    } else fprintf(stream, " [empty]\n");
}

static const char *getInstructionName(InstructionType type)
{
    switch (type)
    {
        case INST_KAPUT:
            return "kaput";
        case INST_PUT:
            return "put";
        case INST_DUP:
            return "dup";
        case INST_SWAP:
            return "swap";
        case INST_RELEASE:
            return "release";

        case INST_IPLUS:
            return "iplus";
        case INST_IMINUS:
            return "iminus";
        case INST_IMUL:
            return "imul";
        case INST_IDIV:
            return "idiv";
        case INST_IMOD:
            return "imod";

        case INST_FPLUS:
            return "fplus";
        case INST_FMINUS:
            return "fminus";
        case INST_FMUL:
            return "fmul";
        case INST_FDIV:
            return "fdiv";
        case INST_FMOD:
            return "fmod";

        case INST_JUMP:
            return "jmp";
        case INST_JUMP_IF:
            return "jif";
        case INST_RETURN:
            return "return";
        case INST_INVOKE:
            return "invoke";
        case INST_NATIVE:
            return "native";

        case INST_IEQ:
            return "ieq";
        case INST_INEQ:
            return "ineq";
        case INST_IGT:
            return "igt";
        case INST_ILT:
            return "ilt";
        case INST_IGEQ:
            return "ige";
        case INST_ILEQ:
            return "ile";

        case INST_FEQ:
            return "feq";
        case INST_FNEQ:
            return "fneq";
        case INST_FGT:
            return "fgt";
        case INST_FLT:
            return "flt";
        case INST_FGEQ:
            return "fge";
        case INST_FLEQ:
            return "fle";

        case INST_HALT:
            return "stop";
        case TOTAL_INST_COUNT:
        default:
            assert(0 && "[instructionName]: Unreachable");
    }
}

static int instructionWithOperand(InstructionType type)
{
    switch (type)
    {
        case INST_KAPUT:
        case INST_RELEASE:
        case INST_IPLUS:
        case INST_IMINUS:
        case INST_IMUL:
        case INST_IDIV:
        case INST_IMOD:
        case INST_FPLUS:
        case INST_FMINUS:
        case INST_FMUL:
        case INST_FDIV:
        case INST_FMOD:
        case INST_RETURN:
        case INST_IEQ:
        case INST_INEQ:
        case INST_IGT:
        case INST_ILT:
        case INST_IGEQ:
        case INST_ILEQ:
        case INST_FEQ:
        case INST_FNEQ:
        case INST_FGT:
        case INST_FLT:
        case INST_FGEQ:
        case INST_FLEQ:
        case INST_HALT:
            return 0;
        case INST_PUT:
        case INST_DUP:
        case INST_SWAP:
        case INST_JUMP:
        case INST_JUMP_IF:
        case INST_INVOKE:
        case INST_NATIVE:
            return 1;
        case TOTAL_INST_COUNT:
        default:
            assert(0 && "[instructionWithOperand]: Unreachable");
    }
}

static Exception vmExecuteInstruction(QuarkVM *vm)
{
    if (vm->instructionPointer >= vm->programSize) return EX_ILLEGAL_INSTRUCTION_ACCESS;

    Instruction instruction = vm->program[vm->instructionPointer];
    switch (instruction.type)
    {
        case INST_KAPUT:
            ++vm->instructionPointer;
            break;
        case INST_PUT:
            if (vm->stackSize >= VM_STACK_CAPACITY) return EX_STACK_OVERFLOW;

            vm->stack[vm->stackSize++] = instruction.value;
            ++vm->instructionPointer;

            break;
        case INST_DUP:
            if (vm->stackSize >= VM_STACK_CAPACITY) return EX_STACK_OVERFLOW;
            if (vm->stackSize - instruction.value.asI64 <= 0) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize] = vm->stack[vm->stackSize - instruction.value.asI64 - 1];
            ++vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_SWAP:
            if (instruction.value.asI64 >= (int64_t) vm->stackSize) return EX_STACK_UNDERFLOW;

            const int64_t a = vm->stackSize - 1;
            const int64_t b = vm->stackSize - 1 - instruction.value.asI64;

            Word temp = vm->stack[a];
            vm->stack[a] = vm->stack[b];
            vm->stack[b] = temp;

            ++vm->instructionPointer;
            break;
        case INST_RELEASE:
            if (vm->stackSize < 1) return EX_STACK_UNDERFLOW;

            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_IPLUS:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asI64 += vm->stack[vm->stackSize - 1].asI64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_IMINUS:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asI64 -= vm->stack[vm->stackSize - 1].asI64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_IMUL:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asI64 *= vm->stack[vm->stackSize - 1].asI64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_IDIV:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;
            if (vm->stack[vm->stackSize - 1].asI64 == 0) return EX_DIVIDE_BY_ZERO;

            vm->stack[vm->stackSize - 2].asI64 /= vm->stack[vm->stackSize - 1].asI64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_IMOD:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asI64 %= vm->stack[vm->stackSize - 1].asI64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_FPLUS:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asF64 += vm->stack[vm->stackSize - 1].asF64;
            --vm->stackSize;
            ++vm->instructionPointer;
            break;
        case INST_FMINUS:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asF64 -= vm->stack[vm->stackSize - 1].asF64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_FMUL:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asF64 *= vm->stack[vm->stackSize - 1].asF64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_FDIV:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;
            if (vm->stack[vm->stackSize - 1].asF64 == 0.0) return EX_DIVIDE_BY_ZERO;

            vm->stack[vm->stackSize - 2].asF64 /= vm->stack[vm->stackSize - 1].asF64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_FMOD:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asF64 = fmod(vm->stack[vm->stackSize - 2].asF64,
                                                      vm->stack[vm->stackSize - 1].asF64);
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_JUMP:
            vm->instructionPointer = instruction.value.asI64;
            break;
        case INST_JUMP_IF:
            if (vm->stackSize < 1) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 1].asI64 ? vm->instructionPointer = instruction.value.asI64
                                               : ++vm->instructionPointer;
            --vm->stackSize;

            break;
        case INST_RETURN:
            if (vm->stackSize < 1) return EX_STACK_UNDERFLOW;

            vm->instructionPointer = vm->stack[vm->stackSize - 1].asI64;
            --vm->stackSize;

            break;
        case INST_INVOKE:
            if (vm->stackSize >= VM_STACK_CAPACITY) return EX_STACK_OVERFLOW;

            vm->stack[++vm->stackSize].asI64 = (int64_t) vm->instructionPointer + 1;
            vm->instructionPointer = instruction.value.asI64;

            break;
        case INST_NATIVE:
            if (instruction.value.asI64 > (int64_t) vm->nativeFunctionsSize) return EX_ILLEGAL_OPERATION;

            const Exception exception = vm->nativeFunctions[instruction.value.asI64](vm);
            if (exception != EX_OK) return exception;

            ++vm->instructionPointer;
            break;
        case INST_IEQ:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asI64 =
                    vm->stack[vm->stackSize - 1].asI64 == vm->stack[vm->stackSize - 2].asI64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_INEQ:
            if (vm->stackSize < 1) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 1].asI64 = !vm->stack[vm->stackSize - 1].asI64;
            ++vm->instructionPointer;

            break;
        case INST_IGT:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asI64 =
                    vm->stack[vm->stackSize - 1].asI64 > vm->stack[vm->stackSize - 2].asI64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_ILT:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asI64 =
                    vm->stack[vm->stackSize - 1].asI64 < vm->stack[vm->stackSize - 2].asI64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_IGEQ:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asI64 =
                    vm->stack[vm->stackSize - 1].asI64 >= vm->stack[vm->stackSize - 2].asI64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_ILEQ:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asI64 =
                    vm->stack[vm->stackSize - 1].asI64 <= vm->stack[vm->stackSize - 2].asI64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_FEQ:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asI64 =
                    vm->stack[vm->stackSize - 1].asF64 == vm->stack[vm->stackSize - 2].asF64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_FNEQ:
            if (vm->stackSize < 1) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 1].asI64 = !vm->stack[vm->stackSize - 1].asF64;
            ++vm->instructionPointer;

            break;
        case INST_FGT:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asI64 =
                    vm->stack[vm->stackSize - 1].asF64 > vm->stack[vm->stackSize - 2].asF64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_FLT:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asI64 =
                    vm->stack[vm->stackSize - 1].asF64 < vm->stack[vm->stackSize - 2].asF64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_FGEQ:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asI64 =
                    vm->stack[vm->stackSize - 1].asF64 >= vm->stack[vm->stackSize - 2].asF64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_FLEQ:
            if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

            vm->stack[vm->stackSize - 2].asI64 =
                    vm->stack[vm->stackSize - 1].asF64 <= vm->stack[vm->stackSize - 2].asF64;
            --vm->stackSize;
            ++vm->instructionPointer;

            break;
        case INST_HALT:
            vm->halt = 1;
            break;
        case TOTAL_INST_COUNT:
        default:
            return EX_INVALID_INSTRUCTION;
    }

    return EX_OK;
}

static Exception vmExecuteProgram(QuarkVM *vm, int limit, int debug)
{
    if (debug)
    {
        printf("[\033[1;34mINFO\033[0m]: Debugger started.\n");
        VM_EXECUTION_LIMIT < 0 ? printf("[\033[1;34mINFO\033[0m]: Total instructions: %s\n", "unlimited") : printf(
                "[\033[1;34mINFO\033[0m]: Total instructions: %d\n", VM_EXECUTION_LIMIT);
        printf("[\033[1;34mINFO\033[0m]: Type '?' for a list of commands.\n");
    }

    for (int i = 1; limit != 0 && !vm->halt && i < (int) vm->programSize; ++i)
    {
        Exception exception = vmExecuteInstruction(vm);
        if (exception != EX_OK)
        {
            fprintf(stderr, "[\033[1;31mERROR\033[0m]: Error at Op %d (%s): %s\n", i,
                    getInstructionName(vm->program[i].type), exceptionAsCString(exception));
            return exception;
        }

        if (debug)
        {
            printf("Op %d:\n", i);
            printf("  Type: %s\n", getInstructionName(vm->program[i].type));

            if (instructionWithOperand(vm->program[vm->instructionPointer].type))
                printf("  I64: %" PRId64 ", F64: %lf, PTR: %p\n", vm->program[i].value.asI64,
                       vm->program[i].value.asF64, vm->program[i].value.asPtr);

            printf("\n>> ");
            char input[256];

            while (fgets(input, sizeof(input), stdin) != NULL)
            {
                input[strlen(input) - 1] = '\0';
                if (strcmp(input, "?") == 0)
                {
                    printf("[\033[1;34mINFO\033[0m]: Available commands:\n");
                    printf("[\033[1;34mINFO\033[0m]: ?: Print this help message\n");
                    printf("[\033[1;34mINFO\033[0m]: .: Print the current stack\n");
                    printf("[\033[1;34mINFO\033[0m]: !: Exit the debugger\n");
                    printf("[\033[1;34mINFO\033[0m]: Type nothing to continue to the next instruction\n");
                } else if (strcmp(input, ".") == 0) vmDumpStack(stdout, vm);
                else if (strcmp(input, "!") == 0)
                {
                    printf("[\033[1;34mINFO\033[0m]: Exiting debugger...\n");
                    return EXIT_SUCCESS;
                } else if (strcmp(input, "") == 0) break;
                else
                {
                    fprintf(stderr, "[\033[1;31mERROR\033[0m]: Unknown command '%s'.\n", input);
                    printf("[\033[1;34mINFO\033[0m]: Type '?' for a list of commands.\n");
                }

                printf("\n>> ");
            }
        }

        if (limit > 0) --limit;
    }

    if (debug)
        printf("\n[\033[1;34mINFO\033[0m]: Debugger finished with %" PRId64 " executed instructions (excluding kaput).\n",
               vm->instructionPointer);

    return EX_OK;
}

static void vmPushNativeFunc(QuarkVM *vm, NativeVM nativeFunction)
{
    assert(vm->nativeFunctionsSize < VM_CAPACITY && "Number of native functions exceeds VM capacity.");
    vm->nativeFunctions[vm->nativeFunctionsSize++] = nativeFunction;
}

static void vmLoadProgramFromMemory(QuarkVM *quarkVm, Instruction *program, int64_t programSize)
{
    assert(programSize < VM_CAPACITY && "Program size exceeds VM capacity.");
    memcpy(quarkVm->program, program, sizeof(program[0]) * programSize);

    quarkVm->programSize = programSize;
}

static void vmLoadProgramFromFile(QuarkVM *quarkVm, const char *filePath)
{
    FILE *file = fopen(filePath, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "[\033[1;31mERROR\033[0m]: Failed to open file '%s' (%s)\n", filePath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (fseek(file, 0, SEEK_END) < 0)
    {
        fprintf(stderr, "[\033[1;31mERROR\033[0m]: Failed to read file '%s' (%s)\n", filePath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    long fileSize = ftell(file);
    if (fileSize < 0)
    {
        fprintf(stderr, "[\033[1;31mERROR\033[0m]: Failed to read file '%s' (%s)\n", filePath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    assert(fileSize % sizeof(quarkVm->program[0]) == 0 && "Invalid bytecode");
    assert((size_t) fileSize <= VM_CAPACITY * sizeof(quarkVm->program[0]) && "Bytecode too large");

    if (fseek(file, 0, SEEK_SET) < 0)
    {
        fprintf(stderr, "[\033[1;31mERROR\033[0m]: Failed to read file '%s' (%s)\n", filePath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    quarkVm->programSize = (int64_t) fread(quarkVm->program, sizeof(quarkVm->program[0]),
                                           fileSize / sizeof(quarkVm->program[0]), file);

    if (ferror(file))
    {
        fprintf(stderr, "[\033[1;31mERROR\033[0m]: Failed to read file '%s' (%s)\n", filePath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

static void vmSaveProgramToFile(const QuarkVM *vm, const char *filePath)
{
    FILE *file = fopen(filePath, "wb");
    if (file == NULL)
    {
        fprintf(stderr, "[\033[1;31mERROR\033[0m]: Failed to open file '%s' (%s).\n", filePath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    fwrite(vm->program, sizeof(vm->program[0]), vm->programSize, file);
    if (ferror(file))
    {
        fprintf(stderr, "[\033[1;31mERROR\033[0m]: Failed to write to file '%s' (%s).\n", filePath, strerror(errno));
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

static int64_t vmTableFindAddress(const VMTable *table, StringView label)
{
    for (int64_t i = 0; i < table->functionSize; ++i)
        if (sv_equals(table->functions[i].label, label)) return table->functions[i].address;

    fprintf(stderr, "[\033[1;31mERROR\033[0m]: Function '%.*s' does not exist.\n", (int) label.count, label.data);
    exit(EXIT_FAILURE);
}

static void vmTablePushFunction(VMTable *table, StringView label, int64_t address)
{
    assert(table->functionSize < VM_CAPACITY && "Number of functions exceeds VM capacity.");
    table->functions[++table->functionSize] = (Function) {label, address};
}

static void vmTablePushDelayedOperand(VMTable *table, int64_t address, StringView label)
{
    assert(table->delayedOperandSize < VM_CAPACITY && "Number of delayed operands exceeds VM capacity.");
    table->delayedOperands[++table->delayedOperandSize] = (DelayedOperand) {label, address};
}

static Word numberToWord(StringView source)
{
    assert(source.count < VM_CAPACITY && "Number literal exceeds VM capacity.");

    char cString[VM_CAPACITY], *endPtr = NULL;

    memcpy(cString, source.data, source.count);
    cString[source.count] = '\0';

    Word result = {0};
    result.asI64 = strtoll(cString, &endPtr, 10);

    if ((int64_t) (endPtr - cString) != source.count)
    {
        result.asF64 = strtod(cString, &endPtr);
        if ((int64_t) (endPtr - cString) != source.count)
        {
            fprintf(stderr, "[\033[1;31mERROR\033[0m]: Invalid number literal `%s`\n", cString);
            exit(EXIT_FAILURE);
        }
    }

    return result;
}

static void vmParseSource(StringView source, QuarkVM *vm, VMTable *vmTable, const char *inputFilePath)
{
    int lineNumber = 0;
    vm->programSize = 0;

    while (source.count > 0)
    {
        assert(vm->programSize < VM_CAPACITY && "Number of instructions exceeds VM capacity.");
        StringView line = sv_trim(sv_trimByDelimiter(&source, '\n'));
        ++lineNumber;

        if (line.count > 0 && !(line.data[0] == '-' && line.data[1] == '-'))
        {
            StringView token = sv_trimByDelimiter(&line, ' ');

            if (token.count > 0 && token.data[token.count - 1] == ':')
            {
                vmTablePushFunction(vmTable, (StringView) {token.count - 1, token.data}, vm->programSize);
                token = sv_trim(sv_trimByDelimiter(&line, ' '));
            }
            if (token.count > 0)
            {
                StringView operand = sv_trim(sv_trimByDelimiter(&line, '-'));

                if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_PUT))))
                {
                    vm->program[vm->programSize++] = (Instruction) {INST_PUT, numberToWord(operand)};
                } else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_KAPUT))))
                    vm->program[vm->programSize++] = (Instruction) {INST_KAPUT, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_DUP))))
                    vm->program[vm->programSize++] = (Instruction) {INST_DUP, .value.asI64 = sv_toInt(operand)};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_SWAP))))
                    vm->program[vm->programSize++] = (Instruction) {INST_SWAP, .value.asI64 = sv_toInt(operand)};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_RELEASE))))
                    vm->program[vm->programSize++] = (Instruction) {INST_RELEASE, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_IPLUS))))
                    vm->program[vm->programSize++] = (Instruction) {INST_IPLUS, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_IMINUS))))
                    vm->program[vm->programSize++] = (Instruction) {INST_IMINUS, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_IMUL))))
                    vm->program[vm->programSize++] = (Instruction) {INST_IMUL, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_IDIV))))
                    vm->program[vm->programSize++] = (Instruction) {INST_IDIV, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_IMOD))))
                    vm->program[vm->programSize++] = (Instruction) {INST_IMOD, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_FPLUS))))
                    vm->program[vm->programSize++] = (Instruction) {INST_FPLUS, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_FMINUS))))
                    vm->program[vm->programSize++] = (Instruction) {INST_FMINUS, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_FMUL))))
                    vm->program[vm->programSize++] = (Instruction) {INST_FMUL, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_FDIV))))
                    vm->program[vm->programSize++] = (Instruction) {INST_FDIV, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_FMOD))))
                    vm->program[vm->programSize++] = (Instruction) {INST_FMOD, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_JUMP))))
                    if (operand.count > 0 && isdigit(*operand.data))
                        vm->program[vm->programSize++] = (Instruction) {INST_JUMP, .value.asI64 = sv_toInt(operand)};
                    else
                    {
                        vmTablePushDelayedOperand(vmTable, vm->programSize, operand);
                        vm->program[vm->programSize++] = (Instruction) {INST_JUMP, {0}};
                    }
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_JUMP_IF))))
                    if (operand.count > 0 && isdigit(*operand.data))
                        vm->program[vm->programSize++] = (Instruction) {INST_JUMP_IF, .value.asI64 = sv_toInt(operand)};
                    else
                    {
                        vmTablePushDelayedOperand(vmTable, vm->programSize, operand);
                        vm->program[vm->programSize++] = (Instruction) {INST_JUMP_IF, {0}};
                    }
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_RETURN))))
                    vm->program[vm->programSize++] = (Instruction) {INST_RETURN, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_INVOKE))))
                    if (operand.count > 0 && isdigit(*operand.data))
                        vm->program[vm->programSize++] = (Instruction) {INST_INVOKE, .value.asI64 = sv_toInt(operand)};
                    else
                    {
                        vmTablePushDelayedOperand(vmTable, vm->programSize, operand);
                        vm->program[vm->programSize++] = (Instruction) {INST_INVOKE, {0}};
                    }
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_NATIVE))))
                    vm->program[vm->programSize++] = (Instruction) {INST_NATIVE, .value.asI64 = sv_toInt(operand)};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_IEQ))))
                    vm->program[vm->programSize++] = (Instruction) {INST_IEQ, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_INEQ))))
                    vm->program[vm->programSize++] = (Instruction) {INST_INEQ, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_IGT))))
                    vm->program[vm->programSize++] = (Instruction) {INST_IGT, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_ILT))))
                    vm->program[vm->programSize++] = (Instruction) {INST_ILT, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_IGEQ))))
                    vm->program[vm->programSize++] = (Instruction) {INST_IGEQ, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_ILEQ))))
                    vm->program[vm->programSize++] = (Instruction) {INST_ILEQ, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_FGT))))
                    vm->program[vm->programSize++] = (Instruction) {INST_FGT, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_FLT))))
                    vm->program[vm->programSize++] = (Instruction) {INST_FLT, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_FGEQ))))
                    vm->program[vm->programSize++] = (Instruction) {INST_FGEQ, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_FLEQ))))
                    vm->program[vm->programSize++] = (Instruction) {INST_FLEQ, {0}};
                else if (sv_equals(token, sv_cStringAsStringView(getInstructionName(INST_HALT))))
                    vm->program[vm->programSize++] = (Instruction) {INST_HALT, {0}};
                else
                {
                    fprintf(stderr, "[\033[1;31mERROR\033[0m]: (In file `%s`) Invalid instruction `%.*s` on line %d\n",
                            inputFilePath, (int) token.count, token.data, lineNumber);
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    for (int64_t i = 0; i < vmTable->delayedOperandSize; ++i)
        vm->program[vmTable->delayedOperands[i].address].value.asI64 = vmTableFindAddress(vmTable,
                                                                                          vmTable->delayedOperands[i].label);
}
