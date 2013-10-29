#ifndef GTOCP3_H
#define GTOCP3_H

#include <time.h>

#define GTO_ALT (1)
#define GTO_AZM (0)

#define GTO_LAT (0)
#define GTO_LONGTD (1)

#define GTO_RA (0)
#define GTO_DECL (1)

struct gtocp3_mount {
	double asix[3];
	double dst[3];
	double rate[3];
	double p0[3];
	double loc[3];
	double maxspd;
	int RDrun;
};

struct gtocp3_mount gto_m;

void GTOCP3_init_f(char *fname, int rs);
void GTOCP3_init(double lat, double longtd, double r0, double r1);
void GTOCP3_read(int fd);

void gto_goto(double ra, double decl);
void gto_goto_aa(double alt, double azm);
void gto_track();
void gto_getpos(int *pos0, int *pos1);
void gto_update();
void gto_maxspd();
void gto_sync();

#endif
