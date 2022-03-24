#ifndef QUARK_VM_COMPILER_H_
#define QUARK_VM_COMPILER_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#define VM_STACK_CAPACITY 1048576
#define ARRAY_CAPACITY(x) (sizeof(x) / sizeof((x)[0]))

#define VM_PROGRAM_CAPACITY 1024
#define VM_FUNCTION_CAPACITY 1024
#define VM_DELAYED_OPERANDS_CAPACITY 1024

int VM_EXECUTION_LIMIT = 64;

typedef enum
{
	EX_OK = 0,
	EX_STACK_OVERFLOW,
	EX_STACK_UNDERFLOW,
	EX_INVALID_INSTRUCTION,
	EX_DIVIDE_BY_ZERO,
	EX_ILLEGAL_INSTRUCTION_ACCESS,
	EX_ILLEGAL_OPERATION,
} Exception;

const char* exceptionAsCstr(Exception exception);

typedef enum
{
	INST_KAPUT = 0,
	INST_PUT,
	INST_DUP,
	INST_ADD,
	INST_SUB,
	INST_MUL,
	INST_DIV,
	INST_MOD,
	INST_JUMP,
	INST_JUMP_IF,
	INST_EQ,
	INST_GT,
	INST_LT,
	INST_GEQ,
	INST_LEQ,
	INST_HALT,
	INST_PRINT_DEBUG,
} InstructionType;

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

// TODO: Replace instruction macros with functions
#define PUT_INSTRUCTION(val) { .type = INST_PUT, .value = val }
#define DUP_INSTRUCTION(addr) { .type = INST_DUP, .value = addr }
#define ADD_INSTRUCTION { .type = INST_ADD }
#define SUB_INSTRUCTION { .type = INST_SUB }
#define MUL_INSTRUCTION { .type = INST_MUL }
#define DIV_INSTRUCTION { .type = INST_DIV }
#define MOD_INSTRUCTION { .type = INST_MOD }
#define JUMP_INSTRUCTION(addr) { .type = INST_JUMP, .value = addr }
#define JUMP_IF_INSTRUCTION(addr) { .type = INST_JUMP_IF, .value = addr }
#define EQ_INSTRUCTION { .type = INST_EQ }
#define GT_INSTRUCTION { .type = INST_GT }
#define LT_INSTRUCTION { .type = INST_LT }
#define GEQ_INSTRUCTION { .type = INST_GEQ }
#define LEQ_INSTRUCTION { .type = INST_LEQ }
#define HALT_INSTRUCTION { .type = INST_HALT }

Exception vmExecuteInstruction(QuarkVM* vm);
Exception vmExecuteProgram(QuarkVM* vm, int limit, int printOps, int debug);
void vmDumpStack(FILE* stream, const QuarkVM* quarkVm);
void vmLoadProgramFromMemory(QuarkVM* quarkVm, Instruction* program, size_t programSize);
void vmLoadProgramFromFile(QuarkVM* quarkVm, const char* filePath);
void vmSaveProgramToFile(const QuarkVM* vm, const char* filePath);

QuarkVM quarkVm = { 0 };

typedef struct
{
	size_t count;
	const char* data;
} StringView;

StringView sv_cstrAsStringView(const char* cstr);
StringView sv_trimStart(StringView sv);
StringView sv_trimEnd(StringView sv);
StringView sv_trim(StringView sv);
StringView sv_trimByDelimeter(StringView* sv, char delimeter);
int sv_equals(StringView a, StringView b);
int sv_toInt(StringView sv);
StringView sv_readFile(const char* filePath);

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

void vmParseSource(StringView source, QuarkVM* vm, VMTable* vmTable);

#endif // QUARK_VM_COMPILER_H_
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

