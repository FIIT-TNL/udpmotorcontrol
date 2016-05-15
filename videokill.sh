#!/bin/bash

# Kills video local and remote (rpi-slave) streamer 
sudo killall raspivid
ssh pi@192.168.129.2 sudo killall raspivid


