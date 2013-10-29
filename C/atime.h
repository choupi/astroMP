#ifndef ATIME_H
#define ATIME_H

#include <time.h>

double decimal_hours(int hour, int minute, int second);

double decimal_hours_tm(struct tm *ts);

double decimal_hours_tm_usec(struct tm *ts, double usec);

void time_hours(struct tm *ts, double deci);

void time_hours_usec(struct tm *ts, double *sec, double deci);

double julian_day(struct tm *ts);

void calender_day(struct tm *ts, double jd);

double UTtoGST(double jd, double dec_UT);

double UTtoGST_tm(struct tm* uttm);

double UTtoGST_tm_usec(struct tm* uttm, double usec);

double GSTtoLST(double longtd, double gst);

double UTtoLST_tm(struct tm* uttm, double longtd);

double UTtoLST_tm_usec(struct tm* uttm, double longtd, double usec);

double RAtoHA(double ra, double lst);

double RAtoHA_tm(struct tm* ratm, struct tm* uttm, double longtd);

void RAtoHA_d2tm(struct tm* hatm, struct tm* uttm, double ra, double longtd);

void RAtoHA_d2tm_usec(struct tm* hatm, struct tm* uttm, double *sec, double ra, double longtd);

void horizon_coord(double* alt, double* azm, double ha, double decl, double lat);

void eqt_coord(double *ha, double *decl, double alt, double azm, double lat);

void atime_test();

#endif
