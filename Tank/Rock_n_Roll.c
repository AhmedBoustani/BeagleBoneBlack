#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#define DIRE "/dev/hbridge"
#define CLOCKID CLOCK_REALTIME
#define SIG SIGUSR1
#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                               } while (0)

int setPWM(char ch, int value);
int command(char *com, int pwm);
void stop();
bool issafe();
static void handler(int sig, siginfo_t *si, void *uc);
int timer();
void forward();
void backward();
void left();
void right();
void motorWrite(char *com);

// global variables
timer_t timerid;
int counter = 1, sum_front = 0, sum_back = 0, 
      avr_front, avr_back;

int main() {
  // Basically, make the tank avoid obsticles
  while (1) {
    forward();
    timer();
    if (!issafe(avr_front)) {
      stop();
      usleep(500000);
      backward();
      usleep(500000);
      left();
      usleep(500000);
      forward();
    }
    if(!issafe(avr_back)){
      stop();
      usleep(500000);
      forward();
      usleep(500000);
      right();
      usleep(500000);
      forward();
    }
  }
  return 0;
}

// Checks if the distance is safe
bool issafe(int sum) {
  if ((sum/2) > 1700) return false;
  return true;
}

/************ CONTROL TANK'S MOVEMENT FUNCTIONS ****************/

// turn left
void left(){
  command("A101", 1);
  command("B011", 1);
}
// turn right
void right(){
  command("A011", 1);
  command("B101", 1);
}
// go backward
void backward(){
  command("A011", 1);
  command("B011", 1);
}
// go forward
void forward(){
  command("A101", 1);
  command("B101", 1);
}
// Stops the wheels
void stop() {
  command("A001", 1);
  command("B001", 1);
}

/************* WRITE TO THE MOTORS FUNCTIONS *********************/

int command(char *com, int pwm) {
  motorWrite(com);
  setPWM(com[0], pwm);
  return 0;
}

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

/**************** TIMER INTERRUPT FUNCTIONS *****************/

// Timer interrupt handler: after 300 executions, it ignores the signal
// Reads from the APC and gives the average dynamically
static void handler(int sig, siginfo_t *si, void *uc) {
  int data;
  FILE *fd = fopen("/sys/devices/ocp.3/helper.15/AIN0", "r");
  if (fd == NULL) exit(1);
  fscanf(fd, "%d", &data);
  fclose(fd);
  sum_back += data;
  fd = fopen("/sys/devices/ocp.3/helper.15/AIN2", "r");
  if (fd == NULL) exit(1);
  fscanf(fd, "%d", &data);
  fclose(fd);
  sum_front += data;
  avr_front = sum_front / counter;
  avr_back = sum_back / counter;
  FILE *fdlog = fopen("/root/distance.txt", "a");
  fprintf(fdlog, "Averages:\t\tFront distance: %d\tBack distance: %d\n", 
           avr_front, avr_back);
  fclose(fdlog);
//  printf("Averages:\t\tFront distance: %d\tBack distance: %d\n",
//           avr_front, avr_back);
  if(counter++ == 300) signal(sig, SIG_IGN);
}

// Timer interrupt
int timer() {
  timer_t timerid;
  struct sigevent sev;
  struct itimerspec its;
  long long freq_nanosecs;
  sigset_t mask;
  struct sigaction sa;

  /* Establish handler for timer signal */
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = handler;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIG, &sa, NULL) == -1)
  errExit("sigaction");

  /* Block timer signal temporarily */
  sigemptyset(&mask);
  sigaddset(&mask, SIG);
  if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
  errExit("sigprocmask");

  /* Create the timer */
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIG;
  sev.sigev_value.sival_ptr = &timerid;
  int timer_value = timer_create(CLOCKID, &sev, &timerid);
  if (timer_value == -1)
    errExit("timer_create");

  /* Start the timer */
  freq_nanosecs = 300;
  its.it_value.tv_sec = freq_nanosecs / 1000000000;
  its.it_value.tv_nsec = freq_nanosecs % 1000000000;
  its.it_interval.tv_sec = its.it_value.tv_sec;
  its.it_interval.tv_nsec = its.it_value.tv_nsec;

  if (timer_settime(timerid, 0, &its, NULL) == -1)
  errExit("timer_settime");

  /* Unlock the timer signal, so that timer notification can be delivered */
  if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
    errExit("sigprocmask");

  return EXIT_SUCCESS;
}
