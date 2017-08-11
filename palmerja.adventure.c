#include "palmerja.buildrooms.c"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>



pthread_mutex_t mutex;

//create a currentTime.txt file as outline in assignment
void* writeTimeFile()
{
	//Writes the current time to a file called time.txt
	time_t currTime;
	currTime = time(NULL);
	FILE* timeFile;
	timeFile = fopen("currentTime", "w+");
	fprintf(timeFile, "%s", ctime(&currTime));
	fclose(timeFile);
	return timeFile;
}

//then pull the current time from the currentTime.txt file
void printTimeFile()
{
	//Create local varaibles.
	FILE* fp;
	char buffer[255];
	memset(buffer, '\0', sizeof(buffer));
	int c;
	int pos = 0;

	fp = fopen("currentTime.txt", "r"); //open file
	
	do
	{
		c = fgetc(fp); //get a character from the file
		
		if (c != EOF) //if not end of file
		{
			buffer[pos] = (char)c;
			pos++;
		}
	} while (c != EOF && c != '\n'); //reads one line
	printf("\n\n");
	printf("%s", buffer);

	fclose(fp); //closes file pointer.
}

void threadTwo()
{
        pthread_t thread2;
        pthread_mutex_init(&mutex, NULL);
        pthread_mutex_lock(&mutex);
        int threadID = pthread_create(&thread2, NULL, writeTimeFile, NULL);
        pthread_mutex_unlock(&mutex);
        pthread_mutex_destroy(&mutex);
        sleep(1);
}

//start the game
void  gameStart(FILE** filePointers, struct room* currentRoom)
{
	char** pathToVictory = malloc(sizeof(char) * 10000);
	memset(pathToVictory, '\0', sizeof(*pathToVictory)); 
	int stepCounter = 0;
	int i;
	char userResponse[30];
	while(true)
	{
		//Room formatting
		printf("\n");
		if(checkRoomType(currentRoom))
		{
			printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
			printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", stepCounter);
			for(i = 0; i < stepCounter; i++)
				printf("%s\n", pathToVictory[i]);
			break;
		}
		
		//interface formatting.
		printf("CURRENT LOCATION: %s\n", currentRoom->roomName);
		printf("POSSIBLE CONNECTIONS: ");
		
		//Iterations for connecting rooms
		for(i = 0; i < currentRoom->numConnections; i++)
		{
			printf("%s", currentRoom->roomConnections[i]->roomName);
			if(i+1 ==  currentRoom->numConnections)
				printf(".\n");
			else
				printf(", ");
		}
		
		//Room formatting
		printf("WHERE TO? >");
		memset(userResponse, '\0', sizeof(userResponse));
		scanf("%s", userResponse);
		if(strcmp(userResponse,"time") == 0)
		{
			threadTwo();
			printTimeFile();
		}
		else
		{
			for(i = 0; i < currentRoom->numConnections; i++)
			{
				if(strcmp(userResponse, currentRoom->roomConnections[i]->roomName) == 0)
				{
					pathToVictory[stepCounter++] = currentRoom->roomName;
					currentRoom = currentRoom->roomConnections[i];
					break;
				}
				//error statement
				if(i+1 == currentRoom->numConnections)
				{
					printf("\n");
					printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
				}
			}
		}
	}		
}

int main(int argc, char **argv)
{
	//Runs all of the functions in the appropriate order and starts up the game! Have fun :)
	int i;
	char* directoryName = spawnDirectory();
	char** roomNames = generateRoomNames();
	FILE** filePointers = createRoomFiles(directoryName);
	int* randomIndices = chooseNameIndices();
	struct room** gameRooms = setupRooms(roomNames, randomIndices); 
	connectRooms(gameRooms, randomIndices);
	setRoomFilePointers(filePointers, gameRooms);
	prepareRooms(gameRooms);
	
	for(i = 0; i < 7; i++)
		fclose(filePointers[i]);
	//readRoomFiles(directoryName, gameRooms);
	
	for(i = 0; i < 7; i++)
		gameRooms[i]->fileHandle = NULL;
	struct room* startRoom = gameRooms[0];
			
	gameStart(filePointers, startRoom);

	return 0;
}
