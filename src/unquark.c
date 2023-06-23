#include "include/compiler.h"

QuarkVM vm = {0};
int isRaw = 0;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);
        printf("[\033[1;34mINFO\033[0m]: Type --help or -h for more information\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
        {
            printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);
            printf("[\033[1;34mINFO\033[0m]: Required Parameters:\n");
            printf("[\033[1;34mINFO\033[0m]:   --file <file> | -f <file>: Specify the file to decompile\n");
            printf("[\033[1;34mINFO\033[0m]: Optional Parameters:\n");
            printf("[\033[1;34mINFO\033[0m]:   --help        | -h: Print this help message and exit\n");
            printf("[\033[1;34mINFO\033[0m]:   --raw: Print raw text\n");

            exit(EXIT_SUCCESS);
        } else if (strcmp(argv[i], "--raw") == 0) isRaw = 1;
        else if (strcmp(argv[i], "--file") == 0 || strcmp(argv[i], "-f") == 0)
        {
            const char *inputFilePath = argv[++i];
            if (inputFilePath == NULL)
            {
                fprintf(stderr, "[\033[1;31mERROR\033[0m]: Missing input file\n");
                printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);

                exit(EXIT_FAILURE);
            }

            vmLoadProgramFromFile(&vm, inputFilePath);
            for (int64_t j = 0; j < vm.programSize; ++j)
            {
                !isRaw ? instructionWithOperand(vm.program[j].type)
                         ? printf("Op \033[1;34m%" PRId64 "\033[0m: %s (I64: %" PRId64 ", F64: %lf, PTR: %p)\n", j,
                                  getInstructionName(vm.program[j].type), vm.program[j].value.asI64,
                                  vm.program[j].value.asF64, vm.program[j].value.asPtr)
                         : printf("Op \033[1;34m%" PRId64 "\033[0m: %s\n", j, getInstructionName(vm.program[j].type))
                       : instructionWithOperand(vm.program[j].type)
                         ? printf("%s (I64: %" PRId64 ", F64: %lf, PTR: %p)\n", getInstructionName(vm.program[j].type),
                                  vm.program[j].value.asI64, vm.program[j].value.asF64, vm.program[j].value.asPtr)
                         : printf("%s\n", getInstructionName(vm.program[j].type));
            }
        } else
        {
            fprintf(stderr, "[\033[1;31mERROR\033[0m]: Unknown option '%s'\n", argv[i]);
            printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);
            printf("[\033[1;34mINFO\033[0m]: Type --help or -h for more information\n");

            exit(EXIT_FAILURE);
        }
    }

    return EXIT_SUCCESS;
}
