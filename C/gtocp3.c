#include "gtocp3.h"
#include "mmp.h"
#include "atime.h"
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIFFS (2)

double ANGtoPOS(double ang, double rate, double p0, double angM)
{
	return fmod(ang+p0, angM)*rate;
}

double POStoANG(double pos, double rate, double p0, double angM)
{
	return fmod(pos/rate+(angM-p0), angM);
}

void Track_speed(double alt, double azm, double *alts, double *azms)
{
	int st;
	MMPGetSampleTime(&st);
	*alts=fabs(gto_m.asix[GTO_ALT]-alt)*gto_m.rate[GTO_ALT]*st/(1000000*DIFFS);
	*azms=fabs(gto_m.asix[GTO_AZM]-azm)*gto_m.rate[GTO_AZM]*st/(1000000*DIFFS);
	if(*alts>gto_m.maxspd) *alts=gto_m.maxspd;
	if(*azms>gto_m.maxspd) *azms=gto_m.maxspd;
}

void GTOCP3_init_f(char *fname, int rs)
{
	FILE* f=fopen(fname, "r");
	double t,t0,t1;
	int pos0, pos1;
	gto_m.RDrun=0;
	gto_getpos(&pos0, &pos1);
	gto_m.p0[GTO_ALT]=0;
	gto_m.p0[GTO_AZM]=70;
	fscanf(f, "%lf\n", &t);
	gto_m.loc[GTO_LAT]=t;
	fscanf(f, "%lf\n", &t);
	gto_m.loc[GTO_LONGTD]=t;
	fscanf(f, "%lf\n", &t);
	gto_m.rate[GTO_ALT]=t;
	fscanf(f, "%lf\n", &t);
	gto_m.rate[GTO_AZM]=t;
	fscanf(f, "%lf\n", &t);
	gto_m.maxspd=t;
	MMPSetProfileMode(GTO_ALT, 0);
	MMPSetProfileMode(GTO_AZM, 0);
	MMPSetMotorType(GTO_ALT,4);
	MMPSetMotorType(GTO_AZM,4);
	MMPSetVelocity(GTO_ALT,t*65536);
	MMPSetVelocity(GTO_AZM,t*65536);
	MMPSetAcceleration(GTO_ALT,30);
	MMPSetAcceleration(GTO_AZM,30);
	MMPUpdateAll();
	printf("lat= %.4lf longtd= %.4lf\n", 
		gto_m.loc[GTO_LAT], gto_m.loc[GTO_LONGTD]);
	printf("alt rate= %.4lf azm rate= %.4lf\n", 
		gto_m.rate[GTO_ALT], gto_m.rate[GTO_AZM]);
	close(f);
	if(rs) {
		printf("set alt azm:"); scanf("%lf %lf", &t0,&t1);
		MMPSetActualPosition(GTO_ALT,ANGtoPOS(t0,gto_m.rate[GTO_ALT],
										gto_m.p0[GTO_ALT], 90.0000));
		MMPSetActualPosition(GTO_AZM,ANGtoPOS(t1,gto_m.rate[GTO_AZM], 
										gto_m.p0[GTO_AZM], 360.0000));
	} else {
		MMPSetActualPosition(GTO_ALT,pos0);
		MMPSetActualPosition(GTO_AZM,pos1);
		gto_update();
	}
}


void GTOCP3_init(double lat, double longtd, double r0, double r1)
{
	gto_m.RDrun=0;
	MMPSetProfileMode(GTO_ALT, 0);
	MMPSetProfileMode(GTO_AZM, 0);
	MMPSetMotorType(GTO_ALT,4);
	MMPSetMotorType(GTO_AZM,4);
	MMPSetVelocity(GTO_ALT,1.8*65536);
	MMPSetVelocity(GTO_AZM,1.8*65536);
	MMPSetAcceleration(GTO_ALT,30);
	MMPSetAcceleration(GTO_AZM,30);
	MMPUpdateAll();
	gto_m.loc[GTO_LAT]=lat;
	gto_m.loc[GTO_LONGTD]=longtd;
	gto_m.rate[GTO_ALT]=r0;
	gto_m.rate[GTO_AZM]=r1;
	gto_m.p0[GTO_ALT]=0;
	gto_m.p0[GTO_AZM]=0;
}

