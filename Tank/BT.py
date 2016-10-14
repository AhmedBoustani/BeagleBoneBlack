#!/usr/bin/python

import Adafruit_BBIO.GPIO as GPIO
import serial
import subprocess

a = serial.Serial("/dev/ttyO1", 115200, timeout=0.5)

a.write("$$$")
on = 'echo 1 > /sys/class/gpio/gpio65/value'
off = 'echo 0 > /sys/class/gpio/gpio65/value'
roll = "./roll"
stop = "./stop_rolling"
while 1:
  b = a.read(100)
  if "light" in b:
    subprocess.call(on, shell='true')
    print "IT'S LIT\n"
  if "off" in b:
    subprocess.call(off, shell='true')
  if "go" in b:
    subprocess.call(roll, shell='true')
  if any(c in b for c in ("stop", "Stop", "shsht")):
    print "stopped rolling\n"
    subprocess.call(stop, shell='true')
  if "exit" in b:
    subprocess.call(stop, shell='true')
    break
print "FREEEDOOOM"
