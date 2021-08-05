#!/usr/bin/env python3
import serial
import sys
import time

drain_timeout = 480
drain_post = 10

if __name__=='__main__':
  ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
  ser.flush()

  # Start drain via d, wait for DRNCOMP then FPH
  # Optionally send setup $3=<dto-s> and $4=<dpost-s>
  if len(sys.argv) >= 2:
    drain_timeout = int(sys.argv[1])
    if len(sys.argv) >= 3:
      drain_post = int(sys.argv[2])

  sent = 0
  got_fph = 0
  post_fph_count = 0
  outstr1 = '$3={0}\n'.format(drain_timeout)
  outstr2 = '$4={0}\n'.format(drain_post)
  outstr3 = "d\n"

  start_time = time.clock_gettime(time.CLOCK_MONOTONIC)
  while True:
    if ser.in_waiting > 0:
      line = ser.readline().decode('utf-8').rstrip()
      if line.startswith("FPH"):
        got_fph = 1
      print(line)
      sys.stdout.flush()
    else:
      if sent == 0:
         elapsed = time.clock_gettime(time.CLOCK_MONOTONIC) - start_time
         if elapsed >= 3.0:
           sent = 1
           print('Sending {0:s} at {1:0.3f}'.format(outstr1, elapsed)) 
           sys.stdout.flush()
           ser.write(outstr1.encode())
      if sent == 1:
         elapsed = time.clock_gettime(time.CLOCK_MONOTONIC) - start_time
         if elapsed >= 6.0:
           sent = 2
           print('Sending {0:s} at {1:0.3f}'.format(outstr2, elapsed))
           sys.stdout.flush()
           ser.write(outstr2.encode())
      if sent == 2:
         elapsed = time.clock_gettime(time.CLOCK_MONOTONIC) - start_time
         if elapsed >= 9.0:
           sent = 3
           print('Sending {0:s} at {1:0.3f}'.format(outstr3, elapsed))
           sys.stdout.flush()
           ser.write(outstr3.encode())
      if got_fph:
        post_fph_count = post_fph_count + 1
        if post_fph_count > 5000:
          sys.exit(0)

