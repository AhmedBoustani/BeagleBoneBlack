/* 
 * new_char.c: holds a buffer of 100 characters as device file.
 *             prints out contents of buffer on read.
 *             writes over buffer values on write.
 */
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <stdbool.h>
#include "lcd.h"

/********************* INITIALIZATION FUNCTIONS ***************/

int gpios[7] = {115,49,117,20,66,69,45};

// Creates GPIOs and sets direction
bool setupGPIOs(void) {
   char label[7] = "BBGPIO";
   int i = 0, ret;
   for (; i < 7; i++) {
     ret = gpio_request(gpios[i], label);
     if (ret != 0) {
       printk(KERN_ALERT "setupGPIOs: Failed to allocate for GPIO %d\n", gpios[i]);
       return false;
     }
     ret = gpio_direction_output(gpios[i], 0);
     if (ret != 0) {
       printk(KERN_ALERT "setupGPIOs: Failed to set direction for GPIO %d\n", gpios[i]);
       return false;
     }
   }
   return true;
}

void Initialize(void) {
  int com1[8] = {0,0,1,1,1,1,0,0};
  msleep(20);
  SetCommand(0,0, com1);
  msleep(10);
  ClearScreen();
}

void init(void) {
  if (setupGPIOs())
    printk(KERN_INFO "...GPIOs are set\n");
  else return;
  // Start Initialization
  printk(KERN_INFO "...Initializing...\n");
  Initialize();
  printk(KERN_INFO "......done\n");
}

/********************* LCD OPERATION FUNCTIONS ***************/
void ClearScreen(void) {
  int clrcom[8] = {0,0,0,0,0,0,0,1}, dispcom[8] = {0,0,0,0,1,1,1,1};
  SetCommand(0,0, clrcom);
  SetCommand(0,0, dispcom);
}

// Sets the commands, takes s and w to represent 
// whether to print data or execute a command
void SetCommand(int s, int w, int *com) {
  int i = 7;
  gpio_set_value(20, 1); // Clear shift register
  gpio_set_value(45, 0); // set lcd latch to 0
  gpio_set_value(117, 1); // set shift register latch to 1
  gpio_set_value(66, s); // set R/S value
  gpio_set_value(69, w); // set R/W value

  while (i >= 0) {
    gpio_set_value(49, com[i--]);
    gpio_set_value(115, 1);
    msleep(1);
    gpio_set_value(115, 0);
  }
  gpio_set_value(45, 1);
  msleep(1);
  gpio_set_value(45, 0);
  gpio_set_value(20, 1);
}

void toBinary(char *s, int n) {
  char *c;
  int arr[8], shift[8] = {0,0,0,0,0,1,1,0}, newl[8] = {1,0,1,0,1,0,0,0};

  ClearScreen();

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

/********************* FILE OPERATION FUNCTIONS ***************/

// runs on startup
// intializes module space and declares major number.
// assigns device structure data.
static int __init driver_entry(void) {
	// REGISTERIONG OUR DEVICE WITH THE SYSTEM
	// (1) ALLOCATE DINAMICALLY TO ASSIGN OUR DEVICE
	int ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if (ret < 0) {
		printk(KERN_ALERT "lcd: Failed to allocate a major number\n");
		return ret;
	}
	printk(KERN_INFO "lcd: major number is %d\n", MAJOR(dev_num));
	printk(KERN_INFO "Use mknod /dev/%s c %d 0 for device file\n", DEVICE_NAME, MAJOR(dev_num));

	// (2) CREATE CDEV STRUCTURE, INITIALIZING CDEV
	mcdev = cdev_alloc();
	mcdev->ops = &fops;
	mcdev->owner = THIS_MODULE;

	// After creating cdev, add it to kernel
	ret = cdev_add(mcdev, dev_num, 1);
	if (ret < 0) {
		printk(KERN_ALERT "lcd: unable to add cdev to kernel\n");
		return ret;
	}
	// Initialize SEMAPHORE
	sema_init(&virtual_device.sem, 1);
    init();
	return 0;
}

// called up exit.
// unregisters the device and all associated gpios with it.
static void __exit driver_exit(void) {
	cdev_del(mcdev);
	unregister_chrdev_region(dev_num, 1);
	ClearScreen();
	printk(KERN_ALERT "lcd: successfully unloaded\n");
}

// Called on device file open
//	inode reference to file on disk, struct file represents an abstract
// checks to see if file is already open (semaphore is in use)
// prints error message if device is busy.
int device_open(struct inode *inode, struct file* filp) {
	if (down_interruptible(&virtual_device.sem) != 0) {
		printk(KERN_ALERT "lcd: could not lock device during open\n");
		return -1;
	}
	printk(KERN_INFO "lcd: device opened\n");
	return 0;
}

// Called upon close
// closes device and returns access to semaphore.
int device_close(struct inode* inode, struct  file *filp) {
	up(&virtual_device.sem);
	printk(KERN_INFO "lcd, closing device\n");
	return 0;	
}

// Called when user wants to get info from device file
ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, loff_t* curOffset) {
	printk(KERN_INFO "lcd: Reading from device...\n");
	return copy_to_user(bufStoreData, virtual_device.data, bufCount);
}

// Called when user wants to send info to device
ssize_t device_write(struct file* filp, const char* bufSource, size_t bufCount, loff_t* curOffset) {
	ssize_t ret;
	printk(KERN_INFO "lcd: writing to device...\n");
	ret = copy_from_user(virtual_device.data, bufSource, bufCount);
	toBinary(virtual_device.data, 0);
	if (strcmp(virtual_device.data, "quit\n") == 0){
		toBinary("Bye Bye!", 0);
		msleep(2000);
		ClearScreen();
	}
	return ret;
}


MODULE_LICENSE("GPL"); // module license: required to use some functionalities.
module_init(driver_entry); // declares which function runs on init.
module_exit(driver_exit);  // declares which function runs on exit.
