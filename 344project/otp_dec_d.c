/*
Author: James Palmer
File Name: otp_dec_d.c
Project 4 final

*/


#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#define MAX 5
//server side program that childDecryptions cipher

void childDecryptionion(int fileLocation, int cipherFileText) {
	
	int amountDataProcc = 0;
	int dataReceived = 0;
	int dataReturned = 0;
	char childDecryptionArray[MAX];
	char childDecryptionKeyWord[MAX];
	int i;

	
	//seperated into a child process
	while (amountDataProcc < cipherFileText) {	
		//reset buffer child to zero
		memset((char *)&childDecryptionArray, '\0', sizeof(childDecryptionArray));	
		//default the given keyword to zero
		memset((char *)&childDecryptionKeyWord, '\0', sizeof(childDecryptionKeyWord));	
		//receive ciphered file and process
		dataReceived = read(fileLocation, childDecryptionArray, MAX);			
		
		//error check to see if otp_enc_d exists
		if (dataReceived < 0) {		
			
			printf("File: otp_enc_d ERROR!");
			exit(1);
		}

		
		//display given data
		dataReturned = write(fileLocation, childDecryptionArray, dataReceived);  
		
		//error check again for myself
		if (dataReturned < 0) {									 
			printf("File:otp_enc_d ERROR!");
			exit(1);
		}
		
		//receive file chars then send into childDecryptionKeyWord buffer
		dataReturned = read(fileLocation, childDecryptionKeyWord, dataReceived); 
		
		//error check based on homework requirements
		//if childDecryptionKeyWord is smaller than given text = error 
		if (dataReturned < dataReceived) {					
			printf("file: otp_enc_d ERROR! childDecryptioned file smaller than given text");
			exit(1);
		}

		
		//subtract childDecryptionKeyWord code 
		for (i = 0; i < strlen(childDecryptionArray) - 1; i++) {
			//make whitespace from ciphertext file for clarity		
			if (childDecryptionArray[i] == ' ') { childDecryptionArray[i] = 91; } 
			
			//from childDecryptionKeyWord file: temporarily make 'space' to be '['
			if (childDecryptionKeyWord[i] == ' ') { childDecryptionKeyWord[i] = 91; }
			
			//from given call do the following
			//alphabet is 26 letters so subtract the file size down to 27 = A-Z
			//expanded array to 27 to prevent basic error or 0-26 size array
			childDecryptionArray[i] = childDecryptionArray[i] - 65;
			
			//same as above with array but with the given word file			
			childDecryptionKeyWord[i] = childDecryptionKeyWord[i] - 65;								

			//more error checking if range is above the given 27 words
			if (childDecryptionArray[i] < 0 || childDecryptionArray[i] >= 27) {		
				fprintf(stderr, "otp_enc_d error : input contains bad characters\n");
				//see if file was closed
				//printf("test newsocket") //check
				
				
				//class forum suggested the following to prevent overflow
				//close port later
				shutdown(fileLocation, SHUT_RDWR);				
				break;											
			}

			
			//as per homework
			//subtract childDecryptionKeyWord ascii code from text ascii 
			childDecryptionArray[i] = childDecryptionArray[i] - childDecryptionKeyWord[i];
			
			//pad if smaller than 0
			if (childDecryptionArray[i] < 0) {childDecryptionArray[i] += 27;}
			
			//crop file if value exceeds file size 27
			childDecryptionArray[i] = childDecryptionArray[i] % 27;
			
			//now convert to A-Z
			childDecryptionArray[i] = childDecryptionArray[i] + 65;	

			//create whitespace for clarity
			if (childDecryptionArray[i] == 91) { 
				childDecryptionArray[i] = ' '; 
			}   
		}

		 //final file write to file
		dataReturned = write(fileLocation, childDecryptionArray, dataReceived); 
		amountDataProcc += dataReceived;
	}
}


//main file prompt
int main (int argc, char* argv[]){

	//start with port number
	//hold server info
	int fileNamePort = atoi(argv[1]);
	int fileName, fileLocation;	
	struct sockaddr_in server, client;						
	socklen_t clientLength = sizeof(client);																									
	pid_t pid;												

	
	//note for TA, am I done yet? Is this pretty enough?
	//copy 0 into the server 
	memset(&server, 0, sizeof(struct sockaddr_in));			

	//as given in video: AF_INET for IP;
	//if doesnt exist: error
	if ((fileName = socket(AF_INET, SOCK_STREAM, 0)) == -1) {	
		printf("File: otp_enc_d ERROR! stream doesn't exist");
		exit(1);
	}

	//school server connections
	server.sin_family = AF_INET;
	server.sin_port = htons(fileNamePort);
	server.sin_addr.s_addr = INADDR_ANY;				
    
															
	//bind the socket for this server to a port address associated with struct server													
	if (bind(fileName, (struct sockaddr *)&server, sizeof(server)) == -1) {  
	
		//bind chosen port with previously created port 
		//error if no binding fails
		printf("File: otp_enc_d ERROR! AGAIN?? No binding call");							
		exit(1);
	}

	//1-5 connections needed to queue
	if (listen(fileName, 5) == -1) {		
		//more errors if not enough connections
		printf("File: otp_enc_d ERROR! Failed to listen");			
		exit(1);
	}

	//infinite while loop up to as many requested files 
	while (1) {

		//accepts fileName given then continues
		fileLocation = accept(fileName, (struct sockaddr*)&client, &clientLength); 

		//I am a fan of error checking
		if (fileLocation == -1) {												
			printf("File: otp_enc_d ERROR! Cannot accept your call, try refunding your phone account");
		}

		//by book definition do the following
		//fork our child process for file encryption
		pid = fork();											
		if (pid  < 0) {	
			//if return is negitive, error message again
			printf("File: otp_enc_d ERROR! It is illegal to fork a child");
		}
		
		//start child process
		if (pid == 0) {											
			
			//need more variables
			int cipherFileText;									
			char childDecryption;
			
			//verify file and proceed
			read(fileLocation, &childDecryption, sizeof(char));			
			if (childDecryption != '@') {		
				//if not varified, state error client
				childDecryption = 0;		
				
				//state error, if occured in fork, exit
				write(fileLocation, &childDecryption, sizeof(char));		
				exit(1);										
			}
			
			else {
				//print error check from client
				childDecryption = 1;
				write(fileLocation, &childDecryption, sizeof(char));		
			}
			
			//final read file then output to text file
			read(fileLocation, &cipherFileText, sizeof(int));
			write(fileLocation, &cipherFileText, sizeof(int));			

			//finally print the file
			childDecryptionion(fileLocation, cipherFileText);
			exit(0);
		}
		
		//parent paired else statement
		else {				
			//continue to establish connection after child has finished
			int parentStatus;
			waitpid(-1, &parentStatus, WNOHANG);			
			continue;
		}
	}
	
	//close files to prevent overflow
	close(fileLocation);
	close(fileName);

	exit(0);
}
