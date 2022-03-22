#define QUARK_VM_COMPILER_IMPLEMENTATION
#include "./include/compiler.h"

#include <stdbool.h>

QuarkVM vm = { 0 };

int main(int argc, char** argv)
{
	bool isRaw = false;

	if (argc < 2)
	{
		printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);
		printf("[\033[1;34mINFO\033[0m]: Type --help or -h for more information\n");
		exit(EXIT_FAILURE);
	}

	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--file") == 0 || strcmp(argv[i], "-f") == 0)
		{
			const char* inputFilePath = argv[++i];

			if (inputFilePath == NULL)
			{
				fprintf(stderr, "[\033[1;31mERROR\033[0m]: Missing input file\n");
				printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);
				exit(EXIT_FAILURE);
			}

			vmLoadProgramFromFile(&vm, inputFilePath);

			for (Word j = 0; j < vm.programSize; ++j)
			{
				switch (vm.program[j].type)
				{
				case INST_KAPUT:
					printf(isRaw == false ? "Op \033[1;34m%ld\033[0m: kaput\n" : "kaput\n", j);
					break;
				case INST_PUT:
					printf(isRaw == false ? "Op \033[1;32m%ld\033[0m: put %ld\n" : "put %ld\n", j, vm.program[j].value);
					break;
				case INST_DUP:
					printf(isRaw == false ? "Op \033[1;32m%ld\033[0m: dup %ld\n" : "dup %ld\n", j, vm.program[j].value);
					break;
				case INST_ADD:
					printf(isRaw == false ? "Op \033[1;34m%ld\033[0m: plus %ld\n" : "plus %ld\n", j, vm.program[j].value);
					break;
				case INST_SUB:
					printf(isRaw == false ? "Op \033[1;34m%ld\033[0m: minus %ld\n" : "minus %ld\n", j, vm.program[j].value);
					break;
				case INST_MUL:
					printf(isRaw == false ? "Op \033[1;34m%ld\033[0m: mul %ld\n" : "mul %ld\n", j, vm.program[j].value);
					break;
				case INST_DIV:
					printf(isRaw == false ? "Op \033[1;34m%ld\033[0m: div %ld\n" : "div %ld\n", j, vm.program[j].value);
					break;
				case INST_MOD:
					printf(isRaw == false ? "Op \033[1;34m%ld\033[0m: mod %ld\n" : "mod %ld\n", j, vm.program[j].value);
					break;
				case INST_JUMP:
					printf(isRaw == false ? "Op \033[1;32m%ld\033[0m: jmp %ld\n" : "jmp %ld\n", j, vm.program[j].value);
					break;
				case INST_JUMP_IF:
					printf(isRaw == false ? "Op \033[1;32m%ld\033[0m: jif %ld\n" : "jif %ld\n", j, vm.program[j].value);
					break;
				case INST_EQ:
					printf(isRaw == false ? "Op \033[1;32m%ld\033[0m: eq %ld\n" : "eq %ld\n", j, vm.program[j].value);
					break;
				case INST_GT:
					printf(isRaw == false ? "Op \033[1;32m%ld\033[0m: gt %ld\n" : "gt %ld\n", j, vm.program[j].value);
					break;
				case INST_LT:
					printf(isRaw == false ? "Op \033[1;32m%ld\033[0m: lt %ld\n" : "lt %ld\n", j, vm.program[j].value);
					break;
				case INST_GEQ:
					printf(isRaw == false ? "Op \033[1;32m%ld\033[0m: ge %ld\n" : "ge %ld\n", j, vm.program[j].value);
					break;
				case INST_LEQ:
					printf(isRaw == false ? "Op \033[1;32m%ld\033[0m: le %ld\n" : "le %ld\n", j, vm.program[j].value);
					break;
				case INST_HALT:
					printf(isRaw == false ? "Op \033[1;34m%ld\033[0m: stop\n" : "stop\n", j);
					break;
				case INST_PRINT_DEBUG:
					printf(isRaw == false ? "Op \033[1;34m%ld\033[0m: print\n" : "print\n", j);
					break;
				default:
					fprintf(stderr, "[\033[1;31mERROR\033[0m]: Unknown instruction type: %d\n", vm.program[j].type);
					exit(EXIT_FAILURE);
				}
			}
		}
		else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
		{
			printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);
			printf("[\033[1;34mINFO\033[0m]: Required Parameters:\n");
			printf("[\033[1;34mINFO\033[0m]:   --file <file> | -f <file>: Specify the file to decompile\n");
			printf("[\033[1;34mINFO\033[0m]: Optional Parameters:\n");
			printf("[\033[1;34mINFO\033[0m]:   --help        | -h: Print this help message and exit\n");
			printf("[\033[1;34mINFO\033[0m]:   --raw: Print raw text\n");

			exit(EXIT_SUCCESS);
		}
		else if (strcmp(argv[i], "--raw") == 0) isRaw = true;
		else
		{
			fprintf(stderr, "[\033[1;31mERROR\033[0m]: Unknown option '%s'\n",
				argv[i]);
			printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);
			printf("[\033[1;34mINFO\033[0m]: Type --help or -h for more information\n");
			exit(EXIT_FAILURE);
		}
	}

	return 0;
}