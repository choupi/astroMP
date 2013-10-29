CC=arm-linux-gcc

all:
	$(CC) -c atime.c
	$(CC) -c mmp.c
	$(CC) -c gtocp3.c
	$(CC) -lm -lpthread -o gto gto.c mmp.o gtocp3.o atime.o

gg: gg.c
	$(CC) -c atime.c
	$(CC) -c mmp.c
	$(CC) -c gtocp3.c
	$(CC) -lm -lpthread -o gg gg.c mmp.o gtocp3.o atime.o

cmd: gotoRD.c
	$(CC) -lm -o gotoRD gotoRD.c mmp.o gtocp3.o atime.o

update:
	busybox ftpput 192.168.1.6 /opt/test/gto gto

test:
	gcc -lm a.c atime.c
