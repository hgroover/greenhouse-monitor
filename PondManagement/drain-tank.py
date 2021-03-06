#!/usr/bin/env python3
import serial
import sys
import time

if __name__=='__main__':
  ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
  ser.flush()

  # Start drain via d, wait for DRNCOMP then FPH
  # Optionally send setup $3=<dto-s> and $4=<dpost-s>
  sent = 0
  got_fph = 0
  post_fph_count = 0
  outstr = "d\n"
  start_time = time.clock_gettime(time.CLOCK_MONOTONIC)
  while True:
    if ser.in_waiting > 0:
      line = ser.readline().decode('utf-8').rstrip()
      if line.startswith("FPH"):
        got_fph = 1
      print(line)
    else:
      if sent == 0:
         elapsed = time.clock_gettime(time.CLOCK_MONOTONIC) - start_time
         if elapsed >= 3.0:
           sent = 1
           print('Sending {0:s} at {1:0.3f}'.format(outstr, elapsed)) 
           ser.write(outstr.encode())
      if got_fph:
        post_fph_count = post_fph_count + 1
        if post_fph_count > 5000:
          sys.exit(0)

