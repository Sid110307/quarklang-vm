#ifndef QUARK_VM_COMPILER_H_
#define QUARK_VM_COMPILER_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>

#define QUARK_VM_STRINGVIEW_IMPLEMENTATION
#include "stringview.h"

#define ARRAY_CAPACITY(x) (sizeof(x) / sizeof((x)[0]))

#define VM_STACK_CAPACITY 1048576
#define VM_PROGRAM_CAPACITY 1024
#define VM_FUNCTION_CAPACITY 1024
#define VM_DELAYED_OPERANDS_CAPACITY 1024
#define VM_WORD_CAPACITY 1024

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

const char* exceptionAsCstr(Exception exception);

typedef enum
{
	INST_KAPUT = 0,
	INST_PUT,
	INST_DUP,
	INST_SWAP,

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
	INST_PRINT_DEBUG,
	TOTAL_INST_COUNT,
} InstructionType;

const char* getInstructionName(InstructionType type);
int instructionWithOperand(InstructionType type);
const char* instructionTypeAsCstr(InstructionType type);

typedef uint64_t InstructionAddress;

typedef union
{
	InstructionAddress asU64;
	int64_t asI64;
	double asF64;
	void* asPtr;
} Word;

static_assert(sizeof(Word) == 8, "The virtual machine's word size must be 64 bytes.");

typedef struct
{
	InstructionType type;
	Word value;
} Instruction;

typedef struct
{
	Word stack[VM_STACK_CAPACITY];
	InstructionAddress stackSize;

	Instruction program[VM_PROGRAM_CAPACITY];
	InstructionAddress programSize;
	InstructionAddress instructionPointer;

	int halt;
} QuarkVM;

Exception vmExecuteInstruction(QuarkVM* vm);
Exception vmExecuteProgram(QuarkVM* vm, int limit, int debug);
void vmDumpStack(FILE* stream, const QuarkVM* quarkVm);
void vmLoadProgramFromMemory(QuarkVM* quarkVm, Instruction* program, size_t programSize);
void vmLoadProgramFromFile(QuarkVM* quarkVm, const char* filePath);
void vmSaveProgramToFile(const QuarkVM* vm, const char* filePath);

QuarkVM quarkVm = { 0 };

typedef struct
{
	StringView label;
	InstructionAddress address;
} Function;

typedef struct
{
	InstructionAddress address;
	StringView label;
} DelayedOperand;

typedef struct
{
	Function functions[VM_FUNCTION_CAPACITY];
	size_t functionSize;
	DelayedOperand delayedOperands[VM_DELAYED_OPERANDS_CAPACITY];
	size_t delayedOperandSize;
} VMTable;

InstructionAddress vmTableFindAddress(const VMTable* table, StringView label);
void vmTablePushFunction(VMTable* table, StringView label, InstructionAddress address);
void vmTablePushDelayedOperand(VMTable* table, InstructionAddress address, StringView label);

Word numberLiteralToWord(StringView source);
void vmParseSource(StringView source, QuarkVM* vm, VMTable* vmTable);

#endif //! QUARK_VM_COMPILER_H_
#ifdef QUARK_VM_COMPILER_IMPLEMENTATION

const char* exceptionAsCstr(Exception exception)
{
	switch (exception)
	{
	case EX_OK: return "All OK";
	case EX_STACK_OVERFLOW: return "EX_STACK_OVERFLOW";
	case EX_STACK_UNDERFLOW: return "EX_STACK_UNDERFLOW";
	case EX_INVALID_INSTRUCTION: return "EX_INVALID_INSTRUCTION";
	case EX_DIVIDE_BY_ZERO: return "EX_DIVIDE_BY_ZERO";
	case EX_ILLEGAL_INSTRUCTION_ACCESS: return "EX_ILLEGAL_INSTRUCTION_ACCESS";
	case EX_ILLEGAL_OPERATION: return "EX_ILLEGAL_OPERATION";
	default: assert(0 && "[exceptionAsCstr]: Unreachable");
	}
}

