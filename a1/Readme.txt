V00953462
A01
Kelly Lemaire

# How to Run PMan

Add some files you want PMan to be able to run as child processes in the directory.
Compile those files.  
To compile main.c and linked_list.c, run `make` in the command line.
To run PMan, type `./pman` into the command line. 
You should see `PMan: >` printed in your terminal. You can input the following commands: 

## bg

bg will create a child process to run a file or command.
To run bg, type `bg` followed by a linux command, or a file path and it's arguments. 
The program will proceed to run the command or the program associated with the given file path. 
The program will print an error to the command line if something goes wrong. This error could be because the file does not exist. 

Ex. 
bg ls : this should print all of the files in the current directoy 
bg ./test : assuming that test exists in the current directory, the program will create a child process to run test

## bglist

bglist will display all of the programs currently executing in the background. Only programs opened with PMan will be displayed. 
Type `bglist` to run this command. 

## bgkill pid

Terminates the job with the process ID pid. 
The pid must be one that was started with PMan, or an error message will be printed to the command line. 
Type `bgkill pid` to run this command. 

## bgstop pid

Temporarily stops the job with the process ID pid. 
The pid must be one that was started with PMan, or an error message will be printed to the command line. 
Type `bgstop pid` to run this command. 

## bgstart pid

Re-starts a job that has been stopped. 
The pid must be one that was started with PMan, or an error message will be printed to the command line. 
Type `bgstart pid` to run this command. 

## pstat pid

Prints information associated with the process ID, pid. 
Prints out the comm, state, utime, stime, rss, voluntary ctxt switches and nonvoluntary ctxt switches to the command line. 

## q

Type `q` in the command line to stop Pman. 


