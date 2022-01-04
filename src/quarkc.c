#include "./compiler.c"

int main(int argc, char** argv)
{
	int printOps = 0;
	int debug = 0;

	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			if (strcmp(argv[i], "--stack") == 0 || strcmp(argv[i], "-s") == 0) VM_EXECUTION_LIMIT = atoi(argv[++i]);
			else if (strcmp(argv[i], "--print-ops") == 0 || strcmp(argv[i], "-p") == 0)
				printOps = 1;
			else if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0)
				debug = 1;
			else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
			{
				printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);
				printf("[\033[1;34mINFO\033[0m]: Required Parameters:\n");
				printf("[\033[1;34mINFO\033[0m]:   --file <file>  | -f <file>: Specify the file to compile\n");
				printf("[\033[1;34mINFO\033[0m]: Optional Parameters:\n");
				printf("[\033[1;34mINFO\033[0m]:   --stack <size> | -s <size>: Specify the maximum stack size i.e. the maximum number of instructions. (default: %d)\n", VM_EXECUTION_LIMIT);
				printf("[\033[1;34mINFO\033[0m]:   --print-ops    | -p: Print the current instruction and its value\n");
				printf("[\033[1;34mINFO\033[0m]:   --debug        | -d: [\033[0;33mBETA\033[0m] Start an interactive debugger\n");
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

				if (debug)
				{
					printf("[\033[1;34mINFO\033[0m]: Debugger started for file '%s'.\n", inputFilePath);
					printf("[\033[1;34mINFO\033[0m]: Total instructions: %d\n", VM_EXECUTION_LIMIT);
					printf("[\033[1;34mINFO\033[0m]: Type '?' for a list of commands.\n");
				}

				for (int i = 0; i < VM_EXECUTION_LIMIT && !quarkVm.halt; ++i)
				{
					if (printOps || debug)
					{
						printf("Op %d:\n", i + 1);
						printf("  Type: %s\n", instructionTypeAsCstr(quarkVm.program[i].type));

						if (quarkVm.program[i].type == INST_PUT ||
							quarkVm.program[i].type == INST_DUP ||
							quarkVm.program[i].type == INST_JUMP ||
							quarkVm.program[i].type == INST_JUMP_IF)
							printf("  Value: %ld\n", quarkVm.program[i].value);
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

					Exception exception = vmExecuteInstruction(&quarkVm);

					if (exception != EX_OK)
					{
						fprintf(stderr, "[\033[1;31mERROR\033[0m]: In file '%s',\n  Error at Op %d: %s\n", inputFilePath, i + 1, exceptionAsCstr(exception));
						exit(EXIT_FAILURE);
					}
				}

				if (quarkVm.halt) printf("[\033[1;34mINFO\033[0m]: Program halted.\n");

				vmDumpStack(stdout, &quarkVm);
				if (debug) printf("\n[\033[1;34mINFO\033[0m]: Debugger finished with %ld executed instructions.\n", quarkVm.instructionPointer);

				return EXIT_SUCCESS;
			}
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