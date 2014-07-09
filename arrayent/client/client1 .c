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
#include <string.h>
#include <stdint.h>

#include "aic.h"
#pragma pack(1)

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
	ECM_INFO=0xE3
};

enum REVEAL_OPCODE
{
	GET_COMM_MODULE_INFO=0x02,
	GET_BOARD_INFO=0x03
};

const char *brand = "01";
const char *category = "06";
const char *modelNumber = "WEL98HEBU0";
const char *serialNumber="21901031";
const char *macAddress="88:e7:12:00:27:7b";

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

	printf("inside binaryToHexString");
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
	printf("Out of conv");
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
	strcat(result,hexValue);
	strcat(result,"ED");
	size = (int) size/2;
	size+=3;
	printf("Size<%x>Size",size);

	sprintf(hexValue,"%x",size);
	//hexValue[0]=(unsigned char)((size&0xFF00)>>4);
	//hexValue[1]=(unsigned char)(size&0x00FF);

	strcat(result,hexValue);
	printf("Data :%s",result);
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
		
	printf("Data :%s",result);
	
	return result;
	
}

char* GetDeviceInformation(unsigned short transId, unsigned char opcode,char* said)
{
	char *result;
	 
	switch(opcode)
	{
		case GET_COMM_MODULE_INFO:
			result=PublishDeviceInformation(transId,said);
			
		break;
		case GET_BOARD_INFO:
			result=GetCommunicationModuleInformation(transId);
		default : break;
	}
	printf("GetDeviceInformation\n");
	return result;	
}


char* GetCommunicationModuleInformation(unsigned short transId)
{
	char tmp[1024];
	char boardPartNumber[] = "000000000000000000";

	
	strcpy(boardPartNumber,asciiToHexString("W00000001",tmp));

	char boardRevisionNumber[] = "00";
	strcpy(boardRevisionNumber,asciiToHexString("1",tmp));

	char projectReleaseNumber[] = "000000000000000000";
	strcpy(projectReleaseNumber,asciiToHexString("W10605695",tmp));
	char result[1024];
	result[0]=0;	
	char gcmFirstPart[] = "0000ED0018E322";
	strcat(result,gcmFirstPart);
	strcat(result,boardPartNumber);
	strcat(result,"00");
	strcat(result,boardRevisionNumber);
	strcat(result,"00");
	strcat(result,projectReleaseNumber);
	strcat(result,"00");

	//printf("<%s> DeviceINformation ", result);
	return result;
}

//Take a packet (ASCII Hex), look at bits, decide if there needs to be a response
//and then send the appropriate response
void sniff(unsigned char* packet, char* said) 
{
	char truePacket[1024];
	//char buff[1024];
	char tmp[1024];
	char message[1024];
	
	REVELATION_PACKET *revealPacket;
	revealPacket = (REVELATION_PACKET *) packet;

	revealPacket->transactionId=ReverseBytes(revealPacket->transactionId);
	int messageLength = 0;
	int len;
	char* deviceInfo ;

	
	switch(revealPacket->reveal_api)
	{
		case ECM_INFO:	deviceInfo = GetDeviceInformation(revealPacket->transactionId,revealPacket->reveal_opcode,said);
				
				hexStringToBinary(deviceInfo,message,&messageLength);

				if(ArrayentSendData(message, messageLength+1, 10000) < 0) 
				{
					printf("<%s> Could not send message\n",said);
				}
				break;
		case 
		
		default:break;
	}
  
	/*if(strcmp(truePacket,DeviceInformation) == 0)
	{
		
	
		//char* deviceInfo= GetDeviceInformation(gdiFirstPart,said);
    
		//printf("Inside Sniffer %s",deviceInfo);
		int messageLength = 0;
    
		printf("<%s> Sending message: %s\n",said,deviceInfo);

		hexStringToBinary(deviceInfo,message,&messageLength);

		if(ArrayentSendData(message, messageLength+1, 10000) < 0) 
		{
			printf("<%s> Could not send message\n",said);
		}
  
    	
	}

	  else if(strcmp(truePacket,GetCommModuleInformation) == 0) 
	  {
		char *devCommInfo=GetCommunicationModuleInformation();
    
		int messageLength = 0;
    
		printf("<%s> Sending message: %s\n",said,devCommInfo);

		hexStringToBinary(devCommInfo,message,&messageLength);

		if(ArrayentSendData(message, messageLength + 1, 10000) < 0)
		{
		  printf("<%s> Could not send message\n",said);
		}

	}*/
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

int main(int argc, char** argv)
{
	char* said = argv[1];
	char* password = argv[2];
	char* aeskey = argv[3];
	char* id = argv[4];
  
	int arg;
	char buf[4096]; 
 
	/*//Test
	unsigned char data[9]={0x36, 0x00,0xed,0x00,0x02,0xe3,0x02,0x01,0x02};
	sniff(data,said);
	printf("Exit");*/

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
 
	//usleep(5000000);

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

	char input[1024];  //used to collect data from outside
	char buffer[1024]; //used to send data to cloud
  
	unsigned char networkData[1024]; //used to get data from the cloud
	int length;  //holds the length of data from the cloud
	int timeout =0;

	
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


	////////////////////////////////////////end of setup

	//Main loop, send data and print any incoming data to the screen
	while(1)
	{
		
		 //Get input from the server
		//while(1) 
		//{
			length = 1024;
			int result = ArrayentRecvData(networkData,&length,timeout);

			//printf("Arrayent Recv Data Result : %d",result);
			if(result >= 0)
			{
				char tmpBuffer[2048];
				//printf("<%x>Length:\n",length);
				printf("<%s> Data received from server: %s\n",said,binaryToHexString(networkData,tmpBuffer,length));
				//sniff(tmpBuffer,said);
				sniff(networkData,said);
			}
			else
			{
				//printf("No Data\n");
			}
			fflush(stdout);
			if(result < 0)
			{
				//break;
			}
		//}
		fflush(stdout);

	}
    
}