int GTOCP3_read_G(int fd)
{
	char c;
	char rfmt[]="%02d:%02d.%01d#";
	//char lrfmt[]="%02d:%02d:%02d.0#";
	char lrfmt[]="%02d:%02d:%02.1lf#";
	char dfmt[]="%c%02d*%02d#";
	char ldfmt[]="%c%02d*%02d:%02d#";
	char resp[15];
	struct tm ts;
	struct tm* ptm;
	time_t rtime;
	double ha, decl, sec;
	read(fd,&c,1);
	printf("%c", c);
	switch(c) {
	case 'R':
		read(fd,&c,1);
		printf("%c@", c);
		if(c!='#') return 1;

		time(&rtime);
	    ptm=gmtime(&rtime);

		eqt_coord(&ha, &decl, gto_m.asix[GTO_ALT], gto_m.asix[GTO_AZM],
			gto_m.loc[GTO_LAT]);
		//RAtoHA_d2tm(&ts, ptm, ha, gto_m.loc[GTO_LONGTD]);
		RAtoHA_d2tm_usec(&ts, ptm, &sec, ha, gto_m.loc[GTO_LONGTD]);

		//sprintf(resp, rfmt, ts.tm_hour,ts.tm_min,ts.tm_sec/6);
		//sprintf(resp, lrfmt, ts.tm_hour,ts.tm_min,ts.tm_sec);
		sprintf(resp, lrfmt, ts.tm_hour,ts.tm_min,sec);
		printf("%s\n", resp);
		//printf("t=%s\n", ctime(&rtime));
		write(fd,resp,strlen(resp));
		//free(ptm);
		break;
	case 'D':
		read(fd,&c,1);
		printf("%c@", c);
		if(c!='#') return 1;

		eqt_coord(&ha, &decl, gto_m.asix[GTO_ALT], gto_m.asix[GTO_AZM],
			gto_m.loc[GTO_LAT]);
		time_hours(&ts,decl);

		//sprintf(resp, dfmt, ts.tm_hour,ts.tm_min);
		sprintf(resp, ldfmt, decl<0?'-':'+', ts.tm_hour,ts.tm_min, ts.tm_sec);
		printf("%s\n", resp);
		write(fd,resp,strlen(resp));
		break;
	default:
		return 1;
	}
	return 0;
}

int GTOCP3_read_S(int fd)
{
	char c, sg;
	char rfmt[]=" %02d:%02d:%02d#";
	char lrfmt[]=" %02d:%02d:%02d.0#";
	char dfmt[]=" %c%02d*%02d#";
	char ldfmt[]=" %c%02d*%02d:%02d#";
	char resp[15];
	struct tm ts;
	int hh,mm,ss;
	read(fd,&c,1);
	printf("%c", c);
	usleep(80000);
	switch(c) {
	case 'r':
		read(fd,resp,12);
		resp[12]='\0';
		printf("%s\n",resp);
		sscanf(resp, rfmt, &hh, &mm, &ss);
		gto_m.dst[GTO_RA]=decimal_hours(hh,mm,ss);
	//	printf("!!!%d %d %d!!!\n", hh,mm,ss);
		printf("$$%.4lf\n", gto_m.dst[GTO_RA]);
		c='1';
		write(fd,&c,1);
		break;
	case 'd':
		read(fd,resp,11);
		resp[11]='\0';
		printf("%s\n",resp);
		sscanf(resp, ldfmt, &sg, &hh, &mm,&ss);
		gto_m.dst[GTO_DECL]=(sg=='+'?1:-1)*decimal_hours(hh,mm,ss);
		//printf("!!!%d %d %d!!!\n", hh,mm,ss);
		printf("$$%.4lf\n", gto_m.dst[GTO_DECL]);
		c='1';
		write(fd,&c,1);
		break;
	default:
		return 1;
	}
	return 0;
}

int GTOCP3_read_M(int fd)
{
	char c;
	read(fd,&c,1);
	printf("%c", c);
	if(c!='S') return 1;
	read(fd,&c,1);
	printf("%c\n", c);
	if(c!='#') return 1;
	gto_m.RDrun=1;
	c='0';
	write(fd,&c,1);
	return 0;
}

int GTOCP3_read_Q(int fd)
{
	char c;
	read(fd,&c,1);
	printf("%c\n", c);
	if(c!='#') return 1;
	gto_m.RDrun=0;
	return 0;
}

int GTOCP3_read_C(int fd)
{
	char c;
	char *resp="Coordinates     matched.        #";
	read(fd,&c,1);
	printf("%c", c);
	if(c!='M') return 1;
	read(fd,&c,1);
	printf("%c\n", c);
	if(c!='#') return 1;
	gto_sync();
	write(fd,resp,strlen(resp));
	return 0;
}

void GTOCP3_read(int fd)
{
	char c;
	while(read(fd,&c,1)) {
		printf("%c", c);
		if(c!=':') continue;
		read(fd,&c,1);
		printf("%c", c);
		switch(c) {
		case 'G':
			GTOCP3_read_G(fd);
			break;
		case 'S':
			GTOCP3_read_S(fd);
			break;
		case 'M':
			GTOCP3_read_M(fd);
			break;
		case 'Q':
			GTOCP3_read_Q(fd);
			break;
		case 'C':
			GTOCP3_read_C(fd);
			break;
		default:
			break;
		}
	}
}

void gto_goto(double ra, double decl)
{
	double alt, azm, ha;
	time_t rtime;
	struct tm *ptm;

	time(&rtime);
	ptm=gmtime(&rtime);
	ha=RAtoHA(ra, UTtoLST_tm(ptm,gto_m.loc[GTO_LONGTD]));
//	ha=RAtoHA(ra, 0);
	horizon_coord(&alt, &azm, ha, decl, gto_m.loc[GTO_LAT]);
	gto_goto_aa(alt, azm);
	//free(ptm);
}

