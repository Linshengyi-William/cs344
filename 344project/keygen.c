/*
Author: James Palmer
File Name: keygen.c
Project 4 final

*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//generate random cipher key with global function
int randomNum(int min, int max);



//generating the random cipher key function here
int randomNum(int min, int max)
{   
    return rand() % (max - min + 1) + min;
}


int main(int argc, char *argv[]){

    char character;
    int i;
    int fileLength;
    time_t clock;

    // Check number of arguments 
    if (argc < 2)
    {
        printf("Length of keygen file\n");
        exit(1);
    }

    //specified size for the key file
    sscanf(argv[1], "%d", &fileLength);

    if (fileLength < 1)
    {
        printf("keygen file not found: ERROR\n");
        exit(1);
    }

    // Seed random number generator
    srand((unsigned) time(&clock));

    //acquire file numbers
    for (i = 0; i < fileLength; i++)
    {
        //call random function
        character = (char) randomNum(64, 90);

        //error case here that needed to be added
        if (character == '@')
        {
           character = ' ';
        }
		//output the character
        printf("%c", character);
    }

    printf("\n");

    return 0;
}
