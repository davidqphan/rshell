#include <unistd.h>          
#include <vector>
#include <string>
#include <stdio.h>       
#include <string.h>       
#include <signal.h>
#include <string.h>
#include <iostream>
#include <sys/wait.h>      
#include <sys/stat.h>
#include <sys/types.h>      
#include <boost/tokenizer.hpp> 

using namespace std;
using namespace boost;

//This function gets the current login username
void getLogin(string userName)
{
    //unistd.h allow getlogin() that get the login username
    if(getlogin() == NULL)
    {
        userName = "";
        perror("Login unstatusful");
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

// parses a command from the vector of cmds based on the rules of the connectors
void parse(string cmd, vector<string>& cmds)
{
        char_separator<char> delim(" ;#[]()");
        tokenizer <char_separator<char> > mytok(cmd, delim);

        for(tokenizer<char_separator<char> >::iterator it = mytok.begin(); it != mytok.end(); it++)
        {
            cmds.push_back(*it);
        }        
}

//The fork function that does the forking
void forking(int pid, char* input[], int& status)
{
    if(pid == -1)
    {
        perror("There was an error with fork(). ");
        exit(1);
    }
    else if(pid == 0)
    {
        status = execvp(input[0], input);
        if(status == -1)
        {
            cout<<"status: "<<status<<endl;
            perror("Error in execvp");
        }
        exit(1);
    }
    else if(pid > 0)
    {
        if(waitpid(pid, &status, 0) == -1)
        {
            perror("wait() had an error.");
        }
    }
}

int main(int argc, char **argv)
{
    bool done = false;
    string userName = getlogin();
    getLogin(userName);
    char hostArray[64]; 
    getHost(hostArray);
    
    while(!done)
    {
        int status;
        string cmd = "";
        vector <string> cmds;
        string prev = ";";                                    

        //This will get the login username as well as the cmdLetter host
        if(getlogin() != NULL)
        {
            cout << userName << "@" << hostArray<< " $ ";
        }

        getline(cin, cmd);

        if(cmd.find("#") != string::npos) //If found "#" within the string then enter loop
        {
            cmd = cmd.substr(0, cmd.find("#")); //Skip everything except before the "#"
            cout<<"Debug Test: if statement cmd say: "<<cmd<<endl; //Debug Testing
        }

        //check for spaces
        if(cmd == "")       
            continue;

        //Tokenizing and parsing
        parse(cmd,cmds);


        const int SIZE = 20;
        char *input[SIZE];
        int counter;
        string prev = ";";
        //This is where execution cmds begins
        //We used unsigned instead of normal int because we're comparing the size
        //of our vector of strings
        for(unsigned int i = 0; i < cmds.size(); i++)
        {
            string cmdLetter = "";
            string command = "";
            int k = 0;
            for(unsigned int j = 0; j < cmds.at(i).size(); j++) //Traverse through each letter
            {
                cmdLetter = cmds.at(i);
                if(cmdLetter[j] == ' ')
                {
                    //if there is a space, then it means
                    //the command is done being parsed
                    //after strcpy the command into input[k]
                    input[k] = new char[command.size() + 1];
                    strcpy(input[k], command.c_str());
                    k++;
                    command = "";
                }
                else
                    command += cmdLetter[j];    //store each letter into command
            }   

            //Allocate exactly enough memory
            //to strcpy command into input[k]
            input[k] = new char[command.size() + 1];
            strcpy(input[k], command.c_str());
            k++;

            //ends the input array with a null terminating character
            input[k++] = NULL;
        }

        for(unsigned int i =0; i < cmds.size(); i++)
        {
            if(cmds[i] == "exit")
            {
                if(prev == ";")
                {
                    exit(0);
                }
                else if(prev == "||" && (status < 0 || status > 0))
                {
                    exit(0);
                }
                else if(prev == "&&" && (status == 0))
                {
                    exit(0);
                }
            }

            if(cmds[i] == "[")
                cmds[i] = "test";

            if(cmds[i] == "]")
                cmds.erase(cmds.begin() + i);


            if(cmds[i] == "#")
            {
                if(prev == ";")
                {
                    
                    int pid = fork();
                    forking(pid, input, status);
                }
                else if(prev == "||")
                {
                    if(status < 0 || status > 0)
                    {
                        
                        int pid = fork();
                        forking(pid, input, status);
                    }
                }
                else if(prev == "&&")
                {
                    int pid = fork();
                    forking(pid, input, status);
                }
            }


            if(cmds[i] == ";" || cmds[i] == "||" || cmds[i] == "&&" || cmds[i] == ")")
            {
                if(prev == ";")
                {

                    int pid = fork();
                    forking(pid, input, status);
                }
                else if(prev == "||")
                {
                    if(status > 0 || status < 0)
                    {

                        int pid = fork();
                        forking(pid, input, status);
                    }
                }
                else if(prev == "&&")
                {
                    if(status == 0)
                    {

                        int pid = fork();
                        forking(pid, input, status);
                    }
                }

                for(int k =0; k < SIZE; k++)
                {
                    input[k] = 0;
                }

                prev = cmds[i];
            }

            else if(cmds[i] == "(")
            {
                cout << "enter (\n";
                if((prev == "||" && status == 0) || (prev == "&&" && (status < 0 || status > 0)))
                {
                    cout << "enter if\n";
                    while(cmds[i] != ")")
                    {
                        cout << "enter while\n";
                        i++;
                    }
                }
            }

            else if(i == cmds.size() -1)
            {

                if(prev == ";")
                {

                    int pid = fork();
                    forking(pid, input, status);
                }
                else if(prev == "||")
                {

                    int pid = fork();
                    forking(pid, input, status);
                }
                else if(prev == "&&")
                {

                    int pid = fork();
                    forking(pid, input, status);
                }

                for(int y =0; y < SIZE; y++)
                {
                    input[y] = 0;
                }

                prev = cmds[i];
            }
        }
    }
    cout << "\n";
    return 0;   
}