const char* instructionTypeAsCstr(InstructionType type)
{
	switch (type)
	{
	case INST_KAPUT: return "INST_KAPUT";
	case INST_PUT: return "INST_PUT";
	case INST_DUP: return "INST_DUP";
	case INST_ADD: return "INST_ADD";
	case INST_SUB: return "INST_SUB";
	case INST_MUL: return "INST_MUL";
	case INST_DIV: return "INST_DIV";
	case INST_MOD: return "INST_MOD";
	case INST_JUMP: return "INST_JUMP";
	case INST_JUMP_IF: return "INST_JUMP_IF";
	case INST_EQ: return "INST_EQ";
	case INST_GT: return "INST_GT";
	case INST_LT: return "INST_LT";
	case INST_GEQ: return "INST_GEQ";
	case INST_LEQ: return "INST_LEQ";
	case INST_HALT: return "INST_HALT";
	case INST_PRINT_DEBUG: return "INST_PRINT_DEBUG";
	default: assert(0 && "[instructionTypeAsCstr]: Unreachable");
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
	case INST_ADD:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 += vm->stack[vm->stackSize - 1].asU64;
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_SUB:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 -= vm->stack[vm->stackSize - 1].asU64;
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_MUL:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 *= vm->stack[vm->stackSize - 1].asU64;
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_DIV:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;
		if (vm->stack[vm->stackSize - 1].asU64 == 0) return EX_DIVIDE_BY_ZERO;

		vm->stack[vm->stackSize - 2].asU64 /= vm->stack[vm->stackSize - 1].asU64;
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_MOD:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 %= vm->stack[vm->stackSize - 1].asU64;
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_JUMP:
		vm->instructionPointer = instruction.value.asU64;
		break;
	case INST_JUMP_IF:
		if (vm->stackSize < 1) return EX_STACK_UNDERFLOW;

		if (vm->stack[vm->stackSize - 1].asU64)
		{
			vm->stackSize--;
			vm->instructionPointer = instruction.value.asU64;
		}
		else vm->instructionPointer++;

		break;
	case INST_EQ:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 = (vm->stack[vm->stackSize - 2].asU64 == vm->stack[vm->stackSize - 1].asU64);
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_GT:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 = (vm->stack[vm->stackSize - 2].asU64 > vm->stack[vm->stackSize - 1].asU64);
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_LT:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 = (vm->stack[vm->stackSize - 2].asU64 < vm->stack[vm->stackSize - 1].asU64);
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_GEQ:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 = (vm->stack[vm->stackSize - 2].asU64 >= vm->stack[vm->stackSize - 1].asU64);
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_LEQ:
		if (vm->stackSize < 2) return EX_STACK_UNDERFLOW;

		vm->stack[vm->stackSize - 2].asU64 = (vm->stack[vm->stackSize - 2].asU64 <= vm->stack[vm->stackSize - 1].asU64);
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	case INST_HALT:
		vm->halt = 1;
		break;
	case INST_PRINT_DEBUG:
		if (vm->stackSize < 1) return EX_STACK_UNDERFLOW;

		printf("%lu\n", vm->stack[vm->stackSize - 1].asU64);
		--vm->stackSize;
		++vm->instructionPointer;
		break;
	default:
		return EX_INVALID_INSTRUCTION;
	}

	return EX_OK;
}

Exception vmExecuteProgram(QuarkVM* vm, int limit, int printOps, int debug)
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
		if (printOps || debug)
		{
			printf("Op %d:\n", i);
			printf("  Type: %s\n", instructionTypeAsCstr(quarkVm.program[i].type));

			if (quarkVm.program[i].type == INST_PUT ||
				quarkVm.program[i].type == INST_DUP ||
				quarkVm.program[i].type == INST_JUMP ||
				quarkVm.program[i].type == INST_JUMP_IF)
				printf("  Value: %ld\n", quarkVm.program[i].value.asU64);
		}

		if (debug)
		{
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

		if (limit != -1) --limit;
	}

	vmDumpStack(stdout, &quarkVm);
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

StringView sv_cstrAsStringView(const char* cstr)
{
	return (StringView)
	{
		.count = strlen(cstr),
			.data = cstr,
	};
}

