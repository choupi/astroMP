/******************************************************************************
 * serial port programming is modified from miniterm.c
 * 
 * AUTHOR: Sven Goldt (goldt@math.tu-berlin.de)
 * Adapted from the example program distributed with the Linux Programmer's
 * Guide (LPG). This has been robustified and tweaked to work as a debugging 
 * terminal for Xen-based machines.
 * 
 * Modifications are released under GPL and copyright (c) 2003, K A Fraser
 * The original copyright message and license is fully intact below.
 */

#include "mmp.h"
#include "atime.h"
#include "gtocp3.h"

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define DEFAULT_BAUDRATE   230400
#define DEFAULT_GTODEVICE  "/dev/ttyUSB0"
#define ENDMINITERM        0x1b

int cook_baud(int baud)
{
    int cooked_baud = 0;
    switch ( baud )
    {
    case     50: cooked_baud =     B50; break;
    case     75: cooked_baud =     B75; break;
    case    110: cooked_baud =    B110; break;
    case    134: cooked_baud =    B134; break;
    case    150: cooked_baud =    B150; break;
    case    200: cooked_baud =    B200; break;
    case    300: cooked_baud =    B300; break;
    case    600: cooked_baud =    B600; break;
    case   1200: cooked_baud =   B1200; break;
    case   1800: cooked_baud =   B1800; break;
    case   2400: cooked_baud =   B2400; break;
    case   4800: cooked_baud =   B4800; break;
    case   9600: cooked_baud =   B9600; break;
    case  19200: cooked_baud =  B19200; break;
    case  38400: cooked_baud =  B38400; break;
    case  57600: cooked_baud =  B57600; break;
    case 115200: cooked_baud = B115200; break;
    case 230400: cooked_baud = B230400; break;
    case 460800: cooked_baud = B460800; break;
    case 500000: cooked_baud = B500000; break;
    }
    return cooked_baud;
}

int main(int argc, char **argv)
{
	pthread_t threads[3];
	void *res;
    int fd, gto, c, cooked_baud;
	int bc=0, rs=0, cycle;
	double t,t0,t1;
	double decl=0, ra=0;
	int hh, mm, ss;
	char sg;
    char *gtoname = DEFAULT_GTODEVICE;
	char *tmpi;
	char lrfmt[]="%02d:%02d:%02d";
	char ldfmt[]="%c%02d*%02d:%02d";

    struct termios   oldsertio, newsertio, oldgtotio, newgtotio;

    while ( --argc != 0 )
    {
        char *p = argv[argc];
        if ( *p++ != '-' )
            goto usage;
        if ( *p == 'b' )
        {
            p++;
            if ( (cooked_baud = cook_baud(atoi(p))) == 0 )
            {
                fprintf(stderr, "Bad baud rate '%d'\n", atoi(p));
                goto usage;
            }
        }
        else if ( *p == 'd' )
        {
            tmpi = ++p;
            if ( *tmpi == '\0' )
                goto usage;
			sscanf(tmpi, ldfmt, &sg, &hh, &mm, &ss);
			decl=(sg=='+'?1:-1)*decimal_hours(hh,mm,ss);
			printf("DECL %lf %d %d %d\n", decl, hh,mm,ss);
        }
        else if ( *p == 'R' )
        {
            tmpi = ++p;
            if ( *tmpi == '\0' )
                goto usage;
			sscanf(tmpi, lrfmt, &hh, &mm, &ss);
			ra=decimal_hours(hh,mm,ss);
			printf("RA %lf %d %d %d\n", ra, hh,mm,ss);
        }
        else if ( *p == 'D' )
        {
            gtoname = ++p;
            if ( *gtoname == '\0' )
                goto usage;
        }
		else if ( *p == 'r' )
		{
			rs=1;
		}
        else
            goto usage;
    }

    gto = open(gtoname, O_RDWR | O_NOCTTY);
    if ( gto < 0 )
    {
        perror(gtoname); 
        exit(-1);
    }
 
 	cooked_baud=cook_baud(DEFAULT_BAUDRATE);
    tcgetattr(gto,&oldgtotio);
    newgtotio.c_iflag = IGNBRK | IGNPAR;
    newgtotio.c_oflag = OPOST;
    newgtotio.c_cflag = cooked_baud | CS8 | CLOCAL | CREAD;
    newgtotio.c_lflag = 0;
    newgtotio.c_cc[VMIN]=0;
    newgtotio.c_cc[VTIME]=8;
    tcflush(gto, TCIFLUSH);
    tcsetattr(gto,TCSANOW,&newgtotio);
	
	mmp_fd=gto;
	GTOCP3_init_f("gto.conf", rs);

	gto_m.dst[GTO_RA]=ra;
	gto_m.dst[GTO_DECL]=decl;
	gto_m.RDrun=1;
	printf("GOTO RD: %.4lf %.4lf\n", ra, decl);
	gto_track();
    return 0;

 usage:
    printf("miniterm -d+HH*MM:SS -RHH:MM:SS [-b<baudrate>] [-D<devicename>]\n");
    printf("Default baud rate: %d\n", DEFAULT_BAUDRATE);
    printf("Default device: %s\n", DEFAULT_GTODEVICE);
	printf("-r: set position\n");
	printf("-d: set decl\n");
	printf("-R: set RA\n");
    return 1;
}
