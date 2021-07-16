#!/usr/bin/env python3
import serial
import sys
import time

feed_time = 2

if __name__=='__main__':
  ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
  ser.flush()

  # Start feed via 1 after setup $5=2000, then wait for FPH
  if len(sys.argv) >= 2:
    feed_time = int(sys.argv[1])
  sent = 0
  got_fph = 0
  post_fph_count = 0
  outstr1 = '$5={0}\n'.format(feed_time * 1000)
  outstr2 = "1\n"
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
           print('Sending {0:s} at {1:0.3f}'.format(outstr1, elapsed)) 
           ser.write(outstr1.encode())
      if sent == 1:
         elapsed = time.clock_gettime(time.CLOCK_MONOTONIC) - start_time
         if elapsed >= 6.0:
           sent = 2
           print('Sending {0:s} at {1:0.3f}'.format(outstr2, elapsed))
           ser.write(outstr2.encode())
      if got_fph:
        post_fph_count = post_fph_count + 1
        if post_fph_count > 5000:
          sys.exit(0)

