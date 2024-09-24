#define MAX_INPUT_SIZE 512
#define MAX_ARGUMENTS 16

#ifndef SHELL1_H
#define SHELL1_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "history.h"
#include <signal.h>
#include "zsignal.h"

void pipeline(char *cmd1, char *cmd2, char *cmd3, char **args1, char **args2, char **args3);
void io(char* token);

#endif // SHELL1_H
