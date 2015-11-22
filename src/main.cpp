#include <unistd.h>          
#include <vector>
#include <string>
#include <stdio.h>       
#include <string.h>       
#include <signal.h>
#include <string.h>
#include <iostream>
#include <unistd.h>     
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

// parses a command from the vector of cmds based on the rules of the connectors
void parse(string cmd, vector<string>& cmds)
{       
        //used built in boost library
        //tokenized the characer that is being delimitized 
        char_separator<char> delim(" ",";#[]()");
        tokenizer <char_separator<char> > mytok(cmd, delim);

        //Push back everything the user inputs
       for(tokenizer<char_separator<char> >::iterator it = mytok.begin(); it != mytok.end(); it++)
        {
            cmds.push_back(*it);
        }        
}

//The test command
void test(char* input[], bool& status)
{
    struct stat sb;
    int position = 1;
    //The flags that we are esentially using
    if (input[1] == "-e" || input[1] == "-f" || input[1] == "-d")
        position++;

    if (stat(input[position], &sb) == -1)
    {
        perror("stat error");
        status = false;
    }

    switch (sb.st_mode & S_IFMT)
    {
        case S_IFREG: //Macro for regular files 
            if(position == 1 || input[1] == "-e" || input[1] == "-f")
                status = true;
            else
                status = false;

        case S_IFDIR: //Marcro for directories
            if(position == 1 || input[1] == "-e" || input[1] == "-d")
                status = true;
            else
                status = false;
    }
    status = false;
}

bool forking(char* input[])
{
    pid_t c_pid, pid;
    int status;
   c_pid = fork();
    if( c_pid < 0)
    {
        perror("forking failed");
        exit(1);
    }
    
    else if(c_pid == 0)
    {
       execvp(*input, input);
        perror("execvp failed");
       exit(1);
     }    
    else if(c_pid > 0)
    {
        if( (pid = wait(&status)) < 0)
        {
            perror("wait");
            exit(1);
        }
    }
    if(status != 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

// calls the correct function (test or forking [which calls execvp])
// and updates the status variable depending on which function is called
bool run(char* input[], bool& status)
{
    // if the first command is test, then run test function
    // else run fork
    if(input[0] == "test")
        test(input, status);                
    else    
        return (forking(input));   
}

int main()
{
    bool done = false;
    string userName = getlogin();
    getLogin(userName);
    char hostArray[64]; 
    getHost(hostArray);
    
    while(!done)
    {
        const int LENGTH = 22;
        int counter = 0;     
        string cmd = "";
       string prev = ";";    
        bool status = true;
        char* input[LENGTH];    
        vector <string> cmds;                                                                 
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

        for(unsigned int i = 0; i < cmds.size(); i++)
        {
            int lastCmd = cmds.size()-1;
            if(cmds[i] == "exit")
            {
                if(prev == ";")
                {
                    done = true;
                    break;
                }
                else if(prev == "||")
                {
                    if(status == true)
                        continue;
                    else
                        done = true;
                        break;
               }
                else if(prev == "||")
                {
                    if(status == true)
                        continue;
                    else
                    {
                        done = true;
                        break;
                    }
                }
                else if(prev == "&&")
                {
                    if(status == true)
                    {
                        done = true;
                        break;
                    }
                    else
                        continue;
                }
            }

            //if cmds is any of these connectors
            //it will only run if the previously assigned connector(s)
            //was a success or not based off the connector's logic
            if(cmds[i] == ")" || cmds[i] == ";" || cmds[i] == "||" || cmds[i] == "&&")
            {
                input[counter] = 0;
                if(prev == ";")
                {
                    status = forking(input);
                }
                else if (prev == "||")
                {
                    if(status == false)
                    {
                        status = forking(input);               
                   }
                }
                else if(prev == "&&")
                {
                    if(status == true)
                    {
                        done = true;
                        break;
                    }
                    else
                        continue;
                }
            }
               for(int j = 0; j < LENGTH; j++)
                {
                    input[j] = 0;
                }

                       status = forking(input);                
                    }
                }

                for(int j = 0; j < LENGTH; j++)
                {
                    input[j] = 0;
                }

                prev = cmds[i];
            }

            // if it is NOT a connector, store the command and arguments
            // from cmds[] into input[] to pass into forking(...)
            else if(cmds[i] != "(" || cmds[i] != ";" || cmds[i] != "||" || cmds[i] != "&&")
           {
                char* c = const_cast<char*>(cmds[i].c_str());
                input[counter] = c;
                counter++;
            }


           //This essentially check for precedence and see whether or not to execute 
            //the next command based on the previous connector and if the previous
            //command was executed
            else if(cmds[i] == "(")
            {
                if((prev == "||" && status == true) || (prev == "&&" && status == false))
                {
                    // increment the index until it hits ")" because it is redundant
                    // to check the next command that should NOT be ran and check
                    // when to end the precedence
                    for(unsigned int j =0; cmds[i] != ")"; j++)
                    {
                        i++;
                    }
                }
            }
            //Check whether to run the last command or not
            else if(i == lastCmd)
            {
                char* c = const_cast<char*>(cmds[i].c_str());
                input[counter] = c;
                input[counter + 1] = NULL;

                if(prev == ";")
                {
                    status = forking(input);             
                }
                else if(prev == "&&")
                {
                   if(status == true)
                    {
                        status = forking(input);         
                    }
                }
                else if(prev == "||")
                {
                    if(status == false)
                    {
                        status = forking(input);               
                    }
                }

                for(int j = 0; j < LENGTH; j++)
                {
                    input[j] = NULL;
                }

                counter = 0;
                prev = cmds[i];
           }

           if(done)
          {
            cout<<"Sayonara!!"<<endl;
          }
    }

    cout << "\n";
    return 0;   
}
