#!/usr/bin/env python3
import sys
import time
import subprocess

if __name__=='__main__':
  start_time = time.clock_gettime(time.CLOCK_MONOTONIC)
  org_time = start_time
  values = []
  timeout_periods = 0
  while len(values) < 5:
    # Use read-temp-sample.py to get a tuple of temp, humidity
    proc = subprocess.Popen("python3 read-temp-sample.py", stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
    thstr = proc.communicate()[0].decode('utf-8')
    #print('Raw: /{}/'.format(thstr.strip()))
    #sys.stdout.flush()
    tempstr, humstr = thstr.strip().split()
    if not tempstr == "no-temp":
      values.append((float(tempstr), float(humstr)))
      print('Sample[{2}]: {0:0.1f}C  {1:0.1f}%'.format(float(tempstr), float(humstr), len(values)), file=sys.stderr)
      sys.stderr.flush()
      if len(values) >= 5:
        break
    # Wait 10s before next sample
    time.sleep(3)
    cur_time = time.clock_gettime(time.CLOCK_MONOTONIC)
    if cur_time - start_time > 60.0:
        timeout_periods = timeout_periods + 1
        print('Timeout @{0:0.0f}s, {1} samples read'.format(cur_time - org_time,len(values)), file=sys.stderr)
        start_time = cur_time
        sys.stderr.flush()
        if timeout_periods > 30:
          break
  if len(values) == 0:
    print('No values read')
    sys.exit(1)
  # Discard high and low temperatures and take the mean
  low_idx = -1
  high_idx = -1
  min_temp = 999
  max_temp = -999
  if len(values) > 2:
    for n in range(0,len(values)):
      if values[n][0] < min_temp:
        low_idx = n
        min_temp = values[n][0]
      if values[n][0] > max_temp:
        high_idx = n
        max_temp = values[n][0]
  temp_sum = 0
  hum_sum = 0
  pop = 0
  for n in range(0, len(values)):
    if n == low_idx or n == high_idx:
      continue
    temp_sum = temp_sum + values[n][0]
    hum_sum = hum_sum + values[n][1]
    pop = pop + 1
  print('Temperature: {0:0.1f}C Humidity: {1:0.1f}% (N={4}, min={2:0.1f}, max={3:0.1f})'.format(temp_sum / pop, hum_sum / pop, min_temp, max_temp, pop))
