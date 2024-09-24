#include "shell1.h"

/*
Name: Kelley Le
Description: pipeline() function
Inputs: three strings and three string arrays as input. The strings represent the command to be executed and the string arrays represent the arguments for the corresponding command. The third command/arguments is optional.
	
This code connects the standard output of cmd1 to the standard input of cmd2, and again connects the standard output of cmd2 to the standard input of cmd3  (if there is a cmd3) and executes the commands.
*/


void pipeline(char *cmd1, char *cmd2, char *cmd3, char **args1, char **args2, char **args3)
{
	//variables
	int fd1[2]; //file descriptor for 1 pipe
	
	//make pipe for 1st command to 2nd command
	if (pipe(fd1) == -1)
	{
		perror("pipe");
		exit(EXIT_FAILURE);
	}
	
	if(cmd3[0] == '\0') //1 pipe
	{
		//fork 1st child to execute 1st command
		if(fork() == 0)
		{
			close(fd1[0]); //close read end
			
			//duplicate write end of pipe to stdout
			dup2(fd1[1], 1);
			
			close(fd1[1]); //close write end
			
			if (execvp(cmd1, args1) < 0)
			{
				fprintf(stderr, "%s: command not found\n", args1[0]);
				exit(EXIT_FAILURE);
			}
		}
		else //parent
		{
			//fork 2nd child to execute 2nd command
			if(fork() == 0)
			{
				close(fd1[1]); //close write end
				
				//duplicate read end of pipe to stdin
				dup2(fd1[0], 0);
				
				//close pipe
				close(fd1[0]); //close read end
				
				if (execvp(cmd2, args2) < 0)
				{
					fprintf(stderr, "%s: command not found\n", args2[0]);
					exit(EXIT_FAILURE);
				}
			}
		}
		
		//close pipe
		close(fd1[0]);
		close(fd1[1]);
		
		//wait for 2 children
		wait( (int *)0 );
		wait( (int *)0 );
	}
	else //2 pipes
	{
		//variables
		
		int fd2[2]; //file descriptor for 2nd pipe
		//make pipe for 2nd command to 3rd command
		if (pipe(fd2) == -1)
		{
			perror("pipe");
			exit(EXIT_FAILURE);
		}
		
		//fork 1st child to execute 1st command
		if(fork() == 0)
		{
			//close pipes
			close(fd1[0]);
		    close(fd2[0]); 
		    close(fd2[1]); 
			
			//duplicate write end of 1st pipe to stdout
			dup2(fd1[1], 1);
			
			close(fd1[1]); 
			
			if (execvp(cmd1, args1) < 0)
			{
				fprintf(stderr, "%s: command not found\n", args1[0]);
				exit(EXIT_FAILURE);
			}
		}
		else //parent
		{
			//fork 2nd child to execute 2nd command
			if(fork() == 0)
			{
				//close pipes
				close(fd1[1]); 
				close(fd2[0]); 
				
				//connecting the two pipes
				dup2(fd1[0], 0);
				dup2(fd2[1], 1);

				close(fd1[0]);
				close(fd2[1]); 
				
				if (execvp(cmd2, args2) < 0)
				{
					fprintf(stderr, "%s: command not found\n", args2[0]);
					exit(EXIT_FAILURE);
				}
			}
			else //parent
			{
				//fork 3rd child to execute 3rd command
				if(fork() == 0)
				{
					//close pipes
					close(fd1[0]);
					close(fd1[1]); 
					close(fd2[1]); 
					
					//duplicate read end of second pipe to stdin
					dup2(fd2[0], 0);
					
					close(fd2[0]); 
					
					if (execvp(cmd3, args3) < 0)
					{
						fprintf(stderr, "%s: command not found\n", args3[0]);
						exit(EXIT_FAILURE);
					}
				}
			}
		}
		
		//close pipes
		close(fd1[0]);
		close(fd1[1]); 
		close(fd2[0]); 
		close(fd2[1]); 
		
		//wait for 3 children
		wait( (int *)0 );
		wait( (int *)0 );
		wait( (int *)0 );
		
	}
	//close pipe
	close(fd1[0]);
	close(fd1[1]);
	
	exit(0);
}
