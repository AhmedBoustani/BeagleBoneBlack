#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#define LCD_DIR "/dev/lcd"
#define PIPE "pipe"
#define MAX 32

int main(int argc, char **argv) {
	int fd;
	FILE *fdp;
	char read_buf[MAX], write_buf[MAX];
	fd = open(LCD_DIR, O_RDWR);
	if (fd < 0) {
		printf("File %s cannot be opened\n", LCD_DIR);
		exit(1);
	}
	fdp = fopen(PIPE, "r");
    if (fdp == NULL) {
		printf("Pipe %s cannot be opened\n", PIPE);
		exit(1);
	}
    if (fgets(read_buf, MAX, fdp) == NULL)  {
		printf("Could not read from %s \n", PIPE);
		exit(1);
	}
    fclose(fdp);

	strcpy(write_buf, read_buf);
	write(fd, write_buf, sizeof(write_buf));

	close(fd);
	return 0;
}