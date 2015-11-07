# rshell
==========
Licensing Information: READ LICENSE
---
Project source can be downloaded from https://github.com/dphan022/rshell.git

Author & Contributor List
----
David Phan

John Phang

All other known bugs and fixes can be sent to jphan016@ucr.edu

Reported bugs/fixes will be submitted to correction.

## How to install
To install and run rshell, enter the following into a terminal: 

```
1. git clone https://github.com/dphan022/rshell.git

2. cd rshell

3. git checkout hw1

4. make

5. bin/rshell
```

## Features
* runs all executables from the bin folder using execvp()

* connectors ```;```, ```&&```, and ```||``` are supported 

* error handling & checking: prompts messages for invalid syntax and commands

* commenting after a command using ```#``` is supported

* displays user name and host name in the ready prompt ```i.e. jdoe001@alpha023$```

* special built-in command allowing the user to exit by running ```exit``` 

## Test Cases
* ```single_command.script              #tests single commands```
* ```multi_command.script               #tests commands with connectors ";", "&&", or "||"```
* ```commented_command.script           #tests commands with comments``` 
* ```exit.script                        #tests exit and commands with exit```

## Known bugs and unsupported features
* arrow keys are not supported ```i.e. the up arrow key which allows you to run the previous command```

* due to no arrow keys support, text cursor navigation is not supported as well

* color highlighting for directories, executables, and such are not supported

* some commands are not supported because they are built-in to bash ```i.e. cd```
