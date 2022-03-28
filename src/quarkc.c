#define QUARK_VM_COMPILER_IMPLEMENTATION
#include "include/compiler.h"

int main(int argc, char** argv)
{
	int debug = 0;
	int limit = -1;
	int stepDebug = 0;

	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			if (strcmp(argv[i], "--limit") == 0 || strcmp(argv[i], "-l") == 0)
			{
				if (argv[++i] == NULL)
				{
					printf("[\033[1;34mINFO\033[0m]: Current limit: %d\n", VM_EXECUTION_LIMIT);
					printf("[\033[1;34mINFO\033[0m]: Change the limit with --limit <number> or -l <number>.\n");
					exit(EXIT_FAILURE);
				}

				VM_EXECUTION_LIMIT = atoi(argv[i]);
			}
			else if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0)
				debug = 1;
			else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
			{
				printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);
				printf("[\033[1;34mINFO\033[0m]: Required Parameters:\n");
				printf("[\033[1;34mINFO\033[0m]:   --file <file>  | -f <file>: Specify the file to compile\n");
				printf("[\033[1;34mINFO\033[0m]: Optional Parameters:\n");
				printf("[\033[1;34mINFO\033[0m]:   --limit <size> | -l <size>: Specify the maximum number of instructions to run. (default: %d)\n", VM_EXECUTION_LIMIT);
				printf("[\033[1;34mINFO\033[0m]:   --debug        | -d: Start an interactive debugger\n");
				printf("[\033[1;34mINFO\033[0m]:   --step         | -s: Step through the program\n");
				printf("[\033[1;34mINFO\033[0m]:   --help         | -h: Print this help message and exit\n");

				exit(EXIT_SUCCESS);
			}
			else if (strcmp(argv[i], "--file") == 0 || strcmp(argv[i], "-f") == 0)
			{
				const char* inputFilePath = argv[++i];

				if (inputFilePath == NULL)
				{
					fprintf(stderr, "[\033[1;31mERROR\033[0m]: Missing input file\n");
					printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);
					exit(EXIT_FAILURE);
				}

				vmLoadProgramFromFile(&quarkVm, inputFilePath);

				if (!stepDebug)
				{
					Exception exception = vmExecuteProgram(&quarkVm, VM_EXECUTION_LIMIT, debug);
					if (exception != EX_OK)
					{
						// vmDumpStack(stdout, &quarkVm);
						return EXIT_FAILURE;
					}
				}
				else
				{
					while (limit != 0 && !quarkVm.halt)
					{
						vmDumpStack(stdout, &quarkVm);
						getchar();

						Exception exception = vmExecuteInstruction(&quarkVm);
						if (exception != EX_OK)
						{
							fprintf(stderr, "[\033[1;31mERROR\033[0m]: %s\n", exceptionAsCstr(exception));
							return EXIT_FAILURE;
						}

						if (limit > 0) --limit;
					}
				}

				return EXIT_SUCCESS;
			}
			else if (strcmp(argv[i], "--step") == 0 || strcmp(argv[i], "-s") == 0) stepDebug = 1;
			else
			{
				fprintf(stderr, "[\033[1;31mERROR\033[0m]: Unknown argument: %s\n", argv[i]);
				printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);
				exit(EXIT_FAILURE);
			}
		}
	}
	else
	{
		printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
