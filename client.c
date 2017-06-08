#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>	
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

typedef struct REGISTRATION{
	unsigned char ID;
	char SERVER_NAME[30];
	char CLIENT_NAME[30];
} REGISTRATION;

typedef struct BROADCAST_A_MESSAGE{
	unsigned char ID;
	char SENDER[30];
	char SUBJECT[80];
	char MESSAGE_BODY[512];
} BROADCAST_A_MESSAGE;

typedef struct DISCONNECT{
	unsigned char ID;
} DISCONNECT;

int SERVER_PORT_NUMBER;
char CLIENT[30];
char SERVER_NAME[30];
char CLIENT_NAME[30];
char CLIENT_MAILID[30];
int SOCKET;
int PID;
struct hostent *SERVER;
struct sockaddr_in SERVER_ADDRESS;
int SERVER_PORT;
char BUFFER[1024];

void CLIENT_RUNNER()
{
	int MENU_INPUT;
	while(1==1)
	{
		printf("\n------MESSAGE BROADCAST SERVER SYSTEM------");
		printf("\n Press 1: REGISTER CLIENT");
		printf("\n Press 2: DEREGISTER CLIENT");
		printf("\n Press 3: BROADCASTE A MESSAGE");
		printf("\n Press 4: QUIT PROGRAM");
		printf("\n-------------------------------------\n");
		printf("\n\nINPUT = ");
		scanf("%d", &MENU_INPUT);

		switch(MENU_INPUT)
		{
/*------------------ CASE 1 ------------------*/
			case 1:
			printf("\n------REGISTER CLIENT------");
			printf("\nCLIENT NAME: %s\n", CLIENT);
			
			struct REGISTRATION REGISTRATION_REQUEST;
			bzero(&REGISTRATION_REQUEST, sizeof(REGISTRATION));
			REGISTRATION_REQUEST.ID = 1; 
			strcpy(REGISTRATION_REQUEST.CLIENT_NAME, CLIENT);
			getchar();

			int W_COUNTER = write(SOCKET, &REGISTRATION_REQUEST, sizeof(REGISTRATION_REQUEST));
			if(W_COUNTER < 0)
			{
				perror("\nERROR: Cannot write into socket\n");
				return;
			}
			printf("\n------REGISTRATION SUCCESS------\n");
			printf("\n\nINPUT = ");
			scanf("%d", &MENU_INPUT);
			break;

/*------------------ CASE 2 ------------------*/
			case 2:
			printf("\n------DEREGISTER CLIENT------");
			printf("\nCLIENT NAME: %s\n", CLIENT);

			struct REGISTRATION DEREGISTRATION_REQUEST;
			bzero(&DEREGISTRATION_REQUEST, sizeof(REGISTRATION));
			DEREGISTRATION_REQUEST.ID = 2;
			strcpy(DEREGISTRATION_REQUEST.CLIENT_NAME, CLIENT);
			getchar();

			W_COUNTER = write(SOCKET, &DEREGISTRATION_REQUEST, sizeof(DEREGISTRATION_REQUEST));
			if(W_COUNTER < 0)
			{
				perror("\nERROR: Cannot write into socket\n");
				return;
			}			
			printf("\n------DEREGISTRATION SUCCESS------\n");
			printf("\n\nINPUT = ");
			scanf("%d", &MENU_INPUT);
			break;			

/*------------------ CASE 3 ------------------*/
			case 3:
			printf("------COMPOSE EMAIL------");
			struct BROADCAST_A_MESSAGE CLIENT_EMAIL;
			bzero(&CLIENT_EMAIL, sizeof(BROADCAST_A_MESSAGE));
			CLIENT_EMAIL.ID = 3;
	
			printf("\nEnter SENDER_EMAIL_ID: ");
			getchar();
			fgets(CLIENT_EMAIL.SENDER,13,stdin);

			printf("\nEnter SUBJECT: ");
			fgets(CLIENT_EMAIL.SENDER,80,stdin);

			printf("\nEnter MESSAGE_BODY: ");
			fgets(CLIENT_EMAIL.MESSAGE_BODY,512,stdin);

			printf("------SENDING EMAIL TO SERVER------");
			int write_count = write(SOCKET, &CLIENT_EMAIL, sizeof(CLIENT_EMAIL));
			if (write_count < 0)
			{
				perror("ERROR: Cannot write into socket");
				return;
			}
			printf("\n\nINPUT = ");
			scanf("%d", &MENU_INPUT);
			break;

/*------------------ CASE 4 ------------------*/
			case 4:
			// CLIENT termination process begins
			printf("------TERMINATE CLIENT------");
			struct DISCONNECT REQUEST;
			bzero(&REQUEST, sizeof(DISCONNECT));
			REQUEST.ID = 4;
			int W_COUNT = write(SOCKET, &REQUEST, sizeof(REQUEST));
			if(W_COUNT < 0)
			{
				printf("\nERROR: cannot write into socket\n");
			}
			close(SOCKET);
			printf("\n--- Connection terminated ---\n");
			break;

/*------------------ DEFAULT ------------------*/
			default:
			printf("\nERROR: Invalid menu input\n");
			scanf("input = %d", &MENU_INPUT);
			break;
		}
		/* Maximum length for hostname set to 1024 */
		bzero(BUFFER,1024);
		int LENGTH_OF_DATA = read(SOCKET,BUFFER,1024);
		printf("\nReply from SERVER: %s\n", BUFFER);	
	
	}
}

int main(int argc, char *argv[])
{
	gethostname(CLIENT,30);
	//printf("%s\n",CLIENT);	
	if(argc == 3)
	{
		strcpy(SERVER_NAME, argv[1]);
		SERVER_PORT_NUMBER = atoi(argv[2]);
		gethostname(CLIENT,31);
	
		//printf("SERVER_NAME: %s\n", SERVER_NAME);
		//printf("PORT: %d\n", SERVER_PORT_NUMBER);
		//printf("CLIENT: %s\n", CLIENT);
		printf("Please wait...\n");

		/* SOCKET creation */
		SOCKET = socket(AF_INET,SOCK_STREAM,0);	
		if(SOCKET < 0)
		{
			perror("\nERROR: Cannot open socket\n");
		}

		bzero((char *) &SERVER_ADDRESS, sizeof(SERVER_ADDRESS));
		SERVER_ADDRESS.sin_family = AF_INET;
		//bcopy((char *) SERVER->h_addr, (char *) &SERVER_ADDRESS.sin_addr.s_addr,SERVER->h_length);
		SERVER_ADDRESS.sin_port = htons(SERVER_PORT);/*
		if(connect(SOCKET, (struct sockaddr *) &SERVER_ADDRESS, sizeof(SERVER_ADDRESS)) < 0)
		{
			perror("\nERROR: cannot connect to server\n");
			exit(0);
		}*/
		fprintf(stdout,"\nSUCCESS: Connected to server\n");
		CLIENT_RUNNER();
	}
	else
	{
		printf("\nERROR: Invalid argument");
		printf("\nUSAGE: ./(filename.out) (server_name) (server_port_no) (client_name)\n");
		exit(0);
	}
}
