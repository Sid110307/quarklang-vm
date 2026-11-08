#include "include/native.h"
#include "include/compiler.h"
#include <stdio.h>

QuarkVM quarkVm = {0};
int debug = 0, stepDebug = 0, limit = -1, dump = 0;

int main(int argc, char **argv)
{
    const char *inputFilePath = NULL;
    if (argc > 1)
        for (int i = 1; i < argc; ++i)
        {
            if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "-d") == 0) debug = 1;
            else if (strcmp(argv[i], "--step") == 0 || strcmp(argv[i], "-s") == 0) stepDebug = 1;
            else if (strcmp(argv[i], "--dump") == 0 || strcmp(argv[i], "-D") == 0) dump = 1;
            else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
            {
                printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);
                printf("[\033[1;34mINFO\033[0m]: Required:\n");
                printf("[\033[1;34mINFO\033[0m]:   --file <file>  | -f <file>: The file to execute\n");
                printf("[\033[1;34mINFO\033[0m]: Optional:\n");
                printf("[\033[1;34mINFO\033[0m]:   --debug        | -d: Start an interactive debugger\n");
                printf("[\033[1;34mINFO\033[0m]:   --step         | -s: Step through the program\n");
                printf("[\033[1;34mINFO\033[0m]:   --dump         | -D: Dump the stack at the end of execution\n");
                printf("[\033[1;34mINFO\033[0m]:   --help         | -h: Print this help message and exit\n");

                exit(EXIT_SUCCESS);
            } else if (strcmp(argv[i], "--file") == 0 || strcmp(argv[i], "-f") == 0)
            {
                inputFilePath = argv[++i];
                if (inputFilePath == NULL)
                {
                    fprintf(stderr, "[\033[1;31mERROR\033[0m]: Missing input file.\n");
                    printf("[\033[1;34mINFO\033[0m]: Usage: %s [options] [--file | -f] <input_file.qce>\n\n", argv[0]);
                    exit(EXIT_FAILURE);
                }
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

    if (inputFilePath == NULL)
    {
        fprintf(stderr, "Missing input file\n");
        return EXIT_FAILURE;
    }

    vmLoadProgramFromFile(&quarkVm, inputFilePath);
    vmPushNativeFunc(&quarkVm, vmAllocate);
    vmPushNativeFunc(&quarkVm, vmFree);
    vmPushNativeFunc(&quarkVm, vmPrintF64);
    vmPushNativeFunc(&quarkVm, vmPrintI64);
    vmPushNativeFunc(&quarkVm, vmPrintPtr);

    Exception exception = EX_OK;
    if (stepDebug)
        while (limit != 0 && !quarkVm.halt)
        {
            int64_t currentOp = quarkVm.instructionPointer;
            if (currentOp < 0 || currentOp >= quarkVm.programSize)
            {
                exception = EX_ILLEGAL_INSTRUCTION_ACCESS;
                break;
            }
            printf("Instruction: %s (%" PRId64 " | %lf | %p)\n",
                   getInstructionName(quarkVm.program[currentOp].type),
                   quarkVm.program[currentOp].value.asI64,
                   quarkVm.program[currentOp].value.asF64,
                   quarkVm.program[currentOp].value.asPtr);
            exception = vmExecuteInstruction(&quarkVm);
            if (exception != EX_OK) break;
            vmDumpStack(stdout, &quarkVm);
            getchar();
            if (limit > 0) --limit;
        }
    else exception = vmExecuteProgram(&quarkVm, limit, debug);

    if (dump) vmDumpStack(stdout, &quarkVm);
    free(quarkVm.program);
    return exception == EX_OK ? EXIT_SUCCESS : EXIT_FAILURE;
}