StringView sv_trimStart(StringView sv)
{
	size_t i = 0;
	while (i < sv.count && isspace(sv.data[i])) i += 1;

	return (StringView)
	{
		.count = sv.count - i,
			.data = sv.data + i,
	};
}

StringView sv_trimEnd(StringView sv)
{
	size_t i = 0;
	while (i < sv.count && isspace(sv.data[sv.count - 1 - i])) i += 1;

	return (StringView)
	{
		.count = sv.count - i,
			.data = sv.data,
	};
}

StringView sv_trim(StringView sv)
{
	return sv_trimStart(sv_trimEnd(sv));
}

StringView sv_trimByDelimeter(StringView* sv, char delimeter)
{
	size_t i = 0;
	while (i < sv->count && sv->data[i] != delimeter) i += 1;

	StringView result =
	{
		.count = i,
			.data = sv->data,
	};

	if (i < sv->count)
	{
		sv->count -= i + 1;
		sv->data += i + 1;
	}
	else
	{
		sv->count -= i;
		sv->data += i;
	}

	return result;
}

int sv_equals(StringView a, StringView b)
{
	if (a.count != b.count) return 0;
	else return memcmp(a.data, b.data, a.count) == 0;
}

int sv_toInt(StringView sv)
{
	int result = 0;

	for (size_t i = 0; i < sv.count && isdigit(sv.data[i]); ++i)
		result = result * 10 + sv.data[i] - '0';

	return result;
}

StringView sv_readFile(const char* filePath)
{
	FILE* file = fopen(filePath, "r");
	if (file == NULL)
	{
		fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not open file '%s' (%s)\n", filePath, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (fseek(file, 0, SEEK_END) < 0)
	{
		fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not read file '%s' (%s)\n", filePath, strerror(errno));
		exit(EXIT_FAILURE);
	}

	long fileSize = ftell(file);
	if (fileSize < 0)
	{
		fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not read file '%s' (%s)\n", filePath, strerror(errno));
		exit(EXIT_FAILURE);
	}

	char* fileContent = malloc(fileSize + 1);
	if (fileContent == NULL)
	{
		fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not allocate memory for file '%s' (%s)\n", filePath, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (fseek(file, 0, SEEK_SET) < 0)
	{
		fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not read file '%s' (%s)\n", filePath, strerror(errno));
		exit(EXIT_FAILURE);
	}

	size_t readBytes = fread(fileContent, 1, fileSize, file);
	if (ferror(file))
	{
		fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not read file '%s' (%s)\n", filePath, strerror(errno));
		exit(EXIT_FAILURE);
	}

	fileContent[fileSize] = '\0';

	fclose(file);
	return (StringView)
	{
		.count = readBytes,
			.data = fileContent,
	};
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
			else if (sv_equals(instructionName, sv_cstrAsStringView("put")))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_PUT,
				.value =
					{
						.asI64 = sv_toInt(operand)
					},
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView("kaput")))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_KAPUT,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView("dup")))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_DUP,
				.value =
					{
						.asI64 = sv_toInt(operand)
					},
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView("plus")))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_ADD,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView("minus")))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_SUB,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView("mul")))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_MUL,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView("div")))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_DIV,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView("mod")))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_MOD,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView("jmp")))
			{
				vmTablePushDelayedOperand(vmTable, vm->programSize, operand);
				vm->program[vm->programSize++] = (Instruction)
				{
					.type = INST_JUMP,
				};
			}
			else if (sv_equals(instructionName, sv_cstrAsStringView("jif")))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_JUMP_IF,
				.value =
					{
						.asI64 = sv_toInt(operand)
					},
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView("eq")))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_EQ,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView("gt")))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_GT,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView("lt")))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_LT,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView("ge")))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_GEQ,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView("le")))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_LEQ,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView("stop")))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_HALT,
			};
			else if (sv_equals(instructionName, sv_cstrAsStringView("print")))
				vm->program[vm->programSize++] = (Instruction)
			{
				.type = INST_PRINT_DEBUG,
			};
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

#endif
