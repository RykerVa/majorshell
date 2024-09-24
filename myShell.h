#ifndef MYSHELL_H
#define MYSHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 512
#define MAX_ARGUMENTS 16

void shell_loop(void);

#endif // MYSHELL_H
