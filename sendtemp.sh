#!/bin/bash
while :;
do
	TEMP=`cat /sys/bus/w1/devices/28-0000060a613e/w1_slave`
	while echo $TEMP | grep -q NO;
	do
		# Re-read in case of bad checksum
		TEMP=`cat /sys/bus/w1/devices/28-0000060a613e/w1_slave`
	done
	echo -n $TEMP | sed 's/^.*t=\(..\)\(.*\)$/\x04\1.\2°C/' > /dev/udp/$1/$2
	sleep 1
done