void gto_goto_aa(double alt, double azm)
{
	int pos0, pos1;

	if(alt<0) alt=0;
	pos0=ANGtoPOS(alt, gto_m.rate[GTO_ALT], gto_m.p0[GTO_ALT], 90.0000);
	MMPSetPosition(GTO_ALT, pos0);
	pos1=ANGtoPOS(azm, gto_m.rate[GTO_AZM], gto_m.p0[GTO_AZM], 360.0000);
	MMPSetPosition(GTO_AZM, pos1);
	MMPUpdateAll();
	printf("gto_goto_aa: %.4lf %.4lf (%d %d)\n", 
		alt, azm, pos0, pos1);
	//free(ptm);
}

void gto_track()
{
	int i0, i1;
	double alt, azm, ha, ra, decl;
	double s0, s1;
	time_t rtime;
	struct tm *ptm;
	struct timeval tv;
	struct timezone tz;

	do {
		gto_update();
		if(!gto_m.RDrun) continue;
		ra=gto_m.dst[GTO_RA];
		decl=gto_m.dst[GTO_DECL];
		printf("ra decl:%.2lf %.2lf\n", ra, decl);
//		time(&rtime);
//		rtime+=DIFFS;
		gettimeofday(&tv, &tz);
		rtime=tv.tv_sec + DIFFS;
		ptm=gmtime(&rtime);
//		ha=RAtoHA(ra, UTtoLST_tm(ptm,gto_m.loc[GTO_LONGTD]));
		ha=RAtoHA(ra, UTtoLST_tm_usec(ptm,gto_m.loc[GTO_LONGTD],
				tv.tv_usec/1000000.0));
		//	ha=RAtoHA(ra, 0);
		horizon_coord(&alt, &azm, ha, decl, gto_m.loc[GTO_LAT]);
		Track_speed(alt, azm, &s0, &s1);
		printf("spd %.6lf %.6lf\n", s0, s1);
		MMPSetVelocity(GTO_ALT, s0*65536);
		MMPSetVelocity(GTO_AZM, s1*65536);
		gto_goto_aa(alt, azm);
	} while(!usleep(500000));
//	MMPGetVelocity(0,&i0);
//	MMPGetVelocity(1,&i1);
//	printf("%d %d\n", i0, i1);
//	MMPGetTime(&i0);
//	printf("%d\n", i0);
}

void gto_getpos(int *pos0, int *pos1)
{
	MMPGetActualPosition(GTO_ALT, pos0);
	MMPGetActualPosition(GTO_AZM, pos1);
}

void gto_update()
{
	int pos0, pos1;
	gto_getpos(&pos0, &pos1);
	gto_m.asix[GTO_ALT]=POStoANG(pos0, gto_m.rate[GTO_ALT], 
					gto_m.p0[GTO_ALT], 90.0000);
	gto_m.asix[GTO_AZM]=POStoANG(pos1, gto_m.rate[GTO_AZM], 
					gto_m.p0[GTO_AZM], 360.0000);
	printf("gto_update: %.4lf %.4lf (%d %d)\n", 
			gto_m.asix[GTO_ALT], gto_m.asix[GTO_AZM], pos0, pos1); 
//	MMPGetMotorType(0,&pos0);
//	MMPGetMotorType(0,&pos1);
//	printf("gto_update(mt): %d %d\n", pos0, pos1);
}

void gto_maxspd()
{
	MMPSetVelocity(GTO_ALT, gto_m.maxspd*65536);
	MMPSetVelocity(GTO_AZM, gto_m.maxspd*65536);
}

void gto_sync()
{
	int pos0, pos1;
	double alt, azm, ha, ra, decl;
	time_t rtime;
	struct tm *ptm;
	struct timeval tv;
	struct timezone tz;
	ra=gto_m.dst[GTO_RA];
	decl=gto_m.dst[GTO_DECL];
	printf("sync to ra decl:%.2lf %.2lf\n", ra, decl);
	gettimeofday(&tv, &tz);
	rtime=tv.tv_sec;
	ptm=gmtime(&rtime);
	ha=RAtoHA(ra, UTtoLST_tm_usec(ptm,gto_m.loc[GTO_LONGTD],
				tv.tv_usec/1000000.0));
	horizon_coord(&alt, &azm, ha, decl, gto_m.loc[GTO_LAT]);
	pos0=ANGtoPOS(alt, gto_m.rate[GTO_ALT], gto_m.p0[GTO_ALT], 90.0000);
	MMPSetActualPosition(GTO_ALT, pos0);
	pos1=ANGtoPOS(azm, gto_m.rate[GTO_AZM], gto_m.p0[GTO_AZM], 360.0000);
	MMPSetActualPosition(GTO_AZM, pos1);
}

