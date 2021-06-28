#!/usr/bin/env python3
import serial
import sys

if __name__=='__main__':
  ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
  ser.flush()

  # Start fill via f, wait for FILCOMP then FPH
  sent = 0
  got_fph = 0
  post_fph_count = 0
  outstr = "d"
  while True:
    if ser.in_waiting > 0:
      line = ser.readline().decode('utf-8').rstrip()
      if line.startswith("FPH"):
        got_fph = 1
      print(line)
    else:
      if sent == 0:
         sent = 1
         ser.write(outstr.encode())
      if got_fph:
        post_fph_count = post_fph_count + 1
        if post_fph_count > 5000:
          sys.exit(0)