const char* getInstructionName(InstructionType type)
{
	switch (type)
	{
	case INST_KAPUT: return "kaput";
	case INST_PUT: return "put";
	case INST_DUP: return "dup";
	case INST_SWAP: return "swap";

	case INST_IPLUS: return "iplus";
	case INST_IMINUS: return "iminus";
	case INST_IMUL: return "imul";
	case INST_IDIV: return "idiv";
	case INST_IMOD: return "imod";

	case INST_FPLUS: return "fplus";
	case INST_FMINUS: return "fminus";
	case INST_FMUL: return "fmul";
	case INST_FDIV: return "fdiv";
	case INST_FMOD: return "fmod";

	case INST_JUMP: return "jmp";
	case INST_JUMP_IF: return "jif";

	case INST_IEQ: return "ieq";
	case INST_INEQ: return "ineq";
	case INST_IGT: return "igt";
	case INST_ILT: return "ilt";
	case INST_IGEQ: return "ige";
	case INST_ILEQ: return "ile";

	case INST_FEQ: return "feq";
	case INST_FNEQ: return "fneq";
	case INST_FGT: return "fgt";
	case INST_FLT: return "flt";
	case INST_FGEQ: return "fge";
	case INST_FLEQ: return "fle";

	case INST_HALT: return "stop";
	case INST_PRINT_DEBUG: return "print";
	case TOTAL_INST_COUNT: default: assert(0 && "[instructionName]: Unreachable");
	}
}

int instructionWithOperand(InstructionType type)
{
	switch (type)
	{
	case INST_KAPUT: return 0;
	case INST_PUT: return 1;
	case INST_DUP: return 1;
	case INST_SWAP: return 1;

	case INST_IPLUS: return 0;
	case INST_IMINUS: return 0;
	case INST_IMUL: return 0;
	case INST_IDIV: return 0;
	case INST_IMOD: return 0;

	case INST_FPLUS: return 0;
	case INST_FMINUS: return 0;
	case INST_FMUL: return 0;
	case INST_FDIV: return 0;
	case INST_FMOD: return 0;

	case INST_JUMP: return 1;
	case INST_JUMP_IF: return 1;

	case INST_IEQ: return 0;
	case INST_INEQ: return 0;
	case INST_IGT: return 0;
	case INST_ILT: return 0;
	case INST_IGEQ: return 0;
	case INST_ILEQ: return 0;

	case INST_FEQ: return 0;
	case INST_FNEQ: return 0;
	case INST_FGT: return 0;
	case INST_FLT: return 0;
	case INST_FGEQ: return 0;
	case INST_FLEQ: return 0;

	case INST_HALT: return 0;
	case INST_PRINT_DEBUG: return 0;
	case TOTAL_INST_COUNT: default: assert(0 && "[instructionWithOperand]: Unreachable");
	}
}

