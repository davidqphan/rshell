# rshell
==========
Licensing Information: READ LICENSE
---
Project source can be downloaded from https://github.com/dphan022/rshell.git

Author & Contributor List
----
David Phan (dphan015@ucr.edu)

John Phang (jphan016@ucr.edu)

All other known bugs and fixes can be sent to jphan016@ucr.edu

Reported bugs/fixes will be submitted to correction.

## How to install
To install and run rshell, enter the following into a terminal: 

```
1. git clone https://github.com/dphan022/rshell.git

2. cd rshell

3. git checkout hw2

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

## Test Command
The square brackets [ ] are actually set up as the test command in bash. This command returns 0 (TRUE) if the test
succeeds and 1 (FALSE) if the test fails. This command is very useful for writing conditions that
can be combined with && and || to write more complex bash command structures.
Your subset of the test command should allow the user to run the command using the keyword ```test```:
test -e /test/file/path

Additionally, the rshell should allow the user to use the symbolic version of the command
[ -e /test/file/path ]

It also allows the user to run tests using the following flags:
* ```-e``` checks if the file/directory exists
* ```-f``` checks if the file/directory exists and is a regular file
* ```-d``` checks if the file/directory exists and is a directory
If a user does not specify a flag, then the -e functionality will be used by default

## Precedence Operators
Additionally, the rshell has ( ) as precedence operators. The parentheses ( ) operators are used to 
change the precedence of the returns of commands, connectors, and chains of connectors. 
For example ```echo A && echo B || echo C && echo D```
Would print the following:
A
B
D

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

* ```echo "hello"``` includes the quotes , it should only print ```hello```
