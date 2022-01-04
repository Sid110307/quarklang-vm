#include "./compiler.c"

int main(int argc, char** argv)
{
	if (argc > 1)
	{
		for (int i = 1; i < argc; i++)
		{
			if (strcmp(argv[i], "--file") == 0 || strcmp(argv[i], "-f") == 0)
			{
				const char* inputFilePath = argv[++i];
				if (inputFilePath == NULL)
				{
					fprintf(stderr, "[\033[1;31mERROR\033[0m]: Missing input file\n");
					printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qas>\n\n", argv[0]);
					exit(EXIT_FAILURE);
				}

				char* outputFilePath = malloc(strlen(inputFilePath) + 5);

				if (outputFilePath == NULL)
				{
					fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not allocate memory for output file\n");
					exit(EXIT_FAILURE);
				}

				strcpy(outputFilePath, inputFilePath);

				char* dot = strrchr(outputFilePath, '.');
				if (dot != NULL)
					*dot = '\0';

				strcat(outputFilePath, ".qce");

				StringView source = readFile(inputFilePath);
				quarkVm.programSize = vmParseSource(source, quarkVm.program, VM_PROGRAM_SIZE);

				vmSaveProgramToFile(quarkVm.program, quarkVm.programSize, outputFilePath);
				printf("[\033[1;34mINFO\033[0m]: Program compiled to '%s'.\n", outputFilePath);
				return EXIT_SUCCESS;
			}
			else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
			{
				printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qas>\n\n", argv[0]);
				printf("[\033[1;34mINFO\033[0m]: Options:\n");
				printf("[\033[1;34mINFO\033[0m]:   --file <file> | -f <file>: Specify the file to compile\n");
				printf("[\033[1;34mINFO\033[0m]:   --help        | -h: Print this help message and exit\n");
				return EXIT_SUCCESS;
			}
			else
			{
				fprintf(stderr, "[\033[1;31mERROR\033[0m]: Unknown argument: %s\n", argv[i]);
				printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qas>\n\n", argv[0]);
				printf("[\033[1;34mINFO\033[0m]: Use -h or --help for more information\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	else
	{
		printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qas>\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}