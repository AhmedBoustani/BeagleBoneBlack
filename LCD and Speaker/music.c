#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Does terminal commands in Part2
int openFiles();

// Sets the period
int Period(int n);

// Does terminal commands in Part1
int setup();

// Plays music
void playMeasure(int n);

// helpful function for PlayMeasure that sets the period sent as an argument
int play(int period);

// Shuts off the sound
int silence();

int main (int argc, char **argv) {
  FILE *red, *green, *yellow;
  int arr[3], number = 0;

  setup();

  // converts the number to binary
  while (number < 8) {
    int i = number;
    int j = 0;
    while (i > 0) {
      arr[j++] = i % 2;
      i /= 2;
    }

    // This loop completes the binary number with zeros to form 3 digits
    while (j < 3) arr[j++] = 0;

    /* The following opens the value files and writes the corresponding number 
     * (either 0 or 1) in the gpio for each pin to set up the counting
     */
    red = fopen("/sys/class/gpio/gpio66/value", "w");
      if (red == NULL)  return EXIT_FAILURE;

    green = fopen("/sys/class/gpio/gpio69/value", "w");
      if (green == NULL) return EXIT_FAILURE;

    yellow = fopen("/sys/class/gpio/gpio45/value", "w");
      if (yellow == NULL) return EXIT_FAILURE;

    fprintf(red, "%d", arr[0]);
    fprintf(green, "%d", arr[1]);
    fprintf(yellow, "%d", arr[2]);

    fclose(red);
    fclose(green);
    fclose(yellow);

    // Prepare the setup for the music
    openFiles();
    // Play the music
    playMeasure(number);
    // Pause for 1 second
    sleep(1);
    // loop again when counting reaches 7
    if (++number == 8)  number = 0;

  }

  return 0;
}

int silence() {
  FILE *run = fopen("/sys/devices/ocp.3/pwm_test_P8_19.13/run", "w");
  if (run == NULL) return EXIT_FAILURE;

  fprintf(run, "%d", 0);
  fclose(run);
  return 0;
}

void playMeasure(int n) {
  int b = 2024783, e = 1516875, d = 1702620, a = 2272727;
  float q = 0.20588235, ep = 0.110294117;
  int c = 0;

  switch (n) {
    case 6:
    case 4:
    case 0:
      while (c++ < 5) { play(b); sleep(ep); silence();}
      play(d); usleep(q);
      play(e); usleep(ep);
      play(a); usleep(ep);
      break;
    case 7:
    case 5:
    case 1:
      play(b); usleep(ep);
      play(d); usleep(ep);
      play(e); usleep(ep);
      play(d); usleep(ep);
      play(e); usleep(q);
      play(a); usleep(ep);
      play(b); usleep(ep);
      break;
    case 2:
      play(a); usleep(ep);
      play(e); usleep(ep);
      play(b); usleep(ep);
      play(d); usleep(ep);
      play(e); usleep(q);
      play(d); usleep(ep);
      play(b); usleep(ep);
      break;
    case 3:
      play(d); usleep(ep); silence();
      play(a); usleep(ep); silence();
      play(d); usleep(ep); silence();
      play(b); usleep(ep); silence();
      play(e); usleep(q); silence();
      play(a); usleep(q); silence();
      break;
   }

}

int setup() {
  FILE *sys, *reddir, *greendir, *yellowdir;
  int gpio_pins[3] = {66, 69, 45}, i = 0;


  // echo GPIO_NUMBER > export
  while (i < 3) {
    sys = fopen("/sys/class/gpio/export", "w");
    if (sys == NULL) return EXIT_FAILURE;
    fprintf(sys, "%d", gpio_pins[i]);
    fclose(sys);
    i++;
 }


  // echo out > direction
  reddir = fopen("/sys/class/gpio/gpio66/direction", "w");
  if (reddir == NULL) return EXIT_FAILURE;
  fprintf(reddir, "%s", "out");
  fclose(reddir);

  greendir = fopen("/sys/class/gpio/gpio69/direction", "w");
  if (greendir == NULL) return EXIT_FAILURE;
  fprintf(greendir, "%s", "out");
  fclose(greendir);

  yellowdir = fopen("/sys/class/gpio/gpio45/direction", "w");
  if (greendir == NULL) return EXIT_FAILURE;
  fprintf(yellowdir, "%s", "out");
  fclose(yellowdir);

  return 0;

}

int play(int period) {
  FILE *pwm = fopen("/sys/devices/ocp.3/pwm_test_P8_19.13/period", "w");
  if (pwm == NULL)
    return EXIT_FAILURE;
  fprintf(pwm, "%d", period);
  fclose(pwm);

  FILE *run = fopen("/sys/devices/ocp.3/pwm_test_P8_19.13/run", "w");
  if (run == NULL) return EXIT_FAILURE;
  fprintf(run, "%d", 1);
  fclose(run);
  return 0;
}


int openFiles() {
  int runvalue = 1;

  FILE *slots1, *run;

  slots1 = fopen("/sys/devices/bone_capemgr.9/slots", "w");
  if (slots1 == NULL) return EXIT_FAILURE;

  fprintf(slots1, "%s", "am33xx_pwm");
  fclose(slots1);


  FILE *slots = fopen("/sys/devices/bone_capemgr.9/slots", "w");
  if (slots == NULL) return EXIT_FAILURE;

  fprintf(slots, "%s", "bone_pwm_P9_14");
  fclose(slots);


  FILE *slots2 = fopen("/sys/devices/bone_capemgr.9/slots", "w");
  if (slots2 == NULL) return EXIT_FAILURE;

 run = fopen("/sys/devices/ocp.3/pwm_test_P8_19.13/run", "w");
  if (run == NULL) return EXIT_FAILURE;

  fprintf(run, "%d", runvalue);
  fclose(run);


  FILE *fduty = fopen("/sys/devices/ocp.3/pwm_test_P8_19.13/duty", "w");
  if (fduty == NULL) return EXIT_FAILURE;
  fprintf(fduty, "%d", 50000);
  fclose(fduty);

  return 0;
}

