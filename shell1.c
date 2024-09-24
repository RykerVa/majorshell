#include "shell1.h"

int main(int argc, char *argv[]) {
	char input[MAX_INPUT_SIZE];
	char *args[MAX_ARGUMENTS];
	char *token;
	int status;
	char *history_command = NULL;
	
	signal(SIGTSTP, SIG_IGN);

	while (1) {
		//variables
		char input[MAX_INPUT_SIZE];	//user input
		char *token; //for parsing input
		char *history_command = NULL;
		char *batchfile;
		int mode = -1; //0 = interactive, 1 = batch
		FILE *fp; //file pointer
		
		// Read user input
		if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
			break;
		}
		// Remove trailing newline character
		input[strlen(input) - 1] = '\0';
	   
		//user only inputs "exit"
		if(strstr(input, "exit") != NULL) {
		   exit(0);
		}

		// Determining mode from user input
		token = strtok(input, " ");
		if (token != NULL) {
			if(strstr(token, "newshell") != NULL) { //"newshell"
				token = strtok(NULL, " "); //get next arg
				
				if(token == NULL) { //no arg, interactive mode
					mode = 0;
				}
				else //batch file mode
				{
					mode = 1;
					batchfile = token;
				  
					token = strtok(NULL, " "); //get next arg
					if(token != NULL) { //too many arguments
						//send error message and exit gracefully
						fprintf(stderr, "Incorrect number of arguments. Exiting...\n");
						exit(1);
					}

					fp = fopen(batchfile, "r"); //open file for reading
					if(fp == NULL) { //file does not exist 
						fprintf(stderr, "%s: File does not exist. Exiting...\n", batchfile);
						exit(1);
					}
				}
			}
			else {
				//error message
				printf("Usage: newshell [batchFile]\n");
			}
		}
		//else: empty input, just continue
		
		if (mode != -1) { //mode was selected
			//variables
			char *args[MAX_ARGUMENTS];
			int status;
		
			// Running the shell
			while (1) {
				//printing prompt
				if (mode == 0) { //interactive
					printf("myshell> ");
					fflush(stdout);
				}
				else if (fgets(input, MAX_INPUT_SIZE, fp) != NULL){ //batch, NOT end of file
					printf("Executing command: %s", input);
				}
				else { //end of file
					fclose(fp); //close file
					return 0;
				}

				// Read user input
				if (mode == 0) { //interactive
					if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
						break;
					}
				}

				addToHistory(input);

				// Remove trailing newline character
				input[strlen(input) - 1] = '\0';
					
				int willExit = 0; //false, will not exit program

				// Tokenize input and look for ';'-separated commands
				token = strtok(input, ";");
				while (token != NULL) {
					//token is the full command to be executed, (e.i. g++ -c main.cpp)
					//In each command function, it will need to be broken up and checked
					//if the commands are valid
					
					// Executing built-in commands
					if(strstr(token, "cd") != NULL) {
						char *com;
						char *arg;

						com = strtok(token, " ");
						arg = strtok(NULL, " ");
						
						if(arg != NULL) {
							chdir(arg);
						}
						else {
							chdir(getenv("HOME"));
						}
					}
					else if(strstr(token, "exit") != NULL) {
						willExit = 1; //true, will exit program at the end
					}
					else if(strstr(token, "path") != NULL) {
						//implement path command
					}
					else if(strstr(token, "myhistory") != NULL) 
					{
						if(strstr(token, " -c"))
						{
							printf("Running clear history");
							clearHistory();
						}
						else if (strstr(token, " -e"))
						{
							getCommand(atoi(strstr(token, " -e") + 3), &history_command);
							printf("cmd: %s\n", history_command);
						}
						else
							printHistory();
					}
					// Executing non-built-in commands
					else {
						// Fork a new process
						pid_t pid = fork();
						if (pid < 0) {
							perror("fork failed");
							exit(EXIT_FAILURE);
						}

						if (pid == 0) {
							// Child process

							signal(SIGTSTP, handle_ctrlz);
							
							if (strchr(token, '<') || strchr(token, '>'))//No need to redirect if there is no redirection
								io(token); // I/O redirect function

							int pipelining = 0; //false
							
							// Separating command from arguments (if any)
							int arg_index = 0;
							char* cmd = strtok(token, " "); //command
							
							if(cmd != NULL) { //not nothing
								args[arg_index++] = cmd;
								char* token2 = strtok(NULL, " ");
								
								//get arguments
								while(token2 != NULL) {
									
									//pipelining
									if(strstr(token2, "|") != NULL) { //1st pipeline character
										//variables
										char* pcmd2 = "\0"; //2nd command
										char* pcmd3 = "\0"; //3rd command (optional)
										char *pargs2[MAX_ARGUMENTS]; //2nd command's  arguments
										char *pargs3[MAX_ARGUMENTS]; //3rd command's  arguments (optional)
										
										//indices for arg arrays
										int index2 = 0;
										int index3 = 0;
										
										pipelining = 1; //true
										
										//2nd argument command
										pcmd2 = strtok(NULL, " ");
										
										if(pcmd2 != NULL) //not nothing
										{
											pargs2[index2++] = pcmd2;
											token2 = strtok(NULL, " ");
										
											while(token2 != NULL) //get arguments for 2nd command
											{
												if(strstr(token2, "|") != NULL) //2nd pipeline character
												{
													//3rd argument command
													pcmd3 = strtok(NULL, " ");
													
													if(pcmd3 != NULL) //not nothing
													{
														pargs3[index3++] = pcmd3;
														token2 = strtok(NULL, " ");
														
														while(token2 != NULL) //get arguments for 3rd command
														{
															pargs3[index3++] = token2;
															token2 = strtok(NULL, " ");
														}
													}
												}
												else
												{
													pargs2[index2++] = token2;
													token2 = strtok(NULL, " ");
												}
											}
										}
										
										//NULL terminating the argument arrays
										args[arg_index] = NULL;
										pargs2[index2] = NULL;
										pargs3[index3] = NULL;
										
										//performing pipelining
										pipeline(cmd, pcmd2, pcmd3, args, pargs2, pargs3);
									}
									else {
										args[arg_index++] = token2;
										token2 = strtok(NULL, " ");
									}
								}
								args[arg_index] = NULL;
							}
							
							//executing command
							if(pipelining == 0) {
								fflush(stdout);
								if (execvp(cmd, args) < 0) {
									fprintf(stderr, "%s: command not found\n", args[0]);
									fflush(stdout);
									exit(EXIT_FAILURE);
								}
							}
						}
						else {
							// Parent process
							if (setpgid(pid, pid))
							{
								printf("setpgid failed\n");fflush(stdout);
								exit(0);
							}
							if (tcsetpgrp(STDIN_FILENO, pid))
							{
								printf("tcsetpgrp failed\n");fflush(stdout);
								exit(0);
							}

							waitpid(pid, &status, 0);

							signal(SIGTTOU, SIG_IGN);
							if (tcsetpgrp(STDIN_FILENO, getpgrp()))
							{
								printf("tcsetpgrp failed\n");fflush(stdout);
								exit(0);
							}
						}
					}

					if (history_command != NULL)
					{
						token = history_command;
						history_command = NULL;
					}
					else
						token = strtok(NULL, ";");
				}
				//if exit, exit after all commands are done executing
				if (willExit == 1) {
					exit(0);
				}
			}
		}
	}

	return EXIT_SUCCESS;
}
