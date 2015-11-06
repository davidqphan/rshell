#include <sys/wait.h>
#include <string.h>
#include <unistd.h> 
#include <stdio.h>
#include <vector>
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

	//This will get the login username as well as the current host
	if(getlogin() != NULL)
		cout << userName << "@" << hostarray;
	
	cout << " $ "; //mimic the original bash script "$" sign

	cout<<endl;
	return 0;
}