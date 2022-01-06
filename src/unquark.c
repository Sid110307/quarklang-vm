#define QUARK_VM_COMPILER_IMPLEMENTATION
#include "./compiler.h"

QuarkVM vm = { 0 };

int main(int argc, char** argv)
{
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
					printf("Op %ld: kaput\n", j);
					break;
				case INST_PUT:
					printf("Op %ld: put %ld\n", j, vm.program[j].value);
					break;
				case INST_DUP:
					printf("Op %ld: dup %ld\n", j, vm.program[j].value);
					break;
				case INST_ADD:
					printf("Op %ld: add\n", j);
					break;
				case INST_SUB:
					printf("Op %ld: sub\n", j);
					break;
				case INST_MUL:
					printf("Op %ld: mul\n", j);
					break;
				case INST_DIV:
					printf("Op %ld: div\n", j);
					break;
				case INST_MOD:
					printf("Op %ld: mod\n", j);
					break;
				case INST_JUMP:
					printf("Op %ld: jmp %ld\n", j, vm.program[j].value);
					break;
				case INST_JUMP_IF:
					printf("Op %ld: jif %ld\n", j, vm.program[j].value);
					break;
				case INST_EQ:
					printf("Op %ld: eq\n", j);
					break;
				case INST_GT:
					printf("Op %ld: gt\n", j);
					break;
				case INST_LT:
					printf("Op %ld: lt\n", j);
					break;
				case INST_GEQ:
					printf("Op %ld: ge\n", j);
					break;
				case INST_LEQ:
					printf("Op %ld: le\n", j);
					break;
				case INST_HALT:
					printf("Op %ld: stop\n", j);
					break;
				case INST_PRINT_DEBUG:
					printf("Op %ld: print\n", j);
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

			exit(EXIT_SUCCESS);
		}
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