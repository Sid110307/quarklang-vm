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

			for (InstructionAddress j = 0; j < vm.programSize; ++j)
			{
				if (!isRaw) printf("Op \033[1;34m%ld\033[0m: %s", j, getInstructionName(vm.program[j].type));
				else printf("%s", getInstructionName(vm.program[j].type));

				if (instructionWithOperand(vm.program[j].type))
					printf(" %ld", vm.program[j].value.asI64);

				printf("\n");

				// fprintf(stderr, "[\033[1;31mERROR\033[0m]: Unknown instruction type: %d\n", vm.program[j].type);
				// exit(EXIT_FAILURE);
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
