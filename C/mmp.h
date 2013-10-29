#ifndef MMP_H
#define MMP_H

#define PROMODE_TRAP 0
#define PROMODE_VCONT 1
#define PROMODE_SCURV 2
#define PROMODE_EGEAR 3

int mmp_fd;

void MMPSetProfileMode(int asix, int mode);
void MMPGetProfileMode(int asix, int* mode);

void MMPSetPosition(int asix, int pos);
void MMPGetPosition(int asix, int* pos);

void MMPSetVelocity(int asix, int velc);
void MMPGetVelocity(int asix, int* velc);

void MMPSetAcceleration(int asix, int acc);
void MMPGetAcceleration(int asix, int* acc);

void MMPSetDeceleration(int asix, int dece);
void MMPGetDeceleration(int asix, int* dece);

void MMPUpdate(int asix);
void MMPUpdateAll();

void MMPGetTime(int* cycle);
void MMPGetSampleTime(int* msec);

void MMPSetActualPosition(int asix, int pos);
void MMPGetActualPosition(int asix, int* pos);

void MMPReset();

#endif

