//***********************************************************
//File Name: hw2.c
//Author Name: Dominic Rollo
//Assignment: Homework 2
//
//Description: User is given a menu that allows them to spawn
//	child processes, kill them, and exit the program.
//
//**************Outside Help*********************************
//  http://tldp.org/LDP/lpg/node11.html
//
//***********************************************************
//**************Questions************************************
// Q1: Orphans use up resources on your computer.  This can 
//	lead to the computer lagging and even possible crashing.
//
// Q2: When a child is created it is given a copy of all the
// current variables.  This includes the read pip for the
// previous child.
//
//***********************************************************
#define _POSIX_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define PROCESS_DIRECTORY_SIZE 100  //remeber to make extendable

typedef struct Process{
	pid_t pid_id;
	int com_pipe;
	int is_running;
	int child_num;
} process;

int counter;
struct Process *running_proc;
int reader;

void print_menu();//prints menu
void interpret(int option);//interprets input
void spawn_child();//spawns child
void choose_child();//allows user to choose child
void kill_child(int child);//kills child process
void print_children();//prints running children
void setup_signal();//setups signal handle
static void fault_handler();//handles signals
void kill_all();//kills all running child processes
void russian_roulette();//randomly choose child to kill

int main(int argc, char *argv[]){

	// Child mode
	if (argc>1){
		setup_signal();
		fprintf(stderr, "Spawned child #%s, PID %d\n", argv[1], getpid());
		while(1){}
	}

	// Parent mode
	int input;	
	counter=0;
	running_proc=malloc(sizeof(struct Process)*PROCESS_DIRECTORY_SIZE);
	do {
		//if (counter>0)
		//	wait(running_proc[counter-1].pid_id);
		usleep(150000);
		print_menu();
	
		if (scanf("%d", &input)==1){
			printf("(User entered %d.)\n", input);
			interpret(input);
		}
		else 
		printf("Could not read that value. Try again.\n");
	}while (input != 3);
		
	free(running_proc);
	return 0;
}

//Prints out the menu options
void print_menu(){
	printf("Main Menu:\n");
	printf("0. Spawn a child\n");
	printf("1. Kill a child\n");
	printf("2. Kill a random child\n");
	printf("3. End program\n");
}

//interprets the user option and calls appropriate function
void interpret(int option){

	switch (option){
		case 0:
			spawn_child();
			break;
		case 1:
			choose_child();
			break;
		case 2:
			russian_roulette();
			break;
		case 3:
			kill_all();
			break;
		default:
			printf("\tNot a valid option. Try again.\n");
			break;
	}
}


//Spaws a child.  Passes it's PID back trhough a pip to the adult
//**************************************************************
void spawn_child(){	
	int fd[2];
	if (pipe(fd)==-1){
		printf("Pipe error\n");
		return;
	}

	pid_t pid = fork();

	if (pid<0)
		printf("Fork fialed\n");
	else if (pid == 0){
		dup2(fd[1], 1);
		char temp[3];
		snprintf(temp, 3, "%d", counter);
		execlp("./hw2","hw2", temp, NULL);
	}
	else if (pid > 0){
		running_proc[counter].com_pipe=fd[0];
		running_proc[counter].is_running=1;
		running_proc[counter].pid_id=pid;
		counter++;
	}
}

// Displays all the currently running children and ask user 
//  which one to kill
//**************************************************************
void choose_child(){
	int to_kill;
	printf("Choose child to kill\n");
	print_children();
	
	do {
		to_kill='\0';
		if (scanf("%d", &to_kill)==1){
				printf("(User entered %d.)\n", to_kill);
				kill_child(to_kill);
			}
			else 
				printf("Could not read that value. Try again.\n");
		} while(!to_kill);
}

// Kills a child based of it place in the runninf_proc array
//**************************************************************
void kill_child(int child){
	kill(running_proc[child].pid_id, SIGUSR1);
	wait(NULL);
	running_proc[child].is_running = 0;
	printf("Killed child %d\n", child);
}

// sets up the signal handler
//**************************************************************
void setup_signal(){
	sigset_t mask;
	sigemptyset(&mask);
	struct sigaction sa ={
		.sa_handler = fault_handler,
		.sa_mask = mask,
		.sa_flags = 0
	};
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);
}

static void fault_handler(int signum){
	if (signum == SIGUSR1)
		exit(0);
	else if (signum == SIGUSR2){
		FILE *ran_f = fopen("/dev/urandom", "r");
		char ran_byte[4];
		for(int i=0; i<4;i++){
			ran_byte[i]= fgetc(ran_f);
		}
		fclose(ran_f);
		if (write(1, ran_byte, 4)==0)
			fprintf(stderr, "write failed\n");
	}
}

// kills all of the running child processes
//**************************************************************
void kill_all(){
	int i =0;
	while (i<counter){
		if (running_proc[i].is_running)
			kill(running_proc[i].pid_id, SIGUSR1);
		i++;
	}
}

// kills the child process with the highest random value
//**************************************************************
void russian_roulette(){
	int high_pid=0;
	unsigned int high_value, temp_value;
	char test[4];
	high_value=0;
	for(int i=0; i<counter; i++){
		if (running_proc[i].is_running){
			kill(running_proc[i].pid_id, SIGUSR2);
			if (read(running_proc[i].com_pipe, test, 4)==0)
				fprintf(stderr, "read fail\n");
			memcpy(&temp_value, test, 4);
			printf("%d: %u\n", i, temp_value);
			if (temp_value > high_value){
				high_value=temp_value;
				high_pid = i;
			}
		}
	}
	printf("\bHighest processes is %d and it is %u\n", high_pid, high_value);
	kill(running_proc[high_pid].pid_id, SIGUSR1);
	wait(NULL);
	running_proc[high_pid].is_running = 0;
	
}

// Prints the current child processes running
//**************************************************************
void print_children(){
	for (int i=0; i<counter; i++)
		if (running_proc[i].is_running)
			printf("%d: PID %d\n", i, running_proc[i].pid_id);
}