const char* instructionTypeAsCstr(InstructionType type)
{
	switch (type)
	{
	case INST_KAPUT: return "INST_KAPUT";
	case INST_PUT: return "INST_PUT";
	case INST_DUP: return "INST_DUP";
	case INST_SWAP: return "INST_SWAP";

	case INST_IPLUS: return "INST_IPLUS";
	case INST_IMINUS: return "INST_IMINUS";
	case INST_IMUL: return "INST_IMUL";
	case INST_IDIV: return "INST_IDIV";
	case INST_IMOD: return "INST_IMOD";

	case INST_FPLUS: return "INST_FPLUS";
	case INST_FMINUS: return "INST_FMINUS";
	case INST_FMUL: return "INST_FMUL";
	case INST_FDIV: return "INST_FDIV";
	case INST_FMOD: return "INST_FMOD";

	case INST_JUMP: return "INST_JUMP";
	case INST_JUMP_IF: return "INST_JUMP_IF";

	case INST_IEQ: return "INST_IEQ";
	case INST_INEQ: return "INST_INEQ";
	case INST_IGT: return "INST_IGT";
	case INST_ILT: return "INST_ILT";
	case INST_IGEQ: return "INST_IGEQ";
	case INST_ILEQ: return "INST_ILEQ";

	case INST_FEQ: return "INST_FEQ";
	case INST_FNEQ: return "INST_FNEQ";
	case INST_FGT: return "INST_FGT";
	case INST_FLT: return "INST_FLT";
	case INST_FGEQ: return "INST_FGEQ";
	case INST_FLEQ: return "INST_FLEQ";

	case INST_HALT: return "INST_HALT";
	case INST_PRINT_DEBUG: return "INST_PRINT_DEBUG";
	case TOTAL_INST_COUNT: default: assert(0 && "[instructionTypeAsCstr]: Unreachable");
	}
}

Exception vmExecuteInstruction(QuarkVM* vm)
{
	if (vm->instructionPointer >= vm->programSize)
		return EX_ILLEGAL_INSTRUCTION_ACCESS;

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
		if (vm->stackSize - instruction.value.asU64 <= 0) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize] = vm->stack[vm->stackSize - 1 - instruction.value.asU64];
		++vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_SWAP:
		if (instruction.value.asU64 >= vm->stackSize) return EX_STACK_UNDERFLOW;

		const uint64_t a = vm->stackSize - 1;
		const uint64_t b = vm->stackSize - 1 - instruction.value.asU64;

		Word temp = vm->stack[a];
		vm->stack[a] = vm->stack[b];
		vm->stack[b] = temp;

		++vm->instructionPointer;
		break;
	case INST_IPLUS:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 += vm->stack[vm->stackSize - 1].asU64;
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_IMINUS:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 -= vm->stack[vm->stackSize - 1].asU64;
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_IMUL:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 *= vm->stack[vm->stackSize - 1].asU64;
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_IDIV:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;
		if (vm->stack[vm->stackSize - 1].asU64 == 0) return EX_DIVIDE_BY_ZERO;

		vm->stack[vm->stackSize - 2].asU64 /= vm->stack[vm->stackSize - 1].asU64;
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_IMOD:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 %= vm->stack[vm->stackSize - 1].asU64;
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
		// if (vm->stack[vm->stackSize - 1].asF64 == 0) return EX_DIVIDE_BY_ZERO;

		vm->stack[vm->stackSize - 2].asF64 /= vm->stack[vm->stackSize - 1].asF64;
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_FMOD:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asF64 = fmod(vm->stack[vm->stackSize - 2].asF64, vm->stack[vm->stackSize - 1].asF64);
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_JUMP:
		vm->instructionPointer = instruction.value.asU64;
		break;
	case INST_JUMP_IF:
		if (vm->stackSize < 1) return EX_STACK_UNDERFLOW;

		if (vm->stack[vm->stackSize - 1].asU64) vm->instructionPointer = instruction.value.asU64;
		else ++vm->instructionPointer;

		--vm->stackSize;
		break;
	case INST_IEQ:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 = (vm->stack[vm->stackSize - 1].asU64 == vm->stack[vm->stackSize - 2].asU64);
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_INEQ:
		if (vm->stackSize < 1) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 1].asU64 = !vm->stack[vm->stackSize - 1].asU64;
		++vm->instructionPointer;
		break;
	case INST_IGT:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 = (vm->stack[vm->stackSize - 1].asU64 > vm->stack[vm->stackSize - 2].asU64);
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_ILT:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 = (vm->stack[vm->stackSize - 1].asU64 < vm->stack[vm->stackSize - 2].asU64);
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_IGEQ:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 = (vm->stack[vm->stackSize - 1].asU64 >= vm->stack[vm->stackSize - 2].asU64);
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_ILEQ:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 = (vm->stack[vm->stackSize - 1].asU64 <= vm->stack[vm->stackSize - 2].asU64);
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_FEQ:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 = (vm->stack[vm->stackSize - 1].asF64 == vm->stack[vm->stackSize - 2].asF64);
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_FNEQ:
		if (vm->stackSize < 1) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 1].asU64 = !vm->stack[vm->stackSize - 1].asF64;
		++vm->instructionPointer;
		break;
	case INST_FGT:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 = (vm->stack[vm->stackSize - 1].asF64 > vm->stack[vm->stackSize - 2].asF64);
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_FLT:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 = (vm->stack[vm->stackSize - 1].asF64 < vm->stack[vm->stackSize - 2].asF64);
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_FGEQ:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 = (vm->stack[vm->stackSize - 1].asF64 >= vm->stack[vm->stackSize - 2].asF64);
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_FLEQ:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 = (vm->stack[vm->stackSize - 1].asF64 <= vm->stack[vm->stackSize - 2].asF64);
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_HALT:
		vm->halt = 1;
		break;
	case INST_PRINT_DEBUG:
		if (vm->stackSize < 1) return EX_STACK_UNDERFLOW;

		fprintf(stdout, "U64: %lu, I64: %ld, F64: %lf, PTR: %p\n",
			vm->stack[vm->stackSize - 1].asU64,
			vm->stack[vm->stackSize - 1].asI64,
			vm->stack[vm->stackSize - 1].asF64,
			vm->stack[vm->stackSize - 1].asPtr);

		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case TOTAL_INST_COUNT:
	default:
		return EX_INVALID_INSTRUCTION;
	}

	return EX_OK;
}

