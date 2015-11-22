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


        char *input[500];
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

            if(cmds.at(i) ==  "exit") 
            {
                done = true;
                //cout<< "Debug Test: See ya!"<<endl;
                break;
            }
            // else if(cmds.at(i) == "test")
            // {
            //         struct stat sb;
            //         int index = 1;
            //         if((input[1] == "-e") || (input[1] == "-f") || (input[1] == "-d"))
            //         {
            //             index++;
            //             continue;
            //         }
            //         switch(sb.st_mode & S_IFMT)
            //         {
            //             case S_IFREG:
            //                 if((input[1] == "-e") || (input[1] == "-f") || (index == 1))
            //                 {
            //                     cout<<"Regular File"<<endl;
            //                 }
            //             case S_IFDIR:
            //                 if((input[1] == "-e") || (input[1] == "-d") || (index == 1))
            //                 {
            //                     cout<<"Directory"<<endl;
            //                 }
            //         }
            //
            else if(cmds.at(i) == ";") 
            {
                cout<<"entered ;"<<endl;
                continue;
            }
            else if(cmds.at(i) == "&&")
            {
                cout<<"Entered &&"<<endl;
                if(status == 0)
                    continue;
                else
                {
                    i++;
                    continue;
                }
            }
            else if(cmds.at(i) == "||") 
            {
                cout<<"entered ||"<<endl;
                if(status)
                    continue;
                else
                {
                    i++;
                    continue;
                }
            }
            else if(cmds.at(i) == "(")
            {
                cout<<"DEBUG MSG HERE TOO"<<endl;
                // //Need to find a way to check previous connectors
                // if((cmd.find("&&") && done == false) || (cmd.find("||") && done == true))
                // {
                //     cout<<"This msg shows!!!!"<<endl;
                //     while(cmds.at(i) != ")")
                //         i++;
                //         cout<<"i is: "<<i<<endl;
                //         continue;
                // }


                //Need to find a way to check previous connectors
                //We we want to ignore the ) sign but run the rest
                if((cmd.find("&&") && done == false) || (cmd.find("||") && done == true))
                {
                        cout<<"This msg shows!!!!"<<endl;
                        if(cmds.at(i) == "&&")
                        {
                            cout<<"Entered &&"<<endl;
                            if(status == 0)
                                continue;
                            else
                            {
                                i++;
                                continue;
                            }
                        }
                        else if(cmds.at(i) == "||") 
                        {
                            cout<<"entered ||"<<endl;
                            if(status)
                                continue;
                            else
                            {
                                i++;
                                continue;
                            }
                        }
                        else if(cmds.at(i) == ")")
                        {
                            i++;
                            continue;
                        }
                }
            }

            int pid = fork();
            forking(pid, input, status);
            }
    }
    cout << "\n";
    return 0;   
}
