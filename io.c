#include "shell1.h"

void io(char* token) {
    char *arg[100];
    int input_fd, output_fd, i;

    while(1){
        i = 0;
        while(token != NULL && i < 100-1){
            arg[i++] = strtok(token, " ");
            token = strtok(NULL, ";");
        }
        arg[i] = NULL;

        input_fd = dup(STDIN_FILENO);
        output_fd = dup(STDOUT_FILENO);

        for(int j = 0; arg[j] != NULL; j++){
            if(strcmp(arg[j], "<") == 0){
                arg[j] = NULL;
                input_fd = open(arg[j+1], O_RDONLY);
                
                if(input_fd < 0) {
                    printf("OPEN ERROR\n");
                    exit(EXIT_FAILURE);
                }

                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
                break;
            }
            else if(strcmp(arg[j], ">") == 0){
                arg[j] = NULL;
                output_fd = open(arg[j+1], O_WRONLY | O_CREAT | O_TRUNC, 0666);

                if(output_fd < 0) {
                    printf("OPEN ERROR\n");
                    exit(EXIT_FAILURE);
                }

                dup2(output_fd, STDOUT_FILENO);
                close(output_fd);
                break;
            }
        }

        if(arg[0] != NULL){
            if(execvp(arg[0], arg) < 0){
                printf("EXECVP ERROR\n");
                exit(EXIT_FAILURE);
            }
        }

        dup2(input_fd, STDIN_FILENO);
        dup2(output_fd, STDOUT_FILENO);

        close(input_fd);
        close(output_fd);
    }

}
