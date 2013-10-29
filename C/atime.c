#include <stdio.h>
#include <math.h>
#include <time.h>

#define deg(x) (x*M_PI/180.0000)
#define adeg(x) (x*180.0000/M_PI)


double decimal_hours(int hour, int minute, int second)
{
	return ((second/60.0)+minute)/60.0+hour;
}

double decimal_hours_tm(struct tm *ts)
{
	return decimal_hours(ts->tm_hour,ts->tm_min,ts->tm_sec);
}

double decimal_hours_tm_usec(struct tm *ts, double usec)
{
	return decimal_hours(ts->tm_hour,ts->tm_min,ts->tm_sec)+(usec/3600.0);
}

void time_hours(struct tm *ts, double deci)
{
	double t;
	time_hours_usec(ts, &t, deci);
}

void time_hours_usec(struct tm *ts, double *sec, double deci)
{
	int tmp;
	deci=fabs(deci);
	tmp=deci;
	ts->tm_hour=tmp;
	deci=(deci-tmp)*60.0;
	tmp=deci;
	ts->tm_min=tmp;
	deci=(deci-tmp)*60.0;
	ts->tm_sec=deci;
	*sec=deci;
}

double julian_day(struct tm *ts)
{
	int y=ts->tm_year+1900;
	int m=ts->tm_mon+1;
	int d=ts->tm_mday;
	/*
	int a=(14-m)/12;
	y=y+4800-a;
	m=m+12*a-3;
	return d+ (153*m+2)/5 + 365*y +y/4 -y/100 +y/400 -32045;
	*/
	int A, B=0, C, D;
	if(m<=2) { y-=1;m+=12; }
	A=y/100;
	if(y >1582 ||(y=1582 && m >=10 && d >=15)) {
		B= 2-A+ (A/4);
	}
	if(y<0) C= 365.25*y-0.75;
	else C= 365.25*y;
	D=30.6001*(m+1);
	return B+C+D+d+1720994.5;
}

void calender_day(struct tm *ts, double jd)
{
	int I=jd+0.5;
	double F=jd+0.5-I;
	int A,B,C,D,E,G;
	if(I>2299160) {
		A=(I-1867216.25)/36524.25;
		B=I+1+A-(A/4);
	} else B=I;
	C=B+1524;
	D=(C-122.1)/365.25;
	E=365.25*D;
	G=(C-E)/30.6001;
	ts->tm_mday=C-E+F-(int)(30.6001*G);
	ts->tm_mon=G>13.5? G-13: G-1;
	ts->tm_year=(ts->tm_mon > 2.5 ? D-4716 : D-4715) -1900;
}

double UTtoGST(double jd, double dec_UT)
{
	double S=jd-2451545.0;
	double T=S/36525.0;
	double T0=fmod(6.697374558 + (2400.051336*T) +(0.000025862*T*T),24);
	double gst= fmod(dec_UT*1.002737909 + T0, 24);
	return gst<0?gst+24:gst;
}

double UTtoGST_tm(struct tm* uttm)
{
	return UTtoGST(julian_day(uttm), decimal_hours_tm(uttm));
}

double UTtoGST_tm_usec(struct tm* uttm, double usec)
{
	return UTtoGST(julian_day(uttm), decimal_hours_tm_usec(uttm,usec));
}

double GSTtoLST(double longtd, double gst)
{
	double lst=fmod(gst+longtd/15.0, 24);
	return lst<0?lst+24:lst;
}

double UTtoLST_tm(struct tm* uttm, double longtd)
{
	return GSTtoLST(longtd, UTtoGST_tm(uttm));
}

double UTtoLST_tm_usec(struct tm* uttm, double longtd, double usec)
{
	return GSTtoLST(longtd, UTtoGST_tm_usec(uttm, usec));
}

double RAtoHA(double ra, double lst)
{
	double ha=lst-ra;
	return ha<0?ha+24:ha;
}

double RAtoHA_tm(struct tm* ratm, struct tm* uttm, double longtd) 
{
	return RAtoHA(decimal_hours_tm(ratm), UTtoLST_tm(uttm,longtd));
}

void RAtoHA_d2tm(struct tm* hatm, struct tm* uttm, double ra, double longtd)
{
	time_hours(hatm,RAtoHA(ra,UTtoLST_tm(uttm,longtd)));
}

