#ifdef __INTELLISENSE__
    #pragma diag_suppress 1696
#endif

#define MAX_INPUT_SIZE 512
#define HISTORY_SIZE 20

#ifndef HISTORY_H
#define HISTORY_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

char* history[HISTORY_SIZE];
int history_value;

int addToHistory(char * input);

int clearHistory();

void printHistory();

int getCommand(int cmd_num, char ** cmd_to_execute);

//TO-DO Add -e flags

#endif
