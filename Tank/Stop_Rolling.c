#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>

// Enables or disables the PWM
int setPWM(char ch, int value) {
    if (ch == 'A') {
    FILE *fd = fopen("/sys/devices/ocp.3/pwm_test_P9_14.17/run", "w");
    if (fd == NULL) return EXIT_FAILURE;
    fprintf(fd, "%d", value);
    fclose (fd);
  }
  else if (ch == 'B') {
    FILE *fd = fopen("/sys/devices/ocp.3/pwm_test_P8_19.16/run", "w");
    if (fd == NULL) return EXIT_FAILURE;
    fprintf(fd, "%d", value);
    fclose (fd);
  }
  return 0;
}

void ChannelA(int in1, int in2, int stdby) {
  FILE *ain1, *ain2, *fstdby;

  ain1 = fopen("/sys/class/gpio/gpio68/value", "w");
    if (ain1 == NULL)  return;

  ain2 = fopen("/sys/class/gpio/gpio67/value", "w");
    if (ain2 == NULL) return;

  fstdby = fopen("/sys/class/gpio/gpio44/value", "w");
    if (fstdby == NULL) return;

  fprintf(ain1, "%d", in1);
  fprintf(ain2, "%d", in2);
  fprintf(fstdby, "%d", stdby);

  fclose(ain1);
  fclose(ain2);
  fclose(fstdby);

}

void ChannelB(int in1, int in2, int stdby) {
  FILE *fbin1, *fbin2, *fstdby;

  fbin1 = fopen("/sys/class/gpio/gpio26/value", "w");
    if (fbin1 == NULL)  return;

  fbin2 = fopen("/sys/class/gpio/gpio46/value", "w");
    if (fbin2 == NULL) return;

  fstdby = fopen("/sys/class/gpio/gpio44/value", "w");
    if (fstdby == NULL) return;

  fprintf(fbin1, "%d", in1);
  fprintf(fbin2, "%d", in2);
  fprintf(fstdby, "%d", stdby);

  fclose(fbin1);
  fclose(fbin2);
  fclose(fstdby);
}

void motorWrite(char *indata){
  char ch = indata[0];
  int in1 = indata[1] - '0', in2 = indata[2] - '0', stdby = indata[3] - '0';
  if (ch == 'A') ChannelA(in1, in2, stdby);
  else if (ch == 'B') ChannelB(in1, in2, stdby);
  else printf("...WRONG INPUT CHANNEL!!... %c \n", indata[0]);
}

int command(char *com, int pwm) {
  motorWrite(com);
  setPWM(com[0], pwm);
  return 0;
}

int main(){
  command("A001", 1);
  command("B001", 1);
  return 0;
}

