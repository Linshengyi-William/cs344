// CS 344 Program 3
// Author: James Palmer
// shell will support three built in commands: exit, cd, and status. It will also support comments, which are lines beginning with the # character.




#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>    
#include <errno.h>     
#include <fcntl.h>     
#include <limits.h>    
#include <signal.h>    
#include <sys/stat.h>  // for stat 
#include <sys/types.h> // for pid_t
#include <sys/wait.h>  // for waitpid
#include <unistd.h>    // for exec

#define errorCheck             0 
#define MAX_ARGS        512 // maximum arg per command line
#define MAX_LENGTH     2048 // maximum length of command line
#define MAX_PIDS       1000 

// define bool as type
typedef enum { false, true } bool;

//global variables
int finishedIndex = 0;
int current = 0;                   
int sigNum = 0;
pid_t pidNum[MAX_PIDS];         
pid_t maxPid[MAX_PIDS]; 
pid_t fgpid = INT_MAX;         

//globals functions
void backgroundProcc(int sig, siginfo_t* info, void* vp);
void sigProcc();




void backgroundProcc(int sig, siginfo_t* info, void* vp){
    if (errorCheck)
    {
        printf("Current Background Status.\n");
    }

    pid_t ref_pid = info->si_pid; 

    // if no fg process, process it here
    if (ref_pid != fgpid && finishedIndex < MAX_PIDS)
    {
        // add to completed bg process array so message can
        // be displayed in main loop
        maxPid[finishedIndex++] = ref_pid;
    } 

    return;
}



void sigProcc(){
    // if interrupt signal occurrents while fg process is running, kill it
    if (fgpid != INT_MAX)
    {
        // kill the foreground process
        kill(fgpid, SIGKILL);
 
        // set global variable for status messages
        sigNum = 2;  
    }  

    // ignore interrupt signal for all other processes
    // and simply return
    return;
}


