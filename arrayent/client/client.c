/**
 * @File   client.c
 * @Brief  Test application for Arrayent server interface.
 *
 * Copyright (c) 2012 Arrayent Inc.  Company confidential.
 * Please contact sales@arrayent.com to get permission to use in your
 * application.
 */

/* $Author: Madhusudhan.G $
   $Revision: #1.0 $
   $DateTime: 2014/06/04 16:10:21 $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<pthread.h> //for threading , link with lpthread
 
#include "aic.h"
#pragma pack(1)

//Funtion Prototypes
void *InitializeTCPSever(void *);
//void InitializeTCPSever(void *);
void *connection_handler(void *);
//int InitializeTCPServer(int portNumber);
void HandleDataReceivedFromClient(char* data);
typedef struct REVELATION_PACKET_STRUCT
{
	signed short transactionId;
	unsigned char header_byte;
	signed short msg_length;
	unsigned char reveal_api;
	unsigned char reveal_opcode;
	unsigned char payload[];
}REVELATION_PACKET;

enum REVEAL_API
{
	ECM_INFO=0xE3,
	REVEAL_SUBSCRIPTION
};

enum REVEAL_OPCODE
{
	GET_DEVICE_INFO=0x01,
	GET_DEVICE_COMM_MODULEINFO,
	GET_BOARD_INFO
};

const char *brand = "01";
const char *category = "06";
const char *modelNumber = "WEL98HEBU0";
const char *serialNumber="21901031";
const char *macAddress="88:e7:12:00:27:7b";
char *said ,*password ,*aeskey ,*id ;
int listenPort;
int isConnected=0;
short SubcribelistVersion=0;

int client;

unsigned short ReverseBytes(ushort value)
{
    return (unsigned short)((value & 0xFFU) << 8 | (value & 0xFF00U) >> 8);
}

int sizeOfString(char* testString)
{
	return (sizeof(testString) / sizeof(char)) ;
}

char* binaryToHexString(unsigned char* inputData, char* outputData, int length)
{

	//printf("inside binaryToHexString");
	fflush(stdout);
	char hexChars[] = "0123456789ABCDEF";
	int index;
	int hexValues[length/2];
	
	for(index = 0; index <= length; index++) 
	{
		char char1 = hexChars[((inputData[index] & 0xf0) >> 4)];
		char char2 = hexChars[(inputData[index] & 0x0f)];
		outputData[index*2] = char1;
		outputData[index*2+1] = char2;
		outputData[index*2+2] = 0;
		//hexValues[index]= outputData[index*2]>>4 |outputData[index*2+1];

		//printf("%x ::",inputData[index]);


	}
	//printf("Out of conv");
	outputData[2*length] = 0;
	return outputData;
}

char* hexStringToBinary(char* inputData, char* outputData, int* length)
{
	int index;
	int size = 0;
	for(index = 0; 1; index += 2)
	{
		if(inputData[index] == 0)
		{
			break;
		}
		size++;
		char char1 = inputData[index];
		char char2 = inputData[index+1];
		if(char1 >= 48 && char1 <= 57) 
		{
			char1 -= 48;
		}
		else if(char1 >= 65 && char1 <= 90) 
		{
			char1 -= 55;
		}
		if(char2 >= 48 && char2 <= 57) 
		{
			char2 -= 48;
		}
		else if(char2 >= 65 && char2 <= 90)
		{
			char2 -= 55;
		}
		outputData[index/2] = char1 << 4 | char2;
	}
	*length = size - 1;
	return outputData;
}

char* asciiToHexString(char* in, char* out)
{
	char hexChars[] = "0123456789ABCDEF";
	int iii = 0;
	while(1) 
	{
		if(in[iii] == 0)
		{
			out[2*iii] = 0;
			break;
		}
		else
		{
			char letter = in[iii];
			out[2*iii] = hexChars[(char)((letter >> 4) & 0x0f)];
			out[2*iii + 1] = hexChars[(char)(letter & 0x0f)];
			iii++;
		}
	}
	return out;
}

char* AsciiToHexString(char* in)
{
	char *out = (char*) malloc((int)strlen(in)*2);	
	char hexChars[] = "0123456789ABCDEF";
	int iii = 0;
	while(1) 
	{
		if(in[iii] == 0)
		{
			out[2*iii] = 0;
			break;
		}
		else
		{
			char letter = in[iii];
			out[2*iii] = hexChars[(char)((letter >> 4) & 0x0f)];
			out[2*iii + 1] = hexChars[(char)(letter & 0x0f)];
			iii++;
		}
	}
	return out;
}

char* PublishDeviceInformation(unsigned short transId, char* said)
{
	char category[] = "06";
	
	char *uniqueID = AsciiToHexString(said);
	char *modelNumber = AsciiToHexString("WEL98HEBU0");
	char *serialNumber= AsciiToHexString("21901031");
	char *macAddress = AsciiToHexString("88:e7:12:00:27:7b");
	char gdiSecondPart[] = "E321";
	unsigned char hexValue[2];
	char *result;
	
	unsigned short size =(unsigned short)(strlen(gdiSecondPart)+strlen(brand)+strlen(category)+strlen(uniqueID)+strlen(modelNumber)+strlen(serialNumber)+strlen(macAddress));
	
	result= (char*) malloc(size+(int)sizeof(unsigned short)+6);
	sprintf(hexValue,"%2x",transId);
	strcpy(result,hexValue);
	strcat(result,"ED");
	size = (int) size/2;
	size+=3;
	sprintf(hexValue,"%04x",size);
	
	printf("HexValue %s \n",hexValue);

	//hexValue[0]=(unsigned char)((size&0xFF00)>>4);
	//hexValue[1]=(unsigned char)(size&0x00FF);

	strcat(result,hexValue);
	strcat(result,gdiSecondPart);
	strcat(result,brand);
	strcat(result,category);
	strcat(result,uniqueID);
	strcat(result,modelNumber);
	strcat(result,"00"); //append \0 to end of modelNumber
	strcat(result,serialNumber);
	strcat(result,"00"); //append \0 to end of serialNumber
	strcat(result,macAddress);
	strcat(result,"00");  //append \0 to end of message
		
	//printf("Data :%s",result);

	free(uniqueID);
	free(modelNumber);
	free(serialNumber);
	free(macAddress);
	return result;
}

char* GetCommunicationModuleInformation(unsigned short transId)
{
	char* boardPartNumber= AsciiToHexString("W00000001");
	char* boardRevisionNumber= AsciiToHexString("01");
	char* projectReleaseNumber= AsciiToHexString("W10605695");
	char *result;
	char gdiSecondPart[] = "E322";
	unsigned char hexValue[2];
	int size=strlen(boardPartNumber)+strlen(boardRevisionNumber)+strlen(projectReleaseNumber)+(strlen(gdiSecondPart));
	result= (char*) malloc(size+(sizeof(unsigned short)*2)+3);
	sprintf(hexValue,"%2x",transId);
	strcpy(result,hexValue);

	
	strcat(result,"ED");
	size = (int) size/2;
	//size+=3;
	sprintf(hexValue,"%04x",size);
	strcat(result,hexValue);	
	printf("HexValue %s \n",hexValue);
	strcat(result,gdiSecondPart);
	strcat(result,boardPartNumber);
	//strcat(result,"00");
	strcat(result,boardRevisionNumber);
	//strcat(result,"00");
	strcat(result,projectReleaseNumber);
	//strcat(result,"00");
	free(boardPartNumber);
	free(boardRevisionNumber);
	free(projectReleaseNumber);
	//printf("<%s> DeviceInformation ", result);
	return result;
}

char* GetSubscribelistVersion(unsigned short transId)
{
	char responseApi[]="E424";
	int size = (int) (sizeof(unsigned short)*3+strlen(responseApi)+1);

	char* result;

	char hexValue[2];

	result=(char*)malloc(size);
	sprintf(hexValue,"%2x",transId);
	strcpy(result,hexValue);
	strcat(result,"ED");
	size = (sizeof(responseApi)+sizeof(unsigned short))/2;
	sprintf(hexValue,"%04x",size);
	strcat(result,hexValue);
	strcat(result,responseApi);	
	sprintf(hexValue,"%04x",SubcribelistVersion);
	strcat(result,hexValue);
	return result;
}

char* SubscribelistAck(unsigned short transId)
{
	char responseApi[]="E425";
	int size = (int)(sizeof(unsigned short)+sizeof(char)+strlen(responseApi)+1);
	char* result;
	char hexValue[2];
	result=(char*)malloc(size);	
	sprintf(hexValue,"%2x",transId);
	strcpy(result,hexValue);
	strcat(result,"ED");
	size = (sizeof(responseApi)+sizeof(char))/2;
	sprintf(hexValue,"%04x",size);
	strcat(result,hexValue);
	strcat(result,responseApi);	
	sprintf(hexValue,"%x",0);
	strcat(result,hexValue);
	return result;
}

char* HandleDeviceInformation(unsigned short transId, unsigned char opcode,char* said)
{
	char *result;
	 
	switch(opcode)
	{
		case 0x01:
			result=PublishDeviceInformation(transId,said);
			
		break;
		case 0x02:
			result=GetCommunicationModuleInformation(transId);
		default : break;
	}
	
	//printf("deviceInfo <%s>\n",result);
	//printf("GetDeviceInformation\n");
	return result;	
}


char* HandleRevealSubscriptionCommands(unsigned short transId, unsigned char opcode)
{
	//char *result;
	switch(opcode)
	{
		case 0x01:
		break;
		case 0x02:
		break;
		case 0x03:
		break;
		case 0x04:
			return(GetSubscribelistVersion(transId));
			
		case 0x05:
			return(SubscribelistAck(transId));
		break;
	}
	return 0;
}

void SendData(unsigned char* data ,int messageLength)
{
	char message[1024];
	//int messageLength = sizeof(data) / sizeof(data[0]);
	//printf("deviceInfo <%s>\n",data);
	//hexStringToBinary(data,message,&messageLength);
		
	printf("Message Sent %d",messageLength);
	
	//int i;
	//for(i=0; i< messageLength;i++)
		//printf("%x",message[i]);

	if(ArrayentSendData(data, messageLength+1, 10000) < 0) 
	{
		printf("<%s> Could not send message\n",message);
	}
		
	//if(ArrayentSendData(message, messageLength+1, 10000) < 0) 
	//{
	//	printf("<%s> Could not send message\n",message);
	//}
}

//Take a packet (ASCII Hex), look at bits, decide if there needs to be a response
//and then send the appropriate response
void sniff(unsigned char* packet, char* said) 
{
	char truePacket[1024];
	//char buff[1024];
	char tmp[1024];
	
	
	REVELATION_PACKET *revealPacket;
	revealPacket = (REVELATION_PACKET *) packet;

	revealPacket->transactionId=ReverseBytes(revealPacket->transactionId);
	
	int len;
	char* deviceInfo ;

	printf("transactionID : %x",revealPacket->transactionId);
	printf("headerbyte {%x}\n",revealPacket->header_byte);
	printf("msglenth {%x}\n",revealPacket->msg_length);
	printf("api {%x}\n",revealPacket->reveal_api);
	printf("opcode {%x}\n",revealPacket->reveal_opcode);
	switch(revealPacket->reveal_api)
	{
		case ECM_INFO:	//SendData(HandleDeviceInformation(revealPacket->transactionId,revealPacket->reveal_opcode,said));
				break;
		case REVEAL_SUBSCRIPTION: //SendData(HandleRevealSubscriptionCommands(revealPacket->transactionId,revealPacket->reveal_opcode));
				break;
		default:break;
	}
  
}


static int token;
static sem_t callbackSem;

int InitializeArrayentWithSaid(char* said)
{
	printf("<%s> Starting connection to Arrayent daemon...\n",said);
	unsigned tries = 5;
	while(ArrayentInitWithId(said) < 0 && tries>0) 
	{
		printf("<%s> Connection to Arrayent daemon failed.  Retrying Test...\n",said);
		fflush(stdout);
		usleep(100000);
		tries--;
	}
	if(tries <= 0)
	{
		printf("<%s> Unable to establish connection, closing daemon.\n",said);
		fflush(stdout);
		return -1;
	}

	return 1;
}

void ConfigureConnection(char* aeskey,char* said , char* password, char* id )
{
	char buf[4096];
	memcpy(buf, aeskey, 32);
	if(ArrayentSetProperty("sys-aeskey", buf) < 0)
	{
		printf("<%s> Could not connect to server.\n",said);
		fflush(stdout);
		exit(1);
	}
	sprintf(buf, "%d", atoi(id));
	//sprintf(buf, "%d", 11);
	//printf("------------------->   |%s|\n",buf);
	if(ArrayentSetProperty("sys-productid", buf) < 0) 
	{
		printf("<%s> Could not connect to server.\n",said);
		fflush(stdout);
		exit(1);
	}
	sprintf(buf, "%s", said);
	sprintf(&buf[48], "%s", password);
	if(ArrayentSetCredentials(buf, &buf[48]) < 0)
	{
		printf("<%s> could not connect to server.\n",said);
		fflush(stdout);
		exit(1);
	}
}

void SetArrayentDNS()
{
	char buf[4096]; 	
	//////////////////////////////////////////////
 	sprintf(buf,"%s","d1.applianceconnect.net"); 
	if(ArrayentSetProperty("sys-lbname1", buf) < 0) 
	{ 
 		printf("<%s> Could not connect to server.\n",said); 
 		fflush(stdout); 
	} 
	else
	{
		printf("<%s> set d1.applianceconnect.net.\n",said); 
	}

	sprintf(buf,"%s","d2.applianceconnect.net");
	if(ArrayentSetProperty("sys-lbname2", buf) < 0)
	{
 		printf("<%s> Could not connect to server.\n",said);
 		fflush(stdout); 
	}
	else
	{
		printf("<%s> set d2.applianceconnect.net.\n",said); 
	}
	sprintf(buf,"%s","d3.applianceconnect.net");
	if(ArrayentSetProperty("sys-lbname3", buf) < 0) 
	{ 
 		printf("<%s> Could not connect to server.\n",said); 
		fflush(stdout); 
	}
	else
	{
		printf("<%s> set d3.applianceconnect.net.\n",said); 
	}
}

void HandleDataReceivedFromClient(char* data)
{
	//if(strlen(data)==1)
		//InitializeArrayentDaemon();
	//else
	//{
		
	//}
}

void * InitializeTCPServer(void *portNumber)
//void InitializeTCPServer(int portNumber)
{
	int socket_desc , client_sock , c;
    struct sockaddr_in server ;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
		//return -1;
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(*(int*) portNumber );
    //server.sin_port = htons( portNumber );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        //return -1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...at 7055");
    c = sizeof(struct sockaddr_in);
     
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
	pthread_t thread_id;
	
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");

	
         
       if( pthread_create( &thread_id , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
           // return 1;
        }
         
	

        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( thread_id , NULL);
	//connection_handler((void*)&client_sock);
        puts("Handler assigned");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        //return 1;
    }
     
    //return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;

    client=sock;
    int read_size;
    unsigned char *message , client_message[2000];
   
    printf (" connection_handler %d",client);

    isConnected=1;

    message = "Now type something and i shall repeat what you type \n";
   // write(sock , message , strlen(message));
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //end of string marker
		//client_message[read_size] = '\0';
		
		//puts("Message Received");
		int i=0;
		for(i=0;i<read_size;i++)
			printf("%x ",client_message[i]);

		SendData(client_message,read_size);
		
		//Send the message back to client
        //write(sock , client_message , strlen(client_message));
		
		//clear the message buffer
		memset(client_message, 0, 2000);
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
         
    //return 0;
} 

int main(int argc, char** argv)
{
	//listenPort=(int)argv[1];
	listenPort=7055;
	said = argv[1];
	password = argv[2];
	aeskey = argv[3];
	id = argv[4];

	int arg;
	
 
	// Init the semaphores and queues;
	if(sem_init(&callbackSem, 0, 1) < 0) 
	{
		printf("<%s> Unable to establish connection, closing daemon.\n",said);
		fflush(stdout);
		exit(1);
	}
	
	// Initialize connection
	InitializeArrayentWithSaid(said);

	//Configure Connection
	ConfigureConnection(aeskey,said,password,id);

 	// Get network status from server
	printf("Waiting for network status from Arrayent daemon...\n");
	if(ArrayentNetStatus(&arg) < 0)
	{
		printf("Connection to Arrayent daemon failed");
		//bailout(i);
	}
	printf("   status: 0x%04x\n", arg);
	sleep(1);
 
	int hello = ArrayentHello();
	if(hello) 
	{
		printf("\nConnection to Arrayent daemon failed");
		exit(1);
	} 
	else
	{
		printf("success hello: %d\n",hello);
	}

	printf("<%s> Connection to Arrayent daemon sucessful.\n",said);
	fflush(stdout);
  
	pthread_t thread_id;

	 if( pthread_create( &thread_id , NULL ,  InitializeTCPServer , (void*) &listenPort) < 0)
        {
            perror("could not create thread");
            //return 1;
        }
	
		unsigned char networkData[1024]; //used to get data from the cloud
		int length;  //holds the length of data from the cloud
		int timeout =0;
	
	
		////////////////////////////////////////end of setup

		//Main loop, send data and print any incoming data to the screen
		while(1)
		{
			length = 1024;
			int result = ArrayentRecvData(networkData,&length,timeout);


			//printf("Data Listening \n");
			if(result >= 0)
			{
				if(isConnected)
				{
					printf("IsConnected Status ::%d",isConnected);
					send(client, networkData,length, 0);
					//printf("Data Sent to Java Module Length = %d  , Data =%s",length,networkData);
				}
			}
			fflush(stdout);
		}
	
}
