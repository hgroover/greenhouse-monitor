#!/usr/bin/python
# video-capture.py
# Simple 640x480 video capture to specified file from picamera

# Copyright (C) 2019 Henry Groover

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
import sys
import time
import datetime
import calendar
from picamera import PiCamera

if len(sys.argv) < 2:
   print('no video name specified')
   sys.exit(1)

capname = sys.argv[1]
# 10s at 640x480 is around 3mb
# 10s at 1024x768 is around 5.8mb
duration = 10
xres = 640
yres = 480
if len(sys.argv) > 2:
  duration = int(sys.argv[2])
if len(sys.argv) > 4:
  xres = int(sys.argv[3])
  yres = int(sys.argv[4])
print( 'Saving to {0:s} for {1:d}s res {2:d}x{3:d}'.format( capname, duration, xres, yres ) )
camera = PiCamera()
camera.resolution = (xres, yres)
camera.start_recording(capname)
camera.wait_recording(duration)
camera.stop_recording()
# Post-processing with MP4Box is needed to create a playable mp4.
# E.g:
# python3 video-capture.py /tmp/foo.h264
# MP4Box -add /tmp/foo.h264 /tmp/foo.mp4
# rm /tmp/foo.h264