int main(int argc, char** argv){
	
	
    // declare variables
    bool isBackgroundProcess = false;
    bool repeat = true;
    char *args[MAX_ARGS + 1];
    char input[MAX_LENGTH];
    char *placeHolder;
    int first, second, i, j;
    int argCount;
    int status;
	int backgroundExit;
	int backgroundStatus; 
    int finalExit;
	pid_t cpid;

    // create instance of sigaction struct for background processes then single handler
    struct sigaction backgroundTask;
    backgroundTask.sa_sigaction = backgroundProcc;     
    backgroundTask.sa_flags = SA_SIGINFO|SA_RESTART;
    sigfillset(&(backgroundTask.sa_mask));
    sigaction(SIGCHLD, &backgroundTask, NULL);

    // create instance of sigaction struct for foreground processes then single handler
    struct sigaction foreground_act;
    foreground_act.sa_handler = sigProcc;
    foreground_act.sa_flags = SA_RESTART;
    sigfillset(&(foreground_act.sa_mask));
    sigaction(SIGINT, &foreground_act, NULL); 

    // create sigaction struct to ignore interrupts the rest of the time
    struct sigaction restOfTheTime_act;
    restOfTheTime_act.sa_handler = SIG_IGN;
    restOfTheTime_act.sa_flags = SA_RESTART;
    sigfillset(&(restOfTheTime_act.sa_mask));
    sigaction(SIGINT, &restOfTheTime_act, NULL); 

			//default array to start background processes
			for (i = 0; i < MAX_PIDS; i++){  
				maxPid[i] = pidNum[i] = INT_MAX;  
			}   

			
			for (i = 0; i <= MAX_ARGS; i++){
				args[i] = (char *) malloc((MAX_LENGTH + 1) * sizeof(char)); 
			}  

			do{
				
				char **nextArg = args;
				// initialize array of pointers to the strings in the arg array
				for (i = 0; i <= MAX_ARGS; i++){
					strcpy(args[i], "\n");
				}

				//buffer added because class server has overflow problems
				strcpy(input, "\0");  
		 
				i = 0;
					//as stated in homework, print process and exit status
				while (i < MAX_PIDS && maxPid[i] != INT_MAX){
					if (errorCheck){
						printf("Cleaning up Pid process %d\n", maxPid[i]);
					}

					
					maxPid[i] = waitpid(maxPid[i], &backgroundStatus, 0);

					if (WIFEXITED(backgroundStatus)){
						backgroundExit = WEXITSTATUS(backgroundStatus);
						printf("background pid %d is: exit value %d.\n", maxPid[i], backgroundExit);
					}
					else{
						backgroundExit = WTERMSIG(backgroundStatus);
						printf("background pid %d is: terminated by: %d\n", maxPid[i], backgroundExit);
		 
					}

					// remove ps from open background process array then cycle through array of background processes
					j = 0;
					while (j < MAX_PIDS && pidNum[j] != INT_MAX){ 
						if (pidNum[j] == maxPid[i]){
							if (errorCheck){
								printf("Removing process %d from array.\n", pidNum[j]);
							}                   

							
							pidNum[j] = INT_MAX;
		 
							
							int k = j;                       
							while (k + 1 < MAX_PIDS && pidNum[k+1] != INT_MAX){
								pidNum[k] = pidNum[k+1];
								pidNum[k+1] = INT_MAX;
								k++;
							}    
							// adjust current index value & make room for new PID  
							current--; 
						}
						// increment counter
						j++;
					}

					// replace value of  completed process
					maxPid[i] = INT_MAX;

					// increment counter
					i++; 
				}

				// reset completed bg process array index tracker
				finishedIndex = 0;
				fflush(stdin);  //flush out prompt
				fflush(stdout);
				
				printf(": "); // prompt and acquire user for input
				fgets(input, MAX_LENGTH, stdin);

				fflush(stdin);

				// check for empty space
				if (input[0] == '\n' || input[0] == '\0'){
					continue;
				}
		 
				// process and parse input
				argCount = 0;
				placeHolder = strtok(input, " "); // check for multiple args

				if (errorCheck){
					printf("placeHolder is %s\n", placeHolder); 
				}

				//processing args loops
				while (placeHolder != NULL && argCount < MAX_ARGS){
		 
					
					if (strlen(placeHolder) == 0){
						continue;
					}   

					if (errorCheck){
						printf("Attempting to overwriting %s with %s\n", *nextArg, placeHolder);
					}

					// copy currentrent arg to arg array
					strcpy(*nextArg, placeHolder);

					if (errorCheck){
						printf("args count:[%d]  is:  %s\n", argCount, args[argCount]); 
					}
					argCount++;
		 
					// get nextArg arg, if any
					placeHolder = strtok(NULL, " ");

					// increment pointer if another exists
					if (placeHolder != NULL){
						*nextArg++;
					} 
				}

				if (errorCheck){
					printf("overwriting %s", *nextArg);
				}

				// remove newline char from last arg, if any
				placeHolder = strtok(*nextArg, "\n"); 
				if (placeHolder != NULL){
					strcpy(*nextArg, placeHolder);
				}

				if (errorCheck){
					printf("The next Arg: %s\n", *nextArg);
					printf("Current args[%d] is: %s\n", argCount - 1, args[argCount - 1]); 
				}

				// if command for background process
				if (strcmp(args[argCount - 1], "&") == 0){
				   
					isBackgroundProcess = true;
					argCount--; 

				}
				else{
				   
					*nextArg++;
				}

				if (errorCheck){
					printf("Currently overwriting %s with NULL\n", *nextArg);
				}

				if (strncmp(args[0], "#", 1) == 0){
				   
				}
				else if (strcmp(args[0], "exit") == 0){

					// kill any processes/ start fresh
					i = 0;
					while (i < MAX_PIDS && pidNum[i] != INT_MAX){
						if (errorCheck){
							printf("Now killing process: %d\n", pidNum[i]);
						}
		 
						kill(pidNum[i], SIGKILL);
						i++;
					}

					// free previously allocated memory then exit
					for (i = 0; i <= MAX_ARGS; i++){
						if (errorCheck){
							printf("Freeing memory for args[%d], which holdsthe value of: %s\n",i, args[i]);
						} 
						free(args[i]); 
					}  
					repeat = false;

				}
				else if (strcmp(args[0], "cd") == 0){ 

					// if no args exist =directory specified in HOME 
					if (argCount == 1){
						chdir(getenv("HOME"));
					}
					// if one arg exists = change to dir provided
					else{
						chdir(args[1]);
					}
				  
				}
				else if (strcmp(args[0], "status") == 0){  

					if (WIFEXITED(status)){
						finalExit = WEXITSTATUS(status);
						printf("The exit value is :%d\n", finalExit); // print exit status
					}
					else if (sigNum != 0){
						printf("Program terminated by signal: %d\n", sigNum);
					} 

				}
				else {
				   
					cpid = fork();
				 //child process starts here
					if (cpid == 0){
						bool checkStatus = false; 
						bool redirectInput = false;
						bool redirectOutput = false;
						int inputOffset = 0;
						int outputOffset = 0;

						if (argCount > 4 && strcmp(args[argCount-4], "<") == 0){
							if (errorCheck){
								printf("First input redirected to: %s\n", args[argCount-3]);     
							}

							redirectInput = true;

							// set target for input path
							inputOffset = 3; 
						}
						
						else if (argCount > 2 && strcmp(args[argCount-2], "<") == 0){
							
							if (errorCheck){
								printf("Second input redirected to: %s\n", args[argCount-1]);     
							}

							// set flag to redirect input
							redirectInput = true;

							inputOffset = 1; 
						}
						
						if (argCount > 4 && strcmp(args[argCount-4], ">") == 0){
							
							if (errorCheck){
								printf("Third output redirected to: %s\n", args[argCount-3]);     
							}
		 
							// set flag to redirect output
							redirectOutput = true;

							outputOffset = 3; 
						}
						
						else if (argCount > 2 && strcmp(args[argCount-2], ">") == 0){
							
							if (errorCheck){
								printf("Fourth output redirected to: %s\n", args[argCount-1]);     
							}
							//see above
							redirectOutput = true;
							outputOffset = 1; 
						}

						//here I redirect background processes if they are non existent
						if (isBackgroundProcess == true && redirectInput == false){
							
							first = open("/dev/null", O_RDONLY);

							checkStatus = true;      
						}
						else if (redirectInput == true){
							
							first = open(args[argCount - inputOffset], O_RDONLY);

							checkStatus = true;  
						}

						if (checkStatus == true){
							
							if (first == -1){
								
								printf("First Smallish: cannot open %s for input\n", args[argCount - inputOffset]);
								exit(1); 
							}

							second = dup2(first, 0);
							
							if (second == -1){
								
								printf("Second Smallish: cannot open %s for input\n", args[argCount - inputOffset]);
								exit(1);
							}   
						}

						if (redirectOutput == true){
							
							first = open(args[argCount - outputOffset], O_WRONLY|O_CREAT|O_TRUNC, 0644);

							if (first == -1){
								
								printf("Third Smallish: cannot open %s for output\n", args[argCount - outputOffset]);
								exit(1); 
							}

							second = dup2(first, 1);
							
							if (second == -1){
								
								printf("Fourth Smallsh: cannot open %s for output\n", args[argCount - outputOffset]);
								exit(1);
							}   
						}

						//show offsets if any
						i = 0;
						if (inputOffset > outputOffset){
							
							i = inputOffset + 1;
						}
						else if (outputOffset > inputOffset){
							
							i = outputOffset + 1;
						}

						// move the pointer to omit the input redirection from array
						for (j = i; j > 0; j--){
							
							*nextArg--;
						}

						// add NULL pointer to last array index
						*nextArg = NULL;
						//homework example to use Unix built-in
						execvp(args[0], args);

						//Error proofing if given a bad filename
						printf("%s", args[0]);
						fflush(NULL);
						perror(" ");  
		 
						exit(1); 
					}
					
					else if (cpid == -1){   
					
						printf("%s", args[0]);
						fflush(NULL);                 
						perror(" ");
					} 
					
					
					//parent process begins here
					else{
						

						// if command for parent process, proint output and process id's
						if (isBackgroundProcess == true){
							
							
							printf("The background pid ID is: %d\n", cpid);

							isBackgroundProcess = false;

							if (current < MAX_PIDS){  
							
								pidNum[current++] = cpid;
							}
						} 
						
						//assign global varibles to single handlers
						else{
						
							sigNum = 0;                     
							fgpid = cpid;
		 
							sigaction(SIGINT, &foreground_act, NULL);

							fgpid = waitpid(fgpid, &status, 0);
							sigaction(SIGINT, &restOfTheTime_act, NULL);    
							fgpid = INT_MAX;

							// Print error message if process fails
							if (sigNum != 0){
								
								printf("This process was terminated because of: %d\n", sigNum);
							}   
						}
					}
				}

			} 
			
	//throw in a user loop command to exit per assignment
    while(repeat == true);

    return 0;
}

