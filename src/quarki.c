#include "include/compiler.h"

QuarkVM quarkVm = {0};
VMTable table = {0};

int main(int argc, char **argv)
{
    if (argc > 1)
        if (strcmp(argv[1], "--file") == 0 || strcmp(argv[1], "-f") == 0)
        {
            const char *inputFilePath = argv[2];
            if (inputFilePath == NULL)
            {
                fprintf(stderr, "[\033[1;31mERROR\033[0m]: Missing input file\n");
                printf("[\033[1;34mINFO\033[0m]: Usage: %s [--file | -f] <input_file.qas>\n\n", argv[0]);
                exit(EXIT_FAILURE);
            }

            char *outputFilePath = malloc(strlen(inputFilePath) + 5);

            if (outputFilePath == NULL)
            {
                fprintf(stderr, "[\033[1;31mERROR\033[0m]: Could not allocate memory for output file\n");
                exit(EXIT_FAILURE);
            }

            strcpy(outputFilePath, inputFilePath);

            char *dot = strrchr(outputFilePath, '.');
            if (dot != NULL) *dot = '\0';

            strcat(outputFilePath, ".qce");
            vmParseSource(sv_readFile(inputFilePath), &quarkVm, &table, inputFilePath);
            vmSaveProgramToFile(&quarkVm, outputFilePath);

            printf("[\033[1;34mINFO\033[0m]: Program compiled to `%s`.\n", outputFilePath);
            return EXIT_SUCCESS;
        } else
        {
            fprintf(stderr, "[\033[1;31mERROR\033[0m]: Unknown argument: %s\n", argv[1]);
            printf("[\033[1;34mINFO\033[0m]: Usage: %s [--file | -f] <input_file.qas>\n\n", argv[0]);

            exit(EXIT_FAILURE);
        }
    else
    {
        printf("[\033[1;34mINFO\033[0m]: Usage: %s [--file | -f] <input_file.qas>\n\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}
