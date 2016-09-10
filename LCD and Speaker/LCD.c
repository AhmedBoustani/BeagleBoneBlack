#include "LCD.h"

bool init () {
  // set up the GPIOs
  setupGPIOs();
  setupDirection();
  printf("...GPIOs are set\n");

  // Start Initialization
  printf("...Initializing...\n");
  if (Initialize()) {
    printf("......done\n");
    return true;
  } else {
    fprintf(stderr, "......INITIALIZATION FAILED!\n");
    return false;
  }
}

bool Initialize () {
  int com1[8] = {0,0,1,1,1,1,0,0}, clrcom[8] = {0,0,0,0,0,0,0,1},
      dispcom[8] = {0,0,0,0,1,1,1,1};
  usleep(20000);
  int setcom1 = SetCommand(0,0, com1);
  usleep(10000);

  int setclr = SetCommand(0,0, clrcom);

  int setdisp = SetCommand(0,0, dispcom);

  if (setcom1 != 0 || setclr != 0 || setdisp != 0) return false;

  return true;
}

int setupGPIOs () {
  FILE *sys;
  int gpios[11] = {65,49,66,69,45,47,27,67,68,44, 117}, i = 0;
  for (; i < 11; i++){
    sys = fopen("/sys/class/gpio/export", "w");
    if (sys == NULL) return EXIT_FAILURE;
    fprintf(sys, "%d", gpios[i]);
    fclose(sys);
  }
  
  return 0;
}

int setupDirection () {
  FILE *rs, *rw, *db7, *db6, *db5, *db4, *db3, *db2, *db1, *db0, *e;

  rs = fopen("/sys/class/gpio/gpio65/direction", "w");
  if (rs == NULL) return EXIT_FAILURE;
  fprintf(rs, "%s", "out");
  fclose(rs);

  rw = fopen("/sys/class/gpio/gpio49/direction", "w");
  if (rw == NULL) return EXIT_FAILURE;
  fprintf(rw, "%s", "out");
  fclose(rw);

  db7 = fopen("/sys/class/gpio/gpio66/direction", "w");
  if (db7 == NULL) return EXIT_FAILURE;
  fprintf(db7, "%s", "out");
  fclose(db7);

  db6 = fopen("/sys/class/gpio/gpio69/direction", "w");
  if (db6 == NULL) return EXIT_FAILURE;
  fprintf(db6, "%s", "out");
  fclose(db6);

  db5 = fopen("/sys/class/gpio/gpio45/direction", "w");
  if (db5 == NULL) return EXIT_FAILURE;
  fprintf(db5, "%s", "out");
  fclose(db5);

  db4 = fopen("/sys/class/gpio/gpio47/direction", "w");
  if (db4 == NULL) return EXIT_FAILURE;
  fprintf(db4, "%s", "out");
  fclose(db4);

  db3 = fopen("/sys/class/gpio/gpio27/direction", "w");
  if (db3 == NULL) return EXIT_FAILURE;
  fprintf(db3, "%s", "out");
  fclose(db3);

  db2 = fopen("/sys/class/gpio/gpio67/direction", "w");
  if (db2 == NULL) return EXIT_FAILURE;
  fprintf(db2, "%s", "out");
  fclose(db2);

  db1 = fopen("/sys/class/gpio/gpio68/direction", "w");
  if (db1 == NULL) return EXIT_FAILURE;
  fprintf(db1, "%s", "out");
  fclose(db1);

  db0 = fopen("/sys/class/gpio/gpio44/direction", "w");
  if (db0 == NULL) return EXIT_FAILURE;
  fprintf(db0, "%s", "out");
  fclose(db0);

  e = fopen("/sys/class/gpio/gpio117/direction", "w");
  if (e == NULL) return EXIT_FAILURE;
  fprintf(e, "%s", "out");
  fclose(e);

  return 0;
}

int SetCommand (int s, int w, int *com) {
  FILE *rs, *rw, *db7, *db6, *db5, *db4, *db3, *db2, *db1, *db0, *e;
  int i = 0;
  e = fopen("/sys/class/gpio/gpio117/value", "w");
  if (e == NULL) return false;
  fprintf(e, "%d", 0);
  fclose(e);

  rs = fopen("/sys/class/gpio/gpio65/value", "w");
  if (rs == NULL) return EXIT_FAILURE;
  fprintf(rs, "%d", s);
  fclose(rs);

  rw = fopen("/sys/class/gpio/gpio49/value", "w");
  if (rw == NULL) return EXIT_FAILURE;
  fprintf(rw, "%d", w);
  fclose(rw);

  db7 = fopen("/sys/class/gpio/gpio66/value", "w");
  if (db7 == NULL) return EXIT_FAILURE;
  fprintf(db7, "%d", com[i++]);
  fclose(db7);

  db6 = fopen("/sys/class/gpio/gpio69/value", "w");
  if (db6 == NULL) return EXIT_FAILURE;
  fprintf(db6, "%d", com[i++]);
  fclose(db6);

  db5 = fopen("/sys/class/gpio/gpio45/value", "w");
  if (db5 == NULL) return EXIT_FAILURE;
  fprintf(db5, "%d", com[i++]);
  fclose(db5);

  db4 = fopen("/sys/class/gpio/gpio47/value", "w");
  if (db4 == NULL) return EXIT_FAILURE;
  fprintf(db4, "%d", com[i++]);
  fclose(db4);

  db3 = fopen("/sys/class/gpio/gpio27/value", "w");
  if (db3 == NULL) return EXIT_FAILURE;
  fprintf(db3, "%d", com[i++]);
  fclose(db3);

  db2 = fopen("/sys/class/gpio/gpio67/value", "w");
  if (db2 == NULL) return EXIT_FAILURE;
  fprintf(db2, "%d", com[i++]);
  fclose(db2);

  db1 = fopen("/sys/class/gpio/gpio68/value", "w");
  if (db1 == NULL) return EXIT_FAILURE;
  fprintf(db1, "%d", com[i++]);
  fclose(db1);

  db0 = fopen("/sys/class/gpio/gpio44/value", "w");
  if (db0 == NULL) return EXIT_FAILURE;
  fprintf(db0, "%d", com[i]);
  fclose(db0);

  // Change the latch to execute the command
  e = fopen("/sys/class/gpio/gpio117/value", "w");
  if (e == NULL) return EXIT_FAILURE;
  fprintf(e, "%d", 1);
  fclose(e);

  // Wait fo the command to be executed
  usleep(10000);

  // Change back the latch
  e = fopen("/sys/class/gpio/gpio117/value", "w");
  if (e == NULL) return EXIT_FAILURE;
  fprintf(e, "%d", 0);
  fclose(e);
  usleep(10000);
  return 0;
}

