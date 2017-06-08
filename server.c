#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>	
#include <string.h>	
#include <errno.h>	
#include <arpa/inet.h>

#define	PORT	8080

typedef struct REGISTRATION{
	unsigned char ID;
	char SERVER_NAME[30];
	char CLIENT_NAME[30];
	char NAME[30];
} REGISTRATION;

typedef struct BROADCAST_A_MESSAGE{
	unsigned char ID;
	char CLIENT_MAIL[30];
	char ACLIENT_NAME[30];
	char SUBJECT[80];
	char BODY[512];
} BROADCAST_A_MESSAGE;

typedef struct DISCONNECT{
	unsigned char ID;
} DISCONNECT;

/* FUNCTION declarations*/
void CONNECT_CLIENT(int SERVER_SOCKET,char *BUFFER);
void SERVER_RUNNER(int PORT_NUMBER);

int main(int argc, char *argv[])
{
	if(argc == 2)
	{
		int PORT_NUMBER = atoi(argv[1]);
		SERVER_RUNNER(PORT_NUMBER);
	}
	else
	{
		printf("\nERROR: Invalid argument");
		printf("\nUsage: ./(filename.out) (port_number)");
	}
	return 0;
}

/*	Connect with client will perform,
	(1) Registration request
	(2) Deregistration request
	(3) Broadcasting request
	(4) Shutting down server

*/
void CONNECT_CLIENT(int SERVER_SOCKET,char *BUFFER)
{
	unsigned char ID = BUFFER[0];
	switch(ID)
	{
		case 1:
		printf("SERVER: ------REGISTRATION REQUEST------\n");
		char BUFFER2[30];
		strcpy(BUFFER2, "REGISTER");
		if(strcmp(BUFFER,BUFFER2) == 0)
		{
			struct REGISTRATION RE;
			memcpy(&RE, BUFFER, sizeof(RE));

			/* initiating REGISTRATION procedure */
			struct REGISTRATION REGISTER_CLIENT;
			strcpy(REGISTER_CLIENT.NAME, RE.NAME);
			char CLIENT[30];	/* for saving client names */
			int LENGTH;
			strcpy(CLIENT, REGISTER_CLIENT.NAME);
			if(CLIENT[LENGTH = strlen(CLIENT)-1])
			{
				CLIENT[LENGTH] = '\0';
			}
			
			FILE *FILE_A, *FILE_B;
			FILE_B = fopen("clients.txt","a");
			{
				fprintf(FILE_B,"%s\n",CLIENT);
				write(SERVER_SOCKET,"SUCCESS: Client registered successfully...",310);
				fclose(FILE_B);
			}
			
		}
		
		case 2:
		printf("SERVER: ------DEREGISTRATION REQUEST------\n");
		char BUFFER3[30];
		strcpy(BUFFER3,"DEREGISTER");
		if(strcmp(BUFFER,BUFFER3) == 0)
		{
			struct REGISTRATION DERE;
			memcpy(&DERE, BUFFER, sizeof(DERE));

			/* initiating DEREGISTRATION procedure */
			struct REGISTRATION DEREGISTER_CLIENT;
			strcpy(DEREGISTER_CLIENT.NAME, DERE.NAME);
			char CLIENT[30];
			int LENGTH;
			strcpy(CLIENT, DEREGISTER_CLIENT.NAME);
			if(CLIENT[LENGTH = strlen(CLIENT)-1])
			{
				CLIENT[LENGTH] = '\0';
			}
			
			FILE *FILE_A, *FILE_B;
			FILE_A = fopen("clients.txt","r");
			{
				FILE_A = fopen("clients.txt","r");
				FILE_B = fopen("temp_clients.txt","a");
				while(feof(FILE_A))
				{
					char CLIENT2[30];
					int LENGTH2;

					/* Fetch CLIENT details*/
					fgets(CLIENT2, 30, FILE_A);
					if(CLIENT2[LENGTH2 = strlen(CLIENT2)-1] == '\n')
					{
						CLIENT2[LENGTH2] = '\0';
					}

					/* Compare all CLIENT_NAMES with current CLIENT_NAMES */
					if(strcmp(CLIENT2, CLIENT) != 0)
					{
						/* DUPLICATE CLIENT_NAMES will not be stored into temperory file */
						fprintf(FILE_B,"%s\n",CLIENT2);
					}
				}
				fclose(FILE_A);
				fclose(FILE_B);
				remove("clients.txt");
				rename("temp_clients.txt","clients.txt");
				write(SERVER_SOCKET, "SUCCESS: Client deregistered successfully...",310);
			}
		}

		case 3:
		printf("SERVER: ------BROADCASTING REQUEST------\n");
		char BUFFER4[30];
		strcpy(BUFFER4,"BROADCAST");
		if(strcmp(BUFFER, BUFFER4) == 0)
		{
			struct BROADCAST_A_MESSAGE BAM;
			memcpy(&BAM, BUFFER, sizeof(BAM));

			/* initiating BROADCASTING procedure */
			struct BROADCAST_A_MESSAGE BROADCAST;
			strcpy(BROADCAST.ACLIENT_NAME, BAM.CLIENT_MAIL);
			printf("SENDER: %s\n", BROADCAST.CLIENT_MAIL);
			
			strcpy(BROADCAST.CLIENT_MAIL, BAM.ACLIENT_NAME);
			strcpy(BROADCAST.SUBJECT, BAM.SUBJECT);
			printf("SUBJECT: %s\n", BROADCAST.SUBJECT);

			strcpy(BROADCAST.BODY, BAM.BODY);
			printf("BODY: %s\n", BROADCAST.BODY);

			char CLIENT[30];
			strcpy(CLIENT, BROADCAST.ACLIENT_NAME);
			
			FILE *FILE_A, *FILE_B;
			FILE_A = fopen("clients.txt","r");
			{
				while(!feof(FILE_A))
				{
					char CLIENT2[30];
					int LENGTH;

					/* Fetch CLIENT details */
					fgets(CLIENT2, 30, FILE_A);
					if(CLIENT2[LENGTH = strlen(CLIENT2)-1] == '\n')
					{
						CLIENT2[LENGTH] = '\0';
					}
					write(SERVER_SOCKET, "SUCCESS: Broadcast message sent successfully...", 310);					
				}
			}
			fclose(FILE_A);
		}

		case 4:
		printf("SERVER: ------SHUTTING DOWN SERVER------");
		char CLIENT[30];
		strcpy(CLIENT, "SHUTDOWN");
		
		if(strcmp(BUFFER, CLIENT) == 0)
		{
			write(SERVER_SOCKET, "SUCCESS: Server closed...",130);
			close(SERVER_SOCKET);
			exit(0);
		}

		default:
		printf("ERROR: Invalid request recived form CLIENT: %s", BUFFER);
		fflush(stdout);
	}
}

