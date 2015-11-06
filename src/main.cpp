#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char * argv[])
{
	char * args[2] = {"ls", NULL} ;
	pid_t c_pid, pid;
	int status;

	c_pid = fork();

	if(c_pid < 0)
	{
		perror("fork failed");
		exit(1);
	}

	else if (c_pid == 0)
	{
		printf("child: executing ls\n");
		execvp(args[0], args);
		perror("execve failed");
		
	}
	
	else if (c_pid > 0)
	{
		if(pid = wait(&status) < 0)
		{
			perror("wait");
			exit(1);
		}

		printf("Parent: Finished\n");
	}

}
