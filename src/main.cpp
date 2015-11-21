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
#include <boost/tokenizer.hpp> //The tokenizer we are using for this program

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
string parse(int semicolon, int connectorAnd, int connectorOr, bool& invalid,
             string cmd, vector <string>& cmds, string tempCmd)
{
    //comes from the built in boost library
    //It basically breaks a sequence of characters into tokens based
    //on character by using the delimiter function
    char_separator<char> delim(" ;&|#()[]",";&|#()[]", keep_empty_tokens); 
    tokenizer< char_separator<char> > mytok(cmd, delim); //pass in a string of command and tokenize it

    tempCmd = "";
    //Format the sequence of connectors in it's rightful place
    //e.g. echo hello world ; echo hi
    //else prompt an error for not using connetors correctly
    for(tokenizer<char_separator<char> >::iterator it = mytok.begin(); it != mytok.end(); it++)
    {
        //Iterator *it is a pointer that is declared in order to traverse the elements in 
        //a range of elements
        if(*it == ""); //This will do nothing  if it come across a space

        else if(*it == "(")
        {
            cout<<"Entered ( looped"<<endl;
            cmds.push_back(tempCmd);
            tempCmd = "";
            cmds.push_back("(");
            cout<<"exiting ( looped...."<<endl;

        }
        else if(*it == ")")
        {
            cout<<"Entered ) looped"<<endl;
            cmds.push_back(tempCmd);
            tempCmd = "";
            cmds.push_back(")");


            cout<<"exiting ) looped...."<<endl;
        }

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
            else if(semicolon == 1)
            {
                if(connectorOr == 0 && connectorAnd == 0)
                {
                    if(tempCmd == "")
                    {
                        perror("Error no arguments before ;");
                        invalid = true;
                        break;
                    }
       
                    cmds.push_back(tempCmd);
                    tempCmd = "";
                    cmds.push_back(";");
                    semicolon = 0;
                }
                else
                {
                    perror("Error, invalid syntax");
                    invalid = true;
                    break;
                }
            }
            
        }
        else if(*it == "&") 
        {
            connectorAnd++;
            if(connectorAnd > 2)
            {
                perror("Error using &&");
                invalid = true;
                break;
            }
            else if(connectorAnd == 2)
            {
                if(connectorOr == 0 && semicolon == 0 )
                {
                    if(tempCmd == "")
                    {
                        perror("No arguments before &&");
                        invalid = true;
                    }
                    cmds.push_back(tempCmd);
                    tempCmd = "";
                    cmds.push_back("&&");
                    connectorAnd = 0;
                }
                else
                {
                    perror("Error, invalid syntax");
                    invalid = true;
                    break;
                }
            }
        }
        else if(*it == "|") 
        {
            connectorOr++;
            if(connectorOr > 2)
            {
                perror("Error using ||");
                invalid = true;
                break;
            }
            else if(connectorOr == 2)
            {
                if(connectorAnd == 0 && semicolon == 0)
                {
                    if(tempCmd == "")
                    {
                        perror("Error no arguments before ||");
                        invalid = true;
                        break;
                    }
                    cmds.push_back(tempCmd);
                    tempCmd = "";
                    cmds.push_back("||");
                    connectorOr = 0;
                }
                else
                {
                    perror("Error, invalid syntax");
                    invalid = true;
                    break;
                }
            }
            
        }       
        else
        {
            if(semicolon != 0 || connectorOr != 0 || connectorAnd != 0)
            {
                perror("Error, invalid syntax");
                invalid = true;
                break;
            }
            if(tempCmd != "")
                tempCmd += ' ';
            tempCmd += *it;
        }
    }
    return tempCmd;
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
    bool done = false; //finish
    string userName = getlogin();
    getLogin(userName);
    char hostArray[64]; 
    getHost(hostArray);
    
    while(!done)
    {
        int status;
        string cmd = "";
        vector <string> cmds;
        int semicolon = 0;
        int connectorAnd = 0;
        int connectorOr = 0;

        bool invalid = false; //user inputted an invalid bash commmand

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

        //Tokenizing and Parsing
        string tempCmd;
        tempCmd = parse(semicolon, connectorAnd, connectorOr, invalid, cmd, cmds, tempCmd);

        cmds.push_back(tempCmd.c_str());
        tempCmd = ""; // Reset tempCmd back to empty to get the next cmd from cmds

        if(!invalid)
        {
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

            if(done)
            {
                cout << "Sayonara!" <<endl;
                break;
            }

        }
    }
    cout << "\n";
    return 0;   
}