/*	This function handles multiple socket connections
	where, SELECT function is used here, which will allow them to do so
	select will provide the information when data to be read on one of the socket
*/
void SERVER_RUNNER(int PORT_NUMBER)
{

	int OPTION = 1;	/* use to set the socket option - defined in macros */ 
	int MASTER_SOCKET; /* for handeling muplitple connections */ 
	int ADDRESS_LENGTH; /* for address length argument */ 
	int NEW_SOCKET; /* a new socket */
	int CLIENT_SOCKET[31];
	int MAXIMUM_CLIENTS = 31; /* number of maximum clients to handle connections */
	int READY; /* identify the incomming request */
	int INDEX;
	int VALUE_READ;
	int SOCKET_DESCRIPTOR;
	int MAX_SOC_DESCRIPTOR;
	struct sockaddr_in ADDRESS;
	char DATA_BUFFER[1024];
      
	/* Setting the bit for file descriptor fd in the set READ_FDS */
	fd_set READ_FDS;
      
	/* Sending message */
	char *message = "SERVER: Connected...\r\n";

	/* Initialization of all the CLIENT_SOCKET[] to 0 */
	for(INDEX = 0 ; INDEX < MAXIMUM_CLIENTS ; INDEX++)
        { CLIENT_SOCKET[INDEX] = 0; }
      
	if( (MASTER_SOCKET = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{
		perror("ERROR: Socket failed\n");
		exit(EXIT_FAILURE);
	}
  
/* 	setsockopt = set the socket option
	----------------------------------
	socket = MASTER_SOCKET
	level = SOL_SOCKET = socket layer itself
	option_name = SO_REUSEADDR = allows our sever to bind to an addres TIME_WAIT state
	option_value = OPTION
	option_length = sizeof(OPTION)
*/

	if( setsockopt(MASTER_SOCKET, SOL_SOCKET, SO_REUSEADDR, (char *)&OPTION, sizeof(OPTION)) < 0 )
	{
		perror("setsockopt\n");
		exit(EXIT_FAILURE);
	}
  
	/* SOCKET created */
	ADDRESS.sin_family = AF_INET;
	ADDRESS.sin_addr.s_addr = INADDR_ANY;
	ADDRESS.sin_port = htons( PORT_NUMBER );
      
	/* Bind the socket to localhost port 8080 */
	if (bind(MASTER_SOCKET, (struct sockaddr *)&ADDRESS, sizeof(ADDRESS))<0) 
	{
		perror("ERROR: Bind failed\n");
		exit(EXIT_FAILURE);
	}
	printf("\nListener on port %d \n", PORT_NUMBER);

	/* For MASTER_SOCKET, maximum pending connections = 3 */
	if (listen(MASTER_SOCKET, 3) < 0)
	{
		perror("LISTEN");
		exit(EXIT_FAILURE);
	}
      
	/* accept the incoming connection */
	ADDRESS_LENGTH = sizeof(ADDRESS);
	puts("Waiting for incomming connections...");
     
	while(1==1) 
	{
		/* clear the socket set */
		FD_ZERO(&READ_FDS);

/* READ_FDS will be watched to see if character become available for reading or not*/
  
	/* Add MASTER_SOCKET to set */
	FD_SET(MASTER_SOCKET, &READ_FDS);
	MAX_SOC_DESCRIPTOR = MASTER_SOCKET;
         
        /* Add child sockets to set */
        for ( INDEX = 0 ; INDEX < MAXIMUM_CLIENTS ; INDEX++) 
        {
		/* SOCKET_DESCRIPTOR */
		SOCKET_DESCRIPTOR = CLIENT_SOCKET[INDEX];
             
		/* Add to read list if it is a valid SOCKET_DESCRIPTOR */
		if(SOCKET_DESCRIPTOR > 0)
			FD_SET( SOCKET_DESCRIPTOR , &READ_FDS);
             
		/* highest file descriptor number, need it for the select function */
		if(SOCKET_DESCRIPTOR > MAX_SOC_DESCRIPTOR)
			MAX_SOC_DESCRIPTOR = SOCKET_DESCRIPTOR;
	}
  
	/* Wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely */
	READY = select( MAX_SOC_DESCRIPTOR + 1 , &READ_FDS , NULL , NULL , NULL);
    

	/* EINTR = call unable to succeed due to connection interruption */
	if ((READY < 0) && (errno!=EINTR)) 
	{ printf("ERROR: select error\n"); }
          
	/* Activity on MASTER_SOCKET = incoming connection */
	if (FD_ISSET(MASTER_SOCKET, &READ_FDS)) 
	{
		if ((NEW_SOCKET = accept(MASTER_SOCKET, (struct sockaddr *)&ADDRESS, (socklen_t*)&ADDRESS_LENGTH))<0)
		{
			perror("ACCEPT\n");
			exit(EXIT_FAILURE);
		}
          
		/* inform user of socket number - used in send and receive commands */
		printf("\n--- NEW CONNECTION ---\nSocket file descriptor : %d\nIP : %s\nPort : %d \n" , NEW_SOCKET , inet_ntoa(ADDRESS.sin_addr) , ntohs(ADDRESS.sin_port));
        
		/* send new connection greeting message */
		if( send(NEW_SOCKET, message, strlen(message), 0) != strlen(message) ) 
		{
			perror("SEND");
		}
              
		puts("MESSAGE: Welcome message sent successfully");
              
		/* Add new socket to CLIENT_SOCKET array */
		for (INDEX = 0; INDEX < MAXIMUM_CLIENTS; INDEX++) 
		{
			/* If nothing in CLIENT_SOCKET array */
			if( CLIENT_SOCKET[INDEX] == 0 )
			{
				CLIENT_SOCKET[INDEX] = NEW_SOCKET;
				printf("Adding to socket list as %d\n" , INDEX);
				break;
			}
		}
	}
          
	/* Else its some IO operation on some other socket */
	for (INDEX = 0; INDEX < MAXIMUM_CLIENTS; INDEX++) 
	{
		SOCKET_DESCRIPTOR = CLIENT_SOCKET[INDEX];
              	
		if (FD_ISSET( SOCKET_DESCRIPTOR , &READ_FDS)) 
		{
			/* Check if it was for closing , and also read the incoming message */
			if ((VALUE_READ = read( SOCKET_DESCRIPTOR , DATA_BUFFER, 1024)) == 0)
			{
				/* A perticular CLIENT DISCONNECTED,
				get that CLIENT information
				then PRINT it*/
				getpeername(SOCKET_DESCRIPTOR , (struct sockaddr*)&ADDRESS , (socklen_t*)&ADDRESS_LENGTH);
				printf("--- HOST DISCONNECTED ---\nIP : %s\nPort : %d\n" , inet_ntoa(ADDRESS.sin_addr) , ntohs(ADDRESS.sin_port));
                     
				/* Close the socket and mark as 0 in list for reuse */
				close( SOCKET_DESCRIPTOR );
				CLIENT_SOCKET[INDEX] = 0;
			}
			/* ECHO back the message */
                	else
			{
				/* set the string terminating NULL byte on the end of the data read */
				DATA_BUFFER[VALUE_READ] = '\0';
				CONNECT_CLIENT(SOCKET_DESCRIPTOR,DATA_BUFFER);
			}
		}
	}
   }
}

