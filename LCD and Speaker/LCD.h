#ifndef LCD_H
#define LCD_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

// Initializes the LCD
bool Initialize ();

// Creates GPIOs
int setupGPIOs();

// Sets the commands, takes s and w to represent 
// whether to print data or execute a command
int SetCommand(int s, int w, int *com);

// Sets direction of GPIOs to out
int setupDirection();

// Calls setupGPIOs, setupDirection, and Initialize and prints the status
bool init();

#endif

