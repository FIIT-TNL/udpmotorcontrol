#!/bin/bash

#/home/pi/udpmotorcontrol/motorcontrol 17 22 5 13

./kill.sh &> /dev/null 
sleep 1 
./motorcontrol 27 17 22 0 1 670 890 300 1200 #&> /dev/null
