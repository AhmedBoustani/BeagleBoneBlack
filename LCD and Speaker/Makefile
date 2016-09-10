CC= gcc
CFLAGS= -g -Wall

default: TalkToLCD InitLCD


TalkToLCD: LCD.o TalkToLCD.o
	$(CC) $(CFLAGS) -o TalkToLCD LCD.o TalkToLCD.o

InitLCD: LCD.o InitLCD.c
	$(CC) $(CFLAGS) -o InitLCD LCD.o InitLCD.c

TalkToLCD.o: LCD.h TalkToLCD.c
	$(CC) $(CFLSGS) -c TalkToLCD.c

LCD.o: LCD.c LCD.h
	$(CC) $(CFLAGS) -c LCD.c

clean: FORCE
	/bin/rm -f *.o *.a *~ TalkToLCD InitLCD

FORCE:

