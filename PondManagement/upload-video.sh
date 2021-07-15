#!/bin/bash
# Prepare and upload video

FILEBASE=gvid$(date +'%Y%m%d-%H%M')
python3 video-capture.py /tmp/${FILEBASE}.h264 || { echo "Video capture failed"; exit 1; }
MP4Box -add /tmp/${FILEBASE}.h264 /tmp/${FILEBASE}.mp4 || { echo "Conversion failed"; rm /tmp/${FILEBASE}.*; exit 1; }

rm /tmp/${FILEBASE}.h264

# Upload
ls -l /tmp/${FILEBASE}.*
