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


#define BUFLEN 512	//Max length of buffer
#define MAXLINESIZE 4096

unsigned int PORT = 3000;	//The port on which to listen for incoming data
char FILEDEST[MAXLINESIZE] = "../data/data.csv";

int getConfigurationValueFromFile(char* configurationFile, char* targetVariable, char* returnValue);

int  getConfigurationFromFile(char* configurationFile);

void die(char *s);
