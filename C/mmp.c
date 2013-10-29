#include "mmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>

void MMPSetMotorType(int asix, int mode)
{
	char cform[]="PK10%02d02 %04X\n\r";
	char command[16];
	char result[3]={'\0'};
	sprintf(command, cform, asix, mode);
	write(mmp_fd,command,15);
	usleep(50000);
	read(mmp_fd, result, 2);
}

void MMPGetMotorType(int asix, int* mode)
{
	char cform[]="PK01%02d03\n\r";
	char command[11];
	char result[8]={'\0'};
	sprintf(command, cform, asix);
	tcflush(mmp_fd, TCIFLUSH);
	write(mmp_fd,command,10);
	usleep(50000);
	read(mmp_fd, result, 7);
	sscanf(result, "%X", mode);
	//printf("GetProfMode: %s\n", result);
}

void MMPSetProfileMode(int asix, int mode)
{
	char cform[]="PK10%02dA0 %04X\n\r";
	char command[16];
	char result[3]={'\0'};
	sprintf(command, cform, asix, mode);
	write(mmp_fd,command,15);
	usleep(50000);
	read(mmp_fd, result, 2);
}

void MMPGetProfileMode(int asix, int* mode)
{
	char cform[]="PK01%02dA1\n\r";
	char command[11];
	char result[8]={'\0'};
	sprintf(command, cform, asix);
	tcflush(mmp_fd, TCIFLUSH);
	write(mmp_fd,command,10);
	usleep(50000);
	read(mmp_fd, result, 7);
	sscanf(result, "%X", mode);
	//printf("GetProfMode: %s\n", result);
}

void MMPSetPosition(int asix, int pos)
{
	char cform[]="PK20%02d10 %04X %04X\n\r";
	char command[21]={'\0'};
	char result[3]={'\0'};
	sprintf(command, cform, asix, (unsigned int)pos/0x10000, pos&(0xFFFF));
	write(mmp_fd,command,20);
	usleep(50000);
	read(mmp_fd, result, 2);
	//printf("%s\n", command);
}

void MMPGetPosition(int asix, int* pos)
{
	char cform[]="PK02%02d4A\n\r";
	char command[11];
	char result[13]={'\0'};
	unsigned int p1,p2;
	sprintf(command, cform, asix);
	tcflush(mmp_fd, TCIFLUSH);
	write(mmp_fd,command,10);
	usleep(50000);
	read(mmp_fd, result, 12);
	//printf("%s\n", result);
	sscanf(result, "%X %X", &p1,&p2);
	*pos=(p1<<16)+p2;
}

void MMPSetVelocity(int asix, int velc)
{
	char cform[]="PK20%02d11 %04X %04X\n\r";
	char command[21];
	char result[3]={'\0'};
	sprintf(command, cform, asix, velc/0x10000, velc&0xFFFF);
	write(mmp_fd,command,20);
	usleep(50000);
	read(mmp_fd, result, 2);
	//printf("%s\n", result);
}

void MMPGetVelocity(int asix, int* velc)
{
	char cform[]="PK02%02d4B\n\r";
	char command[11];
	char result[13]={'\0'};
	int p1,p2;
	sprintf(command, cform, asix);
	tcflush(mmp_fd, TCIFLUSH);
	write(mmp_fd,command,10);
	usleep(50000);
	read(mmp_fd, result, 12);
	sscanf(result, "%X %X", &p1,&p2);
	*velc=(p1<<16)+p2;
}

void MMPSetAcceleration(int asix, int acc)
{
	char cform[]="PK20%02d90 %04X %04X\n\r";
	char command[21];
	char result[3]={'\0'};
	sprintf(command, cform, asix, acc/0x10000, acc&0xFFFF);
	write(mmp_fd,command,20);
	usleep(50000);
	read(mmp_fd, result, 2);
}

