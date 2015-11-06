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
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;


int status; //Global Variable

//This function gets the current login username
void getLogin(string userName)
{
	//unistd.h allow getlogin() that get the login username
	if(getlogin() == NULL)
	{
	    userName = "";
	    perror("Login unsuccessful");
	}
}

//This function gets the current host that the current user is using.
void getHost(char hostArray[])
{
	gethostname(hostArray, 64);
	if(gethostname(hostArray, 64) == -1)
	{
	    perror("Failed to get hostname");
	}
}
//The fork function that does the forking
void forking(int pid, char* input[])
{
	if(pid == -1)
	{
		perror("There was an error with fork(). ");
		exit(1);
	}
	else if(pid == 0)
	{
		status = execvp(input[0], input);
		if(-1 == status)
		{
			perror("Error in execvp");
		}
		exit(1);
	}
	else if(pid > 0)
	{
		if(-1 == waitpid(pid, &status, 0))
		{
			perror("wait() had an error.");
		}
	}
}

int main(int argc, char **argv)
{
	bool done = false; //fininsh
    string userName = getlogin();
    getLogin(userName);
    char hostArray[64]; 
    getHost(hostArray);
	
    while(!done)
    {
    	string cmd = "";
    	vector <string> cmds;
    	int semicolon = 0;
    	bool invalid = false; //user inputted an invalid bash commmand

	    //This will get the login username as well as the current host
    	if(getlogin() != NULL)
    	{
        	cout << userName << "@" << hostArray;
    	}

    	cout<<" $ ";

	    getline(cin, cmd);

	    if(cmd.find("#") != string::npos) //If found "#" within the string then enter loop
	    {
	    	cmd = cmd.substr(0, cmd.find("#")); //Skip everything except before the "#"
	    	cout<<"Debug Test: if statement cmd say: "<<cmd<<endl; //Debug Testing
    	}

    	//check for spaces
    	if(cmd == "")
    	{
    		continue;
    	}

    	//Tokenizing and Parsing
    	char_separator<char> delim (" #", "#", keep_empty_tokens);
    	tokenizer< char_separator<char> > mytok(cmd, delim);

    	
    	string temp;
    	for(tokenizer<char_separator<char> >::iterator it = mytok.begin(); it != mytok.end(); it++)
    	{
    		if(*it == ""); //This will do nothing  if it come across a space
    		
    		//This will take care of the ;
    		else if(*it == ";") 
			{
				semicolon++;
				if(semicolon > 1)
				{
					perror("Error using ;");
					invalid = true;
					break;
				}
			}
			else if(semicolon == 1)
			{
				cmds.push_back(temp);
				temp = "";
				cmds.push_back(";");
				semicolon = 0;
				break;

			}	
			else
			{
				if(semicolon != 0)
				{
					perror("Error!!");
					invalid = true;
					break;
				}
				if(temp != "")
					temp += ' ';
				temp += *it;
			}

    	}

    	cmds.push_back(temp.c_str());
    	temp = "";

    	if(!invalid)
    	{
	    	char* value[200];

	    	//This is where execution cmds begins
	    	for(int i = 0; i < cmds.size(); i++) 
	    	{
	    		string currentCmd = "";
	    		string command = "";
	    		int k = 0;
	    		for(int j = 0; j < cmds.at(i).size(); j++)//Traverse through each letter
	    		{
	    			currentCmd = cmds.at(i);
	    			if(currentCmd[j] == ' ')
	    			{
	    				value[k] = new char[command.size() + 1];
	    				strcpy(value[k], command.c_str());
	    				k++;
	    				command = "";
	    			}
	    			else
	    			{
	    				command += currentCmd[j];
	    			}
	    		}

	    		value[k] = new char[command.size()+1];
	    		strcpy(value[k], command.c_str());
	    		k++;

	    		value[k] = new char[1]; 
	    		value[k] = NULL;

	    		if(cmds.at(i) == "exit")
	    		{
	    			done = true;
	    			cout<< "Debug Test: See ya!"<<endl;
	    			break;
	    		}

	    		else if(cmds.at(i) == ";")
	    		{
	    			if(status)
	    			{
	    				continue;
	    			}
	    			else
	    			{
	    				i++;
	    				continue;
	    			}
	    		}
	    	}

	    	int pid = fork();
	    	forking(pid, value);

	    	if(done)
	    	{
	    		cout << "Sayonara!" <<endl;
	    		break;
	    	}
    	}
    }

	cout<<endl;
	return 0;
}
