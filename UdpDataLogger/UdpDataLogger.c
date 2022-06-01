#include "UdpDataLogger.h"



int main(void)
{
	struct sockaddr_in si_me, si_other;
	
	int s, recv_len;
	unsigned slen = sizeof(si_other) ;

	char bufRcvUDP[BUFLEN];
	char bufDate[BUFLEN];
	char bufferConfiguration[MAXLINESIZE];
	
	getConfigurationFromFile("./UdpDataLogger.conf", "PORT", bufferConfiguration);
	printf("PORT[%s]\n", bufferConfiguration);
	
	//create a UDP socket
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		die("socket");
	}
	
	// zero out the structure
	memset((char *) &si_me, 0, sizeof(si_me));	
	memset(&bufRcvUDP, 0, BUFLEN);

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	
	//bind socket to port
	if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
	{
		die("bind");
	}


	FILE* temperatureLog = fopen(FILEDEST, "a");

	time_t t;

	//keep listening for data
	while(1)
	{
		
		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, bufRcvUDP, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
		{
			die("recvfrom()");
		}
		
		//print details of the client/peer and the data received
		//		printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		t = time(NULL);
		strftime(bufDate, 1000, "%Y-%m-%dT%H:%M:%S", localtime(&t));
		fprintf(temperatureLog, "%s, %s\n",bufDate,bufRcvUDP);
		fflush(temperatureLog);
		
	}

	close(s);
	return 0;
}


int getConfigurationFromFile(char* configurationFile, char* targetVariable, char* returnValue){
  FILE *in_file = fopen(configurationFile, "r");
  char *currentLine = malloc(MAXLINESIZE);
  int targetVariableStringLength = strlen(targetVariable);

  while (fscanf(in_file, "%[^\n] ", currentLine) != EOF) {
    if (strncmp(targetVariable, currentLine, targetVariableStringLength)==0
	    && currentLine[targetVariableStringLength]=='='
	){
      currentLine = currentLine + targetVariableStringLength + 1;
      strcpy(returnValue, currentLine);
      fclose(in_file);
      return 0;
    }
  }
  fclose(in_file);
  return -1;
}

void die(char *s)
{
	perror(s);
	exit(1);
}
