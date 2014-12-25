#!/bin/bash -ex

# FIXME: Rewrite using the Python API so this is reliable.

cd tools

./gst-switch-srv &
sleep 1
./gst-switch-ui &
gst-launch-1.0 videotestsrc pattern=1 is-live=1 \
        ! timeoverlay \
        ! video/x-raw, width=300, height=200 \
        ! gdppay \
        ! tcpclientsink port=3000 \
&

gst-launch-1.0 videotestsrc pattern=18 is-live=1 \
        ! timeoverlay \
        ! video/x-raw, width=300, height=200 \
        ! gdppay \
        ! tcpclientsink port=3000 \
&

gst-launch-1.0 videotestsrc pattern=20 is-live=1 \
        ! timeoverlay \
        ! video/x-raw, width=300, height=200 \
        ! gdppay \
        ! tcpclientsink port=3000 \
&

gst-launch-1.0 videotestsrc pattern=5 is-live=1 \
        ! timeoverlay \
        ! video/x-raw, width=300, height=200 \
        ! gdppay \
        ! tcpclientsink port=3000 \
&

gst-launch-1.0 videotestsrc pattern=12 is-live=1 \
        ! timeoverlay \
        ! video/x-raw, width=300, height=200 \
        ! gdppay \
        ! tcpclientsink port=3000 \
&
