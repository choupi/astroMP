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
#include <pthread.h>

#define DEFAULT_BAUDRATE   230400
#define DEFAULT_SBAUDRATE  9600
#define DEFAULT_SERDEVICE  "/dev/ttyUSB1"
#define DEFAULT_GTODEVICE  "/dev/ttyUSB0"
#define ENDMINITERM        0x1b

#define TW_LAT (25.02)
#define TW_LONGTD (121.45)
#define M_RATE0 (300)
#define M_RATE1 (3000)

volatile int stop = 0;

void child_handler(int s)
{
    stop = 1;
}

void *child_thread(void* t)
{
	int fd=(int)t;
	GTOCP3_read(fd);
}

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
	pthread_t threads;
    int fd, gto, c, cooked_baud;
	int bc=0, rs=0, cycle;
	double t, t0,t1;
    char *sername = DEFAULT_SERDEVICE;
    char *gtoname = DEFAULT_GTODEVICE;
    struct termios   oldsertio, newsertio, oldgtotio, newgtotio;
    struct sigaction sa;
    static char start_str[] = 
        "************ REMOTE CONSOLE: CTRL-[ TO QUIT ********\r\n";
    static char end_str[] =
        "\n************ REMOTE CONSOLE EXITED *****************\n";
	char op[] = "PK02003E\n\r";
	char buf[100];

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
            sername = ++p;
            if ( *sername == '\0' )
                goto usage;
        }
		
        else if ( *p == 'D' )
        {
            gtoname = ++p;
            if ( *gtoname == '\0' )
                goto usage;
        }
		else if ( *p == 'g' )
		{
			bc=0;
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
	GTOCP3_init(TW_LAT, TW_LONGTD, M_RATE0, M_RATE1);
	GTOCP3_init_f("gto.conf", rs);

	if(bc) {
    fd = open(sername, O_RDWR | O_NOCTTY);
    if ( fd < 0 )
    {
        perror(sername); 
        exit(-1);
    }
 	cooked_baud=cook_baud(DEFAULT_SBAUDRATE);
    tcgetattr(fd, &oldsertio); 
    newsertio.c_cflag = cooked_baud | CS8 | CLOCAL | CREAD;
    newsertio.c_iflag = IGNBRK | IGNPAR;
    newsertio.c_oflag = OPOST;
    newsertio.c_lflag = 0;
    newsertio.c_cc[VMIN]=1;
    newsertio.c_cc[VTIME]=0;
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newsertio);
 
	pthread_create(&threads, NULL, child_thread, (void*)fd);
	}

			printf("alt: "); scanf("%lf", &t0);
			printf("azm: "); scanf("%lf", &t1);
			gto_goto_aa(t0,t1);
		do {
			gto_update();
		/*
		bc=read(gto,buf,50);
		if(bc>0) write(1, buf, bc);
		printf("****\n"); fflush(stdout);
		MMPGetTime(&cycle);
		printf("%d# ", cycle);
		MMPGetProfileMode(0,&cycle);
		printf("%d$ ",cycle);
		MMPGetPosition(0,&cycle);
		printf("%d@ ",cycle);
		MMPGetAcceleration(0,&cycle);
		printf("%.2f/",cycle/65536.0);
		MMPGetDeceleration(0,&cycle);
		printf("%.2f\n",cycle/65536.0);
		MMPSetActualPosition(0, 100);
		MMPGetActualPosition(0, &cycle);
		printf("%d@\n",cycle);
		*/
		} while(!usleep(1500000));
	
    return 0;

 usage:
    printf("miniterm [-b<baudrate>] [-d<devicename>]\n");
    printf("Default baud rate: %d\n", DEFAULT_BAUDRATE);
    printf("Default device: %s\n", DEFAULT_SERDEVICE);
    return 1;
}