Exception vmExecuteProgram(QuarkVM* vm, int limit, int debug)
{
	if (debug)
	{
		printf("[\033[1;34mINFO\033[0m]: Debugger started.\n");
		if (VM_EXECUTION_LIMIT < 0) printf("[\033[1;34mINFO\033[0m]: Total instructions: %s\n", "Unlimited");
		else printf("[\033[1;34mINFO\033[0m]: Total instructions: %d\n", VM_EXECUTION_LIMIT);
		printf("[\033[1;34mINFO\033[0m]: Type '?' for a list of commands.\n");
	}

	for (int i = 1; limit != 0 && !vm->halt; ++i)
	{
		if (debug)
		{
			printf("Op %d:\n", i);
			printf("  Type: %s\n", instructionTypeAsCstr(quarkVm.program[i].type));

			if (instructionWithOperand(quarkVm.program[i].type) == 1)
				printf("  Value: %ld\n", quarkVm.program[i].value.asU64);

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
				}
				else if (strcmp(input, ".") == 0)
					vmDumpStack(stdout, &quarkVm);
				else if (strcmp(input, "!") == 0)
				{
					printf("[\033[1;34mINFO\033[0m]: Exiting debugger...\n");
					return EXIT_SUCCESS;
				}
				else if (strcmp(input, "") == 0)
					break;
				else
				{
					fprintf(stderr, "[\033[1;31mERROR\033[0m]: Unknown command '%s'.\n", input);
					printf("[\033[1;34mINFO\033[0m]: Type '?' for a list of commands.\n");
				}

				printf("\n>> ");
			}
		}

		Exception exception = vmExecuteInstruction(vm);
		if (exception != EX_OK)
		{
			fprintf(stderr, "[\033[1;31mERROR\033[0m]: Error at Op %d: %s\n", i, exceptionAsCstr(exception));
			return exception;
		}

		if (quarkVm.halt) printf("[\033[1;34mINFO\033[0m]: Program halted at Op %d.\n", i);
		if (limit > 0) --limit;
	}

	if (debug) printf("\n[\033[1;34mINFO\033[0m]: Debugger finished with %ld executed instructions (excluding INST_KAPUT).\n", quarkVm.instructionPointer);

	return EX_OK;
}

