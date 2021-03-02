#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>

int main(int argc, char *argv[])
{

    char *allProcesses = (char *)malloc(4096);
    char *processId = (char *)malloc(16);
    char *commandLine = (char *)malloc(256);

    // no arguments passed
    if (argc == 1)
    {

        printf("\nRight Usage:\n");
        printf("\t-all prints some information (process id and its argument/s) about all processes\n");
        printf("\t-p takes process id and prints the details of it\n");
        printf("\t-k takes process id and kills it\n");
    }
    // -all argument passed
    else if (argc == 2)
    {

        if (strcmp(argv[1], "-all") == 0)
        {
            // call systemcall
            long int call_return = syscall(335, allProcesses, processId, commandLine, 1); // 1 for - all argument
            printf("Process id and the argument list for the processes:\n");
            printf("uid\tppid\tpid\tname\tstate\t\t\n");
            printf("%s", allProcesses);
        }
    } // -p argument or -k argument
    else if (argc == 3)
    {

        processId = argv[2];

        if (strcmp(argv[1], "-p") == 0)
        {
            long int call_return = syscall(335, allProcesses, processId, commandLine, 2); // 2 for -p argument
            printf("Argument#[%s]-Process Information: \n", argv[2]);
            printf("%s\t%s", processId, commandLine);
        }
        else if (strcmp(argv[1], "-k") == 0)
        {

            processId = atoi(processId);
            long int call_return = syscall(335, allProcesses, processId, commandLine, 3); // 3 for -k argument
            printf("Process %s was killed... ", argv[2]);
        }
    }
}