void RAtoHA_d2tm_usec(struct tm* hatm, struct tm* uttm, double *sec, double ra, double longtd)
{
	time_hours_usec(hatm,sec,RAtoHA(ra,UTtoLST_tm(uttm,longtd)));
}
void horizon_coord(double* alt, double* azm, double ha, double decl, double lat)
{
	double had=deg(ha*15);
	double decld=deg(decl);
	double latd=deg(lat);
	double sa=sin(decld)*sin(latd)+cos(decld)*cos(latd)*cos(had);
	double altd=asin(sa);
	double cazmd=(sin(decld)-sin(latd)*sa)/(cos(latd)*cos(altd));
	if(cazmd<-1.0) cazmd=-1.0; if(cazmd>1.0) cazmd=1.0;
	double azmd=acos(cazmd);
	*alt=adeg(altd);
	*azm=adeg(azmd);
	if(sin(had)>0) *azm=360-*azm;
}


void eqt_coord(double *ha, double *decl, double alt, double azm, double lat)
{
	double altd=deg(alt);
	double azmd=deg(azm);
	double latd=deg(lat);
	double sdecl=sin(altd)*sin(latd)+cos(altd)*cos(latd)*cos(azmd);
	double decld=asin(sdecl);
	double chad=(sin(altd)-sin(latd)*sdecl)/(cos(latd)*cos(decld));
	if(chad<-1.0) chad=-1.0; if(chad>1.0) chad=1.0;
	double had=acos(chad);
	*decl=adeg(decld);
//printf("=%.10lf %.10lf %.10lf %.10lf %.10lf=", sin(altd), sin(latd),sdecl,cos(latd),cos(decld));
//printf("=%.15lf=", chad);
	*ha=adeg(had);
	if(sin(azmd)>0) *ha=360-*ha;
	*ha = *ha/15.0;
}

void atime_test()
{
	time_t rtime;
	struct tm *ptm;
	struct tm mtm;
	struct tm atm;
	double dec_h, jd, gst;
	double altd, azm, ha,decl;
	time(&rtime);
	ptm=gmtime(&rtime);
	/*
	printf("%lf\n", dec_h=decimal_hours(18,31,27));
	time_hours(&mtm, dec_h);
	printf("%dh %dm %ds\n", mtm.tm_hour,mtm.tm_min,mtm.tm_sec);
	printf("%lf\n", dec_h=decimal_hours_tm(ptm));
	time_hours(&mtm, dec_h);
	printf("%dh %dm %ds\n", mtm.tm_hour,mtm.tm_min,mtm.tm_sec);
	mtm.tm_year=85; mtm.tm_mon=2; mtm.tm_mday=17;
	printf("%lf\n", jd=julian_day(&mtm));
	calender_day(&mtm, jd);
	printf("%dy %dm %dd\n", mtm.tm_year+1900,mtm.tm_mon,mtm.tm_mday);
	printf("%lf\n", gst=UTtoGST(2444351.5, 14.614353));
	printf("%lf\n", GSTtoLST(-64,gst));
	*/
	printf("jd=%lf\n", jd=julian_day(ptm));
	calender_day(&mtm, jd);
	printf("date=%dy %dm %dd\n", mtm.tm_year+1900,mtm.tm_mon,mtm.tm_mday);
	printf("GST=%lf\n", gst=UTtoGST(jd,decimal_hours_tm(ptm)));
	printf("LST=%lf\n", GSTtoLST(120.982, gst));
	atm.tm_hour=18;atm.tm_min=32;atm.tm_sec=21;
	mtm.tm_year=80; mtm.tm_mon=4; mtm.tm_mday=22;
	mtm.tm_hour=14;mtm.tm_min=36;mtm.tm_sec=51;
	printf("%lf\n", RAtoHA_tm(&atm, &mtm, -64));
	printf("====%lf\n", RAtoHA(18.539167, UTtoLST_tm_usec(&mtm,-64,0.67)));
	atm.tm_hour=23;atm.tm_min=13;atm.tm_sec=10;
	mtm.tm_hour=5;mtm.tm_min=51;mtm.tm_sec=44;
	horizon_coord(&altd,&azm,decimal_hours_tm(&mtm),decimal_hours_tm(&atm),52);
	printf("%lf %lf\n", altd, azm);
	eqt_coord(&ha,&decl,altd,azm,52);
	printf("%lf %lf\n", ha, decl);
	printf("%lf\n", RAtoHA(ha,decimal_hours(0,24,5)));
	printf("jd=%lf\n", julian_day(ptm));
	printf("%.2lf\n", UTtoLST_tm(ptm,121.45));
	ha=RAtoHA(8.08, UTtoLST_tm(ptm,121.45));
	horizon_coord(&altd,&azm,2.85,20.32,25.02);
	printf("@@%lf %lf\n", altd, azm);
	mtm.tm_year=80; mtm.tm_mon=3; mtm.tm_mday=22;
	mtm.tm_hour=14;mtm.tm_min=36;mtm.tm_sec=51;
	ha=RAtoHA(18.539167, UTtoLST_tm(&mtm,-64.0));
	horizon_coord(&altd,&azm,5.862222,23.219444,52);
	printf("@@%lf %lf\n", altd, azm);
}