void vmDumpStack(FILE* stream, const QuarkVM* quarkVm)
{
	fprintf(stream, "Stack:");
	if (quarkVm->stackSize > 0)
	{
		fprintf(stream, "\n");
		for (InstructionAddress i = 0; i < quarkVm->stackSize; ++i)
			fprintf(stream, "  U64: %lu, I64: %ld, F64: %lf, PTR: %p\n", quarkVm->stack[i].asU64, quarkVm->stack[i].asI64, quarkVm->stack[i].asF64, quarkVm->stack[i].asPtr);
	}
	else fprintf(stream, " [empty]\n");
}

void vmLoadProgramFromMemory(QuarkVM* quarkVm, Instruction* program, size_t programSize)
{
	assert(programSize <= VM_PROGRAM_CAPACITY);
	memcpy(quarkVm->program, program, sizeof(program[0]) * programSize);

	quarkVm->programSize = programSize;
}

void vmLoadProgramFromFile(QuarkVM* quarkVm, const char* filePath)
{
	FILE* file = fopen(filePath, "rb");
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

	assert((fileSize % sizeof(quarkVm->program[0]) == 0) && "Unrecognized bytecode");
	assert((size_t)fileSize <= VM_PROGRAM_CAPACITY * sizeof(quarkVm->program[0]));

	if (fseek(file, 0, SEEK_SET) < 0)
	{
		fprintf(stderr, "[\033[1;31mERROR\033[0m]: Failed to read file '%s' (%s)\n", filePath, strerror(errno));
		exit(EXIT_FAILURE);
	}

	quarkVm->programSize = fread(quarkVm->program, sizeof(quarkVm->program[0]), fileSize / sizeof(quarkVm->program[0]), file);

	if (ferror(file))
	{
		fprintf(stderr, "[\033[1;31mERROR\033[0m]: Failed to read file '%s' (%s)\n", filePath, strerror(errno));
		exit(EXIT_FAILURE);
	}

	fclose(file);
}

