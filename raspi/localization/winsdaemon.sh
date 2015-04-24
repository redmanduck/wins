#!/bin/bash

while true
do
if [ ! `pgrep -f binary` ] && [ ! -f /home/pi/wins/raspi/localization/DEBUG ]; then
sudo /home/pi/wins/raspi/localization/binary test full /home/pi/wins/raspi/localization/data/ee_ab_map_8lin.dat
fi
sleep 10
done