void MMPGetAcceleration(int asix, int* acc)
{
	char cform[]="PK02%02d4C\n\r";
	char command[11];
	char result[13]={'\0'};
	int p1,p2;
	sprintf(command, cform, asix);
	tcflush(mmp_fd, TCIFLUSH);
	write(mmp_fd,command,10);
	usleep(50000);
	read(mmp_fd, result, 12);
	sscanf(result, "%X %X", &p1,&p2);
	*acc=(p1<<16)+p2;
}

void MMPSetDeceleration(int asix, int dece)
{
	char cform[]="PK20%02d91 %04X %04X\n\r";
	char command[21];
	char result[3]={'\0'};
	sprintf(command, cform, asix, dece/0x10000,dece&0xFFFF);
	write(mmp_fd,command,20);
	usleep(50000);
	read(mmp_fd, result, 2);
}

void MMPGetDeceleration(int asix, int* dece)
{
	char cform[]="PK02%02d92\n\r";
	char command[11];
	char result[13]={'\0'};
	int p1,p2;
	sprintf(command, cform, asix);
	tcflush(mmp_fd, TCIFLUSH);
	write(mmp_fd,command,10);
	usleep(50000);
	read(mmp_fd, result, 12);
	sscanf(result, "%X %X", &p1,&p2);
	*dece=(p1<<16)+p2;
}

void MMPUpdate(int asix)
{
	char cform[]="PK00%02d1A\n\r";
	char command[11];
	char result[3]={'\0'};
	sprintf(command, cform, asix);
	tcflush(mmp_fd, TCIFLUSH);
	write(mmp_fd,command,10);
	usleep(50000);
	read(mmp_fd, result, 2);
}

void MMPUpdateAll()
{
	char cform[]="PK10005B %04X\n\r";
	char command[16];
	char result[3]={'\0'};
	sprintf(command, cform, 3);
	tcflush(mmp_fd, TCIFLUSH);
	write(mmp_fd,command,15);
	usleep(50000);
	read(mmp_fd, result, 2);
}

void MMPGetTime(int* cycle)
{
	char cform[]="PK02003E\n\r";
	char command[11];
	char result[13]={'\0'};
	int c1,c2;
	sprintf(command, cform);
	tcflush(mmp_fd, TCIFLUSH);
	write(mmp_fd,command,10);
	usleep(50000);
	read(mmp_fd, result, 12);
	sscanf(result, "%X %X", &c1, &c2);
	*cycle=(c1<<16)+c2;
}

void MMPGetSampleTime(int* msec)
{
	char cform[]="PK02003C\n\r";
	char command[11];
	char result[13]={'\0'};
	int c1,c2;
	sprintf(command, cform);
	tcflush(mmp_fd, TCIFLUSH);
	write(mmp_fd,command,10);
	usleep(50000);
	read(mmp_fd, result, 12);
	sscanf(result, "%X %X", &c1, &c2);
	*msec=(c1<<16)+c2;
}

void MMPSetActualPosition(int asix, int pos)
{
	char cform[]="PK20%02d4D %04X %04X\n\r";
	char command[21];
	char result[3]={'\0'};
	sprintf(command, cform, asix, (unsigned int)pos/0x10000, pos&0xFFFF);
	write(mmp_fd,command,20);
	usleep(50000);
	read(mmp_fd, result, 2);
//	printf("%s\n", command);
}

void MMPGetActualPosition(int asix, int* pos)
{
	char cform[]="PK02%02d37\n\r";
	char command[11];
	char result[13]={'\0'};
	unsigned int p1,p2;
	sprintf(command, cform, asix);
	tcflush(mmp_fd, TCIFLUSH);
	write(mmp_fd,command,10);
	usleep(50000);
	read(mmp_fd, result, 12);
	sscanf(result, "%X %X", &p1,&p2);
	*pos=(p1<<16)+p2;
//	printf("%s\n", result);
}

void MMPReset()
{
	char cform[]="PK000039\n\r";
	char command[11];
	char result[3]={'\0'};
	sprintf(command, cform);
	write(mmp_fd,command,10);
	read(mmp_fd, result, 2);
}