void vmSaveProgramToFile(const QuarkVM* vm, const char* filePath)
{
	FILE* file = fopen(filePath, "wb");
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

InstructionAddress vmTableFindAddress(const VMTable* table, StringView label)
{
	for (size_t i = 0; i < table->functionSize; ++i)
		if (sv_equals(table->functions[i].label, label))
			return table->functions[i].address;

	fprintf(stderr, "[\033[1;31mERROR\033[0m]: Function '%.*s' does not exist.\n", (int)label.count, label.data);
	exit(EXIT_FAILURE);
}

void vmTablePushFunction(VMTable* table, StringView label, InstructionAddress address)
{
	assert(table->functionSize < VM_FUNCTION_CAPACITY);
	table->functions[table->functionSize++] = (Function)
	{
		.label = label,
			.address = address,
	};
}

void vmTablePushDelayedOperand(VMTable* table, InstructionAddress address, StringView label)
{
	assert(table->delayedOperandSize < VM_DELAYED_OPERANDS_CAPACITY);
	table->delayedOperands[table->delayedOperandSize++] = (DelayedOperand)
	{
		.address = address,
			.label = label,
	};
}

Word numberLiteralToWord(StringView source)
{
	assert(source.count < VM_WORD_CAPACITY);
	char cstr[source.count + 1];
	char* endPtr = 0;

	memcpy(cstr, source.data, source.count);
	cstr[source.count] = '\0';

	Word result = { 0 };

	result.asU64 = strtoull(cstr, &endPtr, 10);
	if ((size_t)(endPtr - cstr) != source.count)
	{
		result.asF64 = strtod(cstr, &endPtr);
		if ((size_t)(endPtr - cstr) != source.count)
		{
			fprintf(stderr, "[\033[1;31mERROR\033[0m]: Invalid number literal `%s`\n", cstr);
			exit(EXIT_FAILURE);
		}
	}

	return result;
}

void vmParseSource(StringView source, QuarkVM* vm, VMTable* vmTable)
{
	vm->programSize = 0;

	while (source.count > 0)
	{
		assert(vm->programSize < VM_PROGRAM_CAPACITY);
		StringView line = sv_trim(sv_trimByDelimeter(&source, '\n'));

		if (line.count > 0 && !(line.data[0] == '-' && line.data[1] == '-'))
		{
			StringView instructionName = sv_trimByDelimeter(&line, ' ');
			StringView operand = sv_trim(sv_trimByDelimeter(&line, '-'));

			if (instructionName.count > 0 && instructionName.data[instructionName.count - 1] == ':')
			{
				StringView function =
				{
					.count = instructionName.count - 1,
						.data = instructionName.data,
				};

				vmTablePushFunction(vmTable, function, vm->programSize);
			}
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_PUT))))
			{
				vm->program[vm->programSize++] = (Instruction)
				{
					.type = INST_PUT,
					.value = numberLiteralToWord(operand),
				};
			}
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_KAPUT))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_KAPUT,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_DUP))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_DUP,
				.value =
					{
						.asI64 = sv_toInt(operand)
					},
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_SWAP))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_SWAP,
				.value =
					{
						.asI64 = sv_toInt(operand)
					},
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_IPLUS))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_IPLUS,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_IMINUS))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_IMINUS,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_IMUL))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_IMUL,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_IDIV))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_IDIV,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_IMOD))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_IMOD,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_FPLUS))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_FPLUS,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_FMINUS))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_FMINUS,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_FMUL))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_FMUL,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_FDIV))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_FDIV,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_FMOD))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_FMOD,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_JUMP))))
			{
				if (operand.count > 0 && isdigit(*operand.data))
				{
					vm->program[vm->programSize++] = (Instruction)
					{
						.type = INST_JUMP,
						.value =
							{
								.asI64 = sv_toInt(operand)
							},
					};
				}
				else
				{
					vmTablePushDelayedOperand(vmTable, vm->programSize, operand);
					vm->program[vm->programSize++] = (Instruction)
					{
						.type = INST_JUMP,
					};
				}
			}
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_JUMP_IF))))
			{
				if (operand.count > 0 && isdigit(*operand.data))
				{
					vm->program[vm->programSize++] = (Instruction)
					{
						.type = INST_JUMP_IF,
						.value =
							{
								.asI64 = sv_toInt(operand)
							},
					};
				}
				else
				{
					vmTablePushDelayedOperand(vmTable, vm->programSize, operand);
					vm->program[vm->programSize++] = (Instruction)
					{
						.type = INST_JUMP_IF,
					};
				}
			}
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_IEQ))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_IEQ,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_INEQ))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_INEQ,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_IGT))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_IGT,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_ILT))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_ILT,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_IGEQ))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_IGEQ,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_ILEQ))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_ILEQ,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_FGT))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_FGT,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_FLT))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_FLT,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_FGEQ))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_FGEQ,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_FLEQ))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_FLEQ,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_HALT))))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_HALT,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView(getInstructionName(INST_PRINT_DEBUG))))
			{
				vm->program[vm->programSize++] = (Instruction)
				{
					.type = INST_PRINT_DEBUG,
				};
			}
			else
			{
				fprintf(stderr, "[\033[1;31mERROR\033[0m]: Invalid instruction '%.*s'\n", (int)instructionName.count, instructionName.data);
				exit(EXIT_FAILURE);
			}
		}
	}

	for (size_t i = 0; i < vmTable->delayedOperandSize; ++i)
		vm->program[vmTable->delayedOperands[i].address].value.asU64 = (InstructionAddress)vmTableFindAddress(vmTable, vmTable->delayedOperands[i].label);
}

#endif //! QUARK_VM_COMPILER_IMPLEMENTATION
