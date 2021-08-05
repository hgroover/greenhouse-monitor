#!/usr/bin/env python3
import serial
import sys
import time
# Comment out the following two lines if DHT11 not connected to pi
import Adafruit_DHT
sensor = Adafruit_DHT.DHT11
# Set to 0 if DHT11 not connected, or GPIO pin
dht11_gpio = 4

fill_timeout = 480
fill_post = 10

if __name__=='__main__':
  ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
  #ser.flush()

  # Start fill via f, wait for FILCOMP then FPH
  # Optionally first send $1=<fto-s> and/or $2=<fpost-s>
  sent = 0
  got_fph = 0
  post_fph_count = 0
  got_temperature = 0
  post_fph_count = 5000
  if dht11_gpio != 0:
     # Worst-case of immediate completion and unresponsive DHT11 will be about 30s
     post_fph_count = 10
  outstr1 = '$1={0}\n'.format(fill_timeout)
  outstr2 = '$2={0}\n'.format(fill_post)
  outstr3 = "f\n"
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
      if sent == 2:
         elapsed = time.clock_gettime(time.CLOCK_MONOTONIC) - start_time
         if elapsed >= 9.0:
           sent = 3
           print('Sending {0:s} at {1:0.3f}'.format(outstr3, elapsed))
           ser.write(outstr3.encode())
      if got_fph:
        post_fph_count = post_fph_count + 1
        if post_fph_count > 5000:
          sys.exit(0)
      if dht11_gpio != 0 and got_temperature == 0:
        # Default retries is 15
        humidity, temperature = Adafruit_DHT.read_retry(sensor, dht11_gpio, retries=2, delay_seconds=2)
        if humidity is not None and temperature is not None:
          got_temperature = 1
          print('Temperature: {0:0.1f}C  Humidity: {1:0.1f}%'.format(temperature, humidity))
