/*
Author: James Palmer
File Name: otp_dec.c
Project 4 final
File is almost the same as otp_enc.c
*/


#include <arpa/inet.h>
#include <fcntl.h>     
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>     
#include <stdlib.h>   
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <unistd.h>


//size larger than the file to prevent overflow
#define SIZE    16000

int main(int argc, char *argv[]){
	
    
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[SIZE];
    char keyBuffer[SIZE];
    char secondaryBuffer[1];
	int fileName, fileLocation, n, i;
    int fileInfo, fileSize, givenFileSize, infoReceived, sentInfo;

    //First check number of arguments from given file
    if (argc < 4)
    {
        printf("File open: otp_dec ciphertext key port\n");
        exit(1);
    }

		// This should be the encrypted file that is read only
    fileLocation = atoi(argv[3]);
    fileInfo = open(argv[1], O_RDONLY);

    if (fileInfo < 0)
    {
        printf("Error: cannot open ciphertext file %s\n", argv[1]);
        exit(1);
    }

    contents and file length check
    givenFileSize = read(fileInfo, buffer, SIZE);

    // Check for bad characters 
    for (i = 0; i < givenFileSize - 1; i++)
    {
        if ((int) buffer[i] > 90 || ((int) buffer[i] < 65 && (int) buffer[i] != 32))
        {
            printf("File: otp_dec ERROR! bad text contained in ciphertext\n");
            exit(1);
        }
    }

	//close file
    close(fileInfo);

    // Repeat process for new file
    fileInfo = open(argv[2], O_RDONLY);

	//I like error checking
    if (fileInfo < 0)
    {
        printf("Error: cannot open desired key file %s\n", argv[2]);
        exit(1);
    }

    // Get contents and length of desired file 
    fileSize = read(fileInfo, keyBuffer, SIZE);

    //file check
    for (i = 0; i < fileSize - 1; i++)
    {
        if ((int) keyBuffer[i] > 90 || ((int) keyBuffer[i] < 65 && (int) keyBuffer[i] != 32))
        {
            printf("File:otp_dec ERROR! Key text is bad \n");
            exit(1);
        }
    }
	
	//close file
    close(fileInfo);

    // Key file must equal input file
    if (fileSize < givenFileSize)
    {
        printf("Error: key '%s' is shorter than input file\n", argv[2]);
    }

    // Start socket portion: 
    fileName = socket(AF_INET, SOCK_STREAM, 0);
    if (fileName < 0)
    {
        printf("File: otp_enc_d on port ERROR! could not connect: %d\n", fileLocation);
        exit(2);
    }

    memset(&serv_addr, '\0', sizeof(serv_addr));

    server = gethostbyname("localhost");
    if (server == NULL)
    {
        printf("File: otp_dec_d ERROR! could not connect\n");
        exit(2);
    }    
  
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);         
    serv_addr.sin_port = htons(fileLocation);

 
    if (connect(fileName, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("File: top_dec_d ERROR! port: %d could not connect \n", fileLocation);
        exit(2);
    }

    
    //write input file, again
    sentInfo = write(fileName, buffer, givenFileSize - 1);
	
	//error message output for testing, again
    if (sentInfo < givenFileSize - 1)
    {
        printf("File: top_dec_d ERROR! port: %d could not send %d\n", fileLocation);
        exit(2);
    }

    memset(secondaryBuffer, 0, 1);
    
    //server connection
    infoReceived = read(fileName, secondaryBuffer, 1);
    if (infoReceived < 0)
    {
       printf("File: otp_dec_d ERROR! no server connection\n");
       exit(2);
    }

    //Output and write given homework key, like before	
    sentInfo = write(fileName, keyBuffer, fileSize - 1);
	//more error output
    if (sentInfo < fileSize - 1)
    {
        printf("File: otp_dec_d ERROR! port: %d failed to send \n", fileLocation);
        exit(2);
    }

    //per office hours, I need to clear buffer to pervent overflow
    memset(buffer, 0, SIZE);

    do
    {
        //read given encrypted text 
        infoReceived = read(fileName, buffer, givenFileSize - 1);
    }
	
	//if received
    while (infoReceived > 0);

		if (infoReceived < 0){
			printf("File: otp_dec_d ERROR! ciphertext issue \n");
			exit(2);
		}

    //Print output
    for (i = 0; i < givenFileSize - 1; i++)
    {
        printf("%c", buffer[i]);
    }
    printf("\n");

	
	//final close of file
    close(fileName);

    return 0;
}