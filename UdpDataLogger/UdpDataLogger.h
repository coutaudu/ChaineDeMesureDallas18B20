/*  
 * COUTAUD Ulysse
 * L3P AII Univ. Claude Bernard Lyon 1 
 *
 * Fork basé sur l'exemple :
 * https://www.geeksforgeeks.org/udp-server-client-implementation-c/
 */


#include <stdio.h>	//printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h> // close function
#include <time.h>


#define BUFLEN 255	//Max length of buffer
#define PORT 30000	//The port on which to listen for incoming data
#define MAXLINESIZE 4096

char FILEDEST[] = "../data/data.csv";

int getConfigurationFromFile(char* configurationFile, char* targetVariable, char* returnValue);

void die(char *s);
