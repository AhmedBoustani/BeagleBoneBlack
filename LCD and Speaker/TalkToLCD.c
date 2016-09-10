#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LCD.h"
#define MAX 1024


// Converts a string to binary and prints it to the LCD
// Takes the string to be printed and n to determine which line
// in the LCD: 0 for the first line and 1 for the second
void toBinary(char *s, int n);

// Does the 4 classic operations and prints it to the LCD
// Prints the statement in the first line and the result in the second
void Calculate(char *str);


int main (int argc, char** argv) {
  char buff[MAX]; // to store the value scanned from the pipe

  // Clears the screen
  int clrcom[8] = {0,0,0,0,0,0,0,1};
  SetCommand(0,0, clrcom);

  // Reads from the pipe
  FILE *fd = fopen("pipe", "r");
  if (fd == NULL) return EXIT_FAILURE;
  if (fgets(buff, MAX, fd) == NULL) return EXIT_FAILURE;
  fclose(fd);

  toBinary(buff, 0);
  Calculate(buff);
  return 0;
}

void toBinary(char *s, int n) {
  char *c;
  int arr[8], shift[8] = {0,0,0,0,0,1,1,0}, j = 2, newl[8] = {1,0,1,0,1,0,0,0};
  int size = sizeof(s);

  // Checks in which line to print
  if (n == 1) SetCommand(0,0, newl);

  for (c = s; *c; ++c) {
    int i = 0, j = 7;
    // Ignore the '\n' from the pipe
    if (*c == '\n') continue;

    // Use a byte shifter to convert to binary
    while (i < 8) arr[i++] = *c >> j-- & 1;

    // In case of a long text, change to the second line
    if ((c - s) == 16 ) SetCommand(0,0, newl);

    // Shift and print
    SetCommand(0,0,shift);
    SetCommand(1,0,arr);
  }
}

void Calculate (char *str) {
    char o;
    float num1,num2;
    char *c1, *c2, *s, res[10], error[40];
    strcpy(s, str);

    // Parse the string
    c1 = strtok(str, "+-/*");
    o = *(s + (sizeof(c1)/4));
    c2 = s + sizeof(c1)/4 + 1;

    // Convert to from string to int
    num1 = atoi(c1);
    num2 = atoi(c2);

    switch (o) {
        case '+':
            snprintf(res, 10, "= %.1f", num1+num2);
            toBinary(res, 1);
            break;
        case '-':
           snprintf(res, 10, "= %.1f", num1-num2);
            toBinary(res, 1);
            break;
        case '*':
            snprintf(res, 10, "= %.1f", num1*num2);
            toBinary(res, 1);
            break;
        case '/':
            snprintf(res, 10, "= %.1f", num1/num2);
            toBinary(res, 1);
            break;
        default:
            /* If operator is other than +, -, * or /, error message is shown */
            snprintf(error, 40, "%s", "Wrong operator!");
            toBinary(error, 1);
            break;
    }
}

