Homework 2: Game of Clones
This homework is due on Thursday, Septemeber 28, at 11:59:59 PM (Eastern daylight time). You must use submit to turn in your homework like so: submit cs421_jtang hw2 hw2.c

Your program must be named hw2.c, and it will be compiled on Ubuntu 16.04 as follows:

  gcc ‐‐std=c99 ‐Wall ‐O2 ‐o hw2 hw2.c
(Note the above is dash, dash, "std=c99", and the other flags likewise are preceded by dashes.) There must not be any compilation warnings in your submission; warnings will result in grading penalties. In addition, each code file must be properly indented and have a file header comment, as described on the coding conventions page.
Due to UMBC policy, you may not use the GL systems for this assignment, because you will be using the fork() system call.
In this homework, you will write a program that spawns multiple child processes. The user will then be able to inspect those processes and attempt to kill them.

Part 1: Determine Program Mode
The C program will be divided into several parts as follows. When the process begin, inspect its command line arguments. If there are no arguments (other than the program name itself), the program runs in parent mode. Otherwise, it runs in child mode.

Part 2: Main Menu
When starting up in parent mode, initialize a global counter to zero. Then create an array of 100 structures.

Then display the following menu:

Main Menu:
0. Spawn a child
1. Kill a child
2. Kill random child
3. End program
Read from the user a menu option, and then perform the requested action. Afterwards, return to the main menu.
Part 3: Spawn a Child
When the user selects the first menu option, Create an unnamed pipe with pipe(). Then call fork() to create another child process. The current value of the global counter is this child's unique ID; thus, the first child's ID will be zero. Update the global counter by one afterwards.

In the child process, call dup2() to close stdout and duplicate the writing end of the pipe. Then call execlp(), to re-execute the hw2 program, passing as an argument that child's ID. This results in running the new program in child mode.

When hw2 starts up in child mode, set two signal handlers, one for SIGUSR1 and another for SIGUSR2. Print a message with its child ID to standard error. Finally, go into an infinite loop.

Back in the parent process, close the writing end of the newly created pipe. Store in the array of structures the child's PID, the reading end of the unnamed pipe, and the fact that the child is alive. Then return to the main menu.

Part 4: Kill a Child
When the user selects this menu option, list all child processes that are still alive. Then read from the user which child ID to kill. Send the SIGUSR1 signal to that child process.

When a child receives a SIGUSR1 signal, terminate itself.

Back in the parent, after sending SIGUSR1, reap the child process. Update the child's entry within the array of structures as having terminated. Then return to the main menu.

Part 5: Kill a Random Child
When the user selects this menu option, send SIGUSR2 to all child processes that are still alive.

When a child receives a SIGUSR2 signal, open /dev/urandom. Read the first four bytes into an unsigned int. Then write that unsigned int to stdout. Use write(), not printf(), to write raw bytes to the stdout file descriptor.

Back in the parent, read from the pipe for each child. For the child that wrote the largest unsigned random number, kill that child (by sending it a SIGUSR1, then reaping it). In the unlikely event that more than one child had the largest number, arbitrarily pick one child to kill. Afterwards, return to the main menu.

Part 6: End Program
When the user selects this menu option, send SIGUSR1 to all alive children, and reap those children. Finally, quit the program.

Part 7: Required Documentation
For this assignment, you are required to provide the following documentation. In your hw2.c file add a comment block near the top that answers the following questions.

Question 1: It is necessary to kill and reap child processes to avoid creating orphans. Describing the consequences of having orphans on your system.

Question 2: Suppose the user creates two children processes, by selecting the first menu option twice. The first child process will have three opened file descriptors. Why does the second child have four descriptors instead of three?

Sample Output
Here is a sample output from running the program. Observe that this implementation prints more output than strictly required by this assignment.

Main Menu:
0. Spawn a child
1. Kill a child
2. Kill random child
3. End program
(User enters 0.)
Spawned child #0, PID 15022
Main Menu:
0. Spawn a child
1. Kill a child
2. Kill random child
3. End program
(User enters 0.)
Spawned child #1, PID 15023
Main Menu:
0. Spawn a child
1. Kill a child
2. Kill random child
3. End program
(User enters 1.)
Choose child to kill
0: PID 15022
1: PID 15023
(User enters 1.)
Killed child #1
Main Menu:
0. Spawn a child
1. Kill a child
2. Kill random child
3. End program
(User enters 0.)
Spawned child #2, PID 15024
Main Menu:
0. Spawn a child
1. Kill a child
2. Kill random child
3. End program
(User enters 0.)
Spawned child #3, PID 15025
Main Menu:
0. Spawn a child
1. Kill a child
2. Kill random child
3. End program
(User enters 1.)
Choose child to kill
0: PID 15022
2: PID 15024
3: PID 15025
(User enters 2.)
Killed child #2
Main Menu:
0. Spawn a child
1. Kill a child
2. Kill random child
3. End program
(User enters 2.)
Killed child #0 (its random number was 1249667333)
Main Menu:
0. Spawn a child
1. Kill a child
2. Kill random child
3. End program
(User enters 3.)
Killed child #3
Other Hints and Notes
Ask plenty of questions on the Blackboard discussion board.
At the top of your submission, list any help you receieved as well as web pages you consulted.
Assume the user will always enter a valid menu option, and will only enter a vaild child ID for menu option one.
Assume the user will not spawn more than 100 children.
Assume the user will not select menu option two when no children are alive.
Use the %ld specifier when converting a PID to a string. Use snprintf() to format a string into a memory buffer.
Periodically run the ps -ef command, to check if you have any stray hw2 child processes. Kill them as necessary; see the killall command.
Test child mode by manually running hw2 with an argument. Send signals from the command line to test its signal handlers.
