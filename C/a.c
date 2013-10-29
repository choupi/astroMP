#include "atime.h"
#include <stdio.h>

int main()
{
	struct tm *ptm;
	time_t rtime;
	double lst,ra,ha,decl=25,alt,azm;
	time(&rtime);
	ptm=gmtime(&rtime);

	atime_test();

/*
	lst=UTtoLST_tm(ptm, 121.1);
	printf("%.4lf\n", lst);
	for(ra=0;ra<24;ra+=0.5) {
		ha=RAtoHA(ra,lst);
		horizon_coord(&alt, &azm, ha, decl, 25.02);
		printf("(%.2lf,%.2lf) %.2lf (%.2lf,%.2lf)\n",ra,decl,ha,alt,azm);
	}
	*/
}
