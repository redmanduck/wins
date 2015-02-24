#!/bin/bash
#
sudo iw wlp2s0 scan | egrep '(^BSS)|freq|signal' >  allchannels.min
python findoptimal.py allchannels.min
