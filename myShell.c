#include "myShell.h"

void shell_loop(void) {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARGUMENTS];
    char *token;
    int status;

    while (1) {
        printf("myshell> ");
        fflush(stdout);

        // Read user input
        if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
            break;
        }

        // Remove trailing newline character
        input[strlen(input) - 1] = '\0';

        // Tokenize input and store arguments in args array
        int arg_index = 0;
        token = strtok(input, " ");
        while (token != NULL) {
            args[arg_index++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_index] = NULL;

        // Fork a new process
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Child process
            if (execvp(args[0], args) < 0) {
                perror(args[0]);
                exit(EXIT_FAILURE);
            }
        } else {
            // Parent process
            waitpid(pid, &status, 0);
        }
    }
}
