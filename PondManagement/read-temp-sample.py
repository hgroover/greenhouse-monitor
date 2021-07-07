#!/usr/bin/env python3
import sys
import time
# Comment out the following two lines if DHT11 not connected to pi
import Adafruit_DHT
#import Raspberry_Pi_2
sensor = Adafruit_DHT.DHT11
# Set to 0 if DHT11 not connected, or GPIO pin
dht11_gpio = 4

if __name__=='__main__':
  got_temperature = 0
  start_time = time.clock_gettime(time.CLOCK_MONOTONIC)
  org_time = start_time
  timeout_periods = 0
  while got_temperature == 0 and dht11_gpio != 0:
        # Default retries is 15
        #humidity, temperature = Adafruit_DHT.read_retry(sensor, dht11_gpio, retries=18, delay_seconds=4)
        humidity, temperature = Adafruit_DHT.read(sensor, dht11_gpio)
        if humidity is not None and temperature is not None:
          got_temperature = 1
          print('{0:0.1f}  {1:0.1f}'.format(temperature, humidity), file=sys.stdout)
          sys.stdout.flush()
          break
        # Wait before next attempt
        time.sleep(3)
        cur_time = time.clock_gettime(time.CLOCK_MONOTONIC)
        if cur_time - start_time > 60.0:
          print('no-temp  no-hum', file=sys.stdout)
          sys.stdout.flush()
          break
  if got_temperature == 0:
        sys.exit(-1)
  # Got sample
