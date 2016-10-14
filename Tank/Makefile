CC = gcc
CFLAGS= -Wall -g

default: roll setup stop_rolling

roll: Rock_n_Roll.c
	$(CC) $(CFLAGS) -o roll Rock_n_Roll.c -lrt

setup: ADC_Setup.c
	$(CC) $(CFLAGS) -o setup ADC_Setup.c

stop_rolling: Stop_Rolling.c
	$(CC) $(CFLAGS) -o stop_rolling Stop_Rolling.c

clean: FORCE
	/bin/rm -f *.o *.a *~ setup roll Police_Car stop_rolling

FORCE:
