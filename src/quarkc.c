#include "include/native.h"
#include "include/compiler.h"

QuarkVM quarkVm = {0};
int debug = 0, stepDebug = 0, limit = -1, dump = 0;

int main(int argc, char **argv)
{
    if (argc > 1)
        for (int i = 1; i < argc; ++i)
        {
            if (strcmp(argv[i], "--limit") == 0 || strcmp(argv[i], "-l") == 0)
            {
                if (argv[++i] == NULL)
                {
                    printf("[\033[1;34mINFO\033[0m]: Current limit: %d\n", VM_EXECUTION_LIMIT);
                    printf("[\033[1;34mINFO\033[0m]: Change the limit with --limit <number> or -l <number>.\n");
                    exit(EXIT_FAILURE);
                }

                VM_EXECUTION_LIMIT = (int) strtol(argv[i], NULL, 10);
            } else if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0) debug = 1;
            else if (strcmp(argv[i], "--step") == 0 || strcmp(argv[i], "-s") == 0) stepDebug = 1;
            else if (strcmp(argv[i], "--dump") == 0 || strcmp(argv[i], "-D") == 0) dump = 1;
            else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
            {
                printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);
                printf("[\033[1;34mINFO\033[0m]: Required:\n");
                printf("[\033[1;34mINFO\033[0m]:   --file <file>  | -f <file>: The file to compile\n");
                printf("[\033[1;34mINFO\033[0m]: Optional:\n");
                printf("[\033[1;34mINFO\033[0m]:   --limit <size> | -l <size>: The maximum number of instructions to run. (default: %d)\n",
                       VM_EXECUTION_LIMIT);
                printf("[\033[1;34mINFO\033[0m]:   --debug        | -d: Start an interactive debugger\n");
                printf("[\033[1;34mINFO\033[0m]:   --step         | -s: Step through the program\n");
                printf("[\033[1;34mINFO\033[0m]:   --dump         | -D: Dump the stack at the end of execution\n");
                printf("[\033[1;34mINFO\033[0m]:   --help         | -h: Print this help message and exit\n");

                exit(EXIT_SUCCESS);
            } else if (strcmp(argv[i], "--file") == 0 || strcmp(argv[i], "-f") == 0)
            {
                const char *inputFilePath = argv[++i];
                if (inputFilePath == NULL)
                {
                    fprintf(stderr, "[\033[1;31mERROR\033[0m]: Missing input file.\n");
                    printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);
                    exit(EXIT_FAILURE);
                }

                vmLoadProgramFromFile(&quarkVm, inputFilePath);

                vmPushNativeFunc(&quarkVm, vmAllocate); // 0
                vmPushNativeFunc(&quarkVm, vmFree);     // 1
                vmPushNativeFunc(&quarkVm, vmPrintF64); // 2
                vmPushNativeFunc(&quarkVm, vmPrintI64); // 3
                vmPushNativeFunc(&quarkVm, vmPrintPtr); // 4

                int status = vmExecuteProgram(&quarkVm, limit, debug);
                if (!stepDebug)
                {
                    if (dump) vmDumpStack(stdout, &quarkVm);
                    if (status != EX_OK) return EXIT_FAILURE;

                    return EXIT_SUCCESS;
                } else
                    for (int j = 0; limit != 0 && !quarkVm.halt && j < quarkVm.programSize; ++j)
                    {
                        printf("Instruction: %s (%" PRId64 " | %lf | %p)\n",
                               getInstructionName(quarkVm.program[j].type),
                               quarkVm.program[j].value.asI64,
                               quarkVm.program[j].value.asF64,
                               quarkVm.program[j].value.asPtr);

                        if (vmExecuteInstruction(&quarkVm) != EX_OK) return EXIT_FAILURE;
                        vmDumpStack(stdout, &quarkVm);
                        getchar();

                        if (limit > 0) --limit;
                    }

                return EXIT_SUCCESS;
            } else
            {
                fprintf(stderr, "[\033[1;31mERROR\033[0m]: Unknown argument: %s\n", argv[i]);
                printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);

                exit(EXIT_FAILURE);
            }
        }
    else
    {
        printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
