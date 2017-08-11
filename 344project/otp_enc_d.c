/*
Author: James Palmer
File Name: otp_enc_d.c
Project 4 final
File is almost the same as otp_enc.c
*/


#include <fcntl.h>     
#include <netinet/in.h>
#include <stdio.h>     
#include <stdlib.h>    
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define SIZE    128000

int main(int argc, char *argv[])
{
    
    
    socklen_t clilen; 
    struct sockaddr_in serv_addr, cli_addr;
    char buffer[SIZE];
    char keyBuffer[SIZE];
    char secondaryBuffer[SIZE];
	int fileName, fileLocation, newFile, n, i;
    int fileSize, givenFileSize, pid, sentInfo;

    //First check number of arguments from given file
    if (argc < 2)
    {
        printf("File open: otp_enc_d port\n");
        exit(1);
    }
    fileLocation = atoi(argv[1]);

    //this file needs a socket to read
    if ((fileName = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("File: opt_enc_d ERROR! couldn't create socket \n");
        exit(1);
    }

    //free address space for memory
    memset(&serv_addr, '\0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(fileLocation);

    //bind socket to prevent errors
    if (bind(fileName, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
		//error output if cannot bind
        printf("File: otp_enc_d ERROR! port: %d cannot be bounded\n", fileLocation);
        exit(2);
    }

    //like before, call listen for connection
    if (listen(fileName, 5) == -1)
    {
		//error output if cannot call listen
        printf("File: otp_enc_d ERROR! port: %d cannot listen call \n", fileLocation);
        exit(2);
    }
    clilen = sizeof(cli_addr);

    //call all connections
	//infiite loop until no more connections
    while (1)
    {
		//accepts new fileName given then continues
        newFile = accept(fileName, (struct sockaddr *) &cli_addr, &clilen);
        if (newFile < 0)
        {
			//error output
            printf("File: otp_enc_d ERROR! undesireable connection\n");
            continue;
        }

        //start a fork to connect files
        pid = fork();

        if (pid < 0)
        {
            printf("File: otp_enc_d ERROR! connection with fork failed \n");
        }

        //start the child Process
        if (pid == 0)
        {

            //clear buffer to prevent overflow
            memset(buffer, 0, SIZE);
		
            //get size and length of file
            givenFileSize  = read(newFile, buffer, SIZE);
            if (givenFileSize  < 0)
            {
				//error output
                printf("File: otp_enc_d ERROR! port: %d missing plaintext \n", fileLocation);
                exit(2);
            }

            //confirm with client on file
            sentInfo = write(newFile, "!", 1);
            if (sentInfo < 0)
            {
				//if cannot send to client, error
                printf("File: otp_enc_d ERROR! client did not respond\n");
                exit(2);
            }


            //clear our buffer again
            memset(keyBuffer, 0, SIZE);

            //acquire name file and contents, including size
            fileSize = read(newFile, keyBuffer, SIZE);
            if (fileSize < 0)
            {
				//error reading file
                printf("File: otp_enc_d ERROR! port: %d failed to read file \n", fileLocation);
                exit(2);
            }

            //error check for broken characters from file 
            for (i = 0; i < givenFileSize ; i++)
            {
                if ((int) buffer[i] > 90 || ((int) buffer[i] < 65 && (int) buffer[i] != 32))
                {
					//error
                    printf("File: otp_enc_d ERROR! text error form plaintext \n");
                    exit(1);
                }
            }

            //check if key exists, if not, error message again
            for (i = 0; i < fileSize; i++)
            {
                if ((int) keyBuffer[i] > 90 || ((int) keyBuffer[i] < 65 && (int) keyBuffer[i] != 32))
                {
                    printf("File: otp_enc_d ERROR! key is bugged with bad text \n");
                    exit(1);
                }
            }
            
            //check if key = file in length, if not error
            if (fileSize < givenFileSize )
            { 
                printf("File: otp_enc_d ERROR! file length is longer than our key\n");
                exit(1);
            }

            //fill in decryption section below
            for (i = 0; i < givenFileSize ; i++)
            {
                //per homework, instead of spaces give @
                if (buffer[i] == ' ')
                {
                    buffer[i] = '@';
                }
                if (keyBuffer[i] == ' ')
                {
                    keyBuffer[i] = '@';
                }

                //typecast here = ASCII maniuplation 
                int inputChar = (int) buffer[i];
                int keyChar = (int) keyBuffer[i];

                //minus the file for ASCII maniuplation -64
                inputChar = inputChar - 64;
                keyChar = keyChar - 64;

                //per homework: check if sum is 27 or A-Z
                int cipherText = (inputChar + keyChar) % 27;

                //per homework all characters must be IN CAPITAL LETTERS
                cipherText = cipherText + 64;

                //transition back to regular characters
                secondaryBuffer[i] = (char) cipherText + 0;

                //was easier to make spaces asteristics
				//now change asteristics to spaces
                if (secondaryBuffer[i] == '@')
                {
                    secondaryBuffer[i] = ' ';
                }
            }

            //what is the point of our file if we cannot decrypt our text
			//output decryption here, if cannot, error output
            sentInfo = write(newFile, secondaryBuffer, givenFileSize );
            if (sentInfo < givenFileSize )
            {
                printf("File: otp_enc_d ERROR! cannot output decryption\n");
                exit(2);
            }

            //close the files and sockets to prevent overflow
            close(newFile);
            close(fileName);

            exit(0);
        }
		//final newfile close
        else close(newFile);
    } 

    return 0;
}