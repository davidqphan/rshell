#include <unistd.h>          // allows us to use fork 
#include <sys/types.h>      // pid_t
#include <sys/wait.h>      // wait()
#include <string.h>       // strtok()
#include <stdio.h>       // perror
#include <signal.h>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>

using namespace std;


int status; //Global Variable

void getLogin(string userName)
{
	//unistd.h allow getlogin() that get the login username
	if(getlogin() == NULL)
	{
	    userName = "";
	    perror("Login unsuccessful");
	}
}

void getHost(char hostArray[])
{
	gethostname(hostArray, 64);
	if(gethostname(hostArray, 64) == -1)
	{
	    perror("Failed to get hostname");
	}
}

int main(int argc, char **argv)
{

    string userName = getlogin();
    getLogin(userName);
    char hostarray[64]; 
    getHost(hostarray);
	
    while(true)
    {
	    //This will get the login username as well as the current host
    	if(getlogin() != NULL)
        cout << userName << "@" << hostarray << " $ ";
	
	char command[128];

	cin.getline(command,128);

	vector<char*> arguments;
	char* prog = strtok(command, " ");
	char* temp = prog;

	while(temp != NULL)
	{
	    arguments.push_back(temp);
	    temp = strtok(NULL, " ");
	}

	argv = new char*[arguments.size()+1];

	for(int i =0; i < arguments.size(); i++)
	{
	    argv[i] = arguments[i];
	}

	argv[arguments.size()] = NULL;

	// checks if user wants to exit or not
	if(strcmp(command, "exit") == 0)
	{
	    return 0;
	}
	else
	{	
            pid_t c_pid = fork();

	    if(c_pid < 0)
            {
	        perror("Error: cannot fork");
		    return -1;
	    }
	    else if(c_pid == 0)
            {
		// child 
		execvp(prog, argv);

		perror(command);
		return -1;
	    }
	    else
	    {
	        // parent. wait for child to finish
		if(waitpid(c_pid, 0, 0) < 0)
		{
		    perror("Error: cannot wait for child.");
		    return -1;
		}
	    }
	
	}
    }

	cout<<endl;
	return 0;
}
