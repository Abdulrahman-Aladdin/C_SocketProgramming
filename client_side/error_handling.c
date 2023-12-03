#include "error_handling.h"
#include <stdio.h>
#include <stdlib.h>

// Error handling function for the errors caused by the user
void DieWithUserMessage(const char *msg, const char *detail)
{
    fputs(msg, stderr);
    fputs(": ", stderr);
    fputs(detail, stderr);
    fputc('\n', stderr);
    exit(1);
}

// Error handling function for the errors caused by system calls
void DieWithSystemMessage(const char *msg)
{
    perror(msg);
    exit(1);
}
