#include <stdio.h>
#include <stdlib.h>

// Setup Motor() gpios in the usermode 
int motor();
// Setup the PWM to receive data from sensors
int setupPWM();
// This program sets up the ADC

int main(void) {
  char path[35] = "/sys/devices/bone_capemgr.9/slots";
  printf("Setting up ADC...");
  FILE *fd = fopen(path, "w");
  if (fd == NULL) {
    fprintf(stderr, "Could not open file: %s\n", path);
    return EXIT_FAILURE;
  }
  fprintf(fd, "%s", "cape-bone-iio");
  fclose (fd);
  if (motor() == 0)
    printf("done\n");
  else printf ("Failed\n");

  setupPWM();
  return EXIT_SUCCESS;
}

// Configure PWM pins
int setupPWM() {
  char path[35] = "/sys/devices/bone_capemgr.9/slots";
  FILE *fd = fopen(path, "w");
  if (fd == NULL) {
    fprintf(stderr, "Could not open file: %s\n", path);
    return EXIT_FAILURE;
  }
  fprintf(fd, "%s", "am33xx_pwm");
  fclose(fd);

  // ChannelB : EHRPWM2A
  fd = fopen(path, "w");
  if (fd == NULL) {
    fprintf(stderr, "Could not open file: %s\n", path);
    return EXIT_FAILURE;
  }
  fprintf(fd, "%s", "bone_pwm_P8_19");
  fclose (fd);

  // ChannelA : EHRPWM1A
  fd = fopen(path, "w");
  if (fd == NULL) {
    fprintf(stderr, "Could not open file: %s\n", path);
    return EXIT_FAILURE;
  }
  fprintf(fd, "%s", "bone_pwm_P9_14");
  fclose (fd);
  return 0;
}

int motor (){
  int gpio_pins[5] = {68,67,44,26,46};
  FILE *sys, *ain1, *ain2, *bin1, *bin2, *stdby;
  int i = 0;

  while (i < 5) {
    sys = fopen("/sys/class/gpio/export", "w");
    if (sys == NULL) return EXIT_FAILURE;
    fprintf(sys, "%d", gpio_pins[i++]);
    fclose(sys);
 }

  ain1 = fopen("/sys/class/gpio/gpio68/direction", "w");
  if (ain1 == NULL) return EXIT_FAILURE;
  fprintf(ain1, "%s", "out");
  fclose(ain1);

  ain2 = fopen("/sys/class/gpio/gpio67/direction", "w");
  if (ain2 == NULL) return EXIT_FAILURE;
  fprintf(ain2, "%s", "out");
  fclose(ain2);

  bin1 = fopen("/sys/class/gpio/gpio26/direction", "w");
  if (bin1 == NULL) return EXIT_FAILURE;
  fprintf(bin1, "%s", "out");
  fclose(bin1);

  bin2 = fopen("/sys/class/gpio/gpio46/direction", "w");
  if (bin2 == NULL) return EXIT_FAILURE;
  fprintf(bin2, "%s", "out");
  fclose(bin2);

  stdby = fopen("/sys/class/gpio/gpio44/direction", "w");
  if (stdby == NULL) return EXIT_FAILURE;
  fprintf(stdby, "%s", "out");
  fclose(stdby);

  return 0;
}
