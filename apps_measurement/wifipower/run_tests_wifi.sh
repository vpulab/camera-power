#!/bin/bash

clear
#echo "Deleting files in ./dat && ./logs..."
#rm -rf dat/* && rm -rf logs/*

IP="150.244.59.43"
#IP='127.0.0.1'
PORT=1030

WAIT=1
TIME=1 #sampling time (in seconds)
SAMP=20 #number of samples to take
#WAIT=5
#TIME=1 #sampling time (in seconds)
#SAMP=10 #number of samples to take
MAXBITRATE=8192
PKTSIZE=50000

START=1 #repeat measurements (start index)
REPEAT=$((START+5)) #repeat measurements

echo "Each test will wait $WAIT secs to get data ($SAMP samples at $TIME secs/sample)."
#read -p 'Reset the measurement device and press any key to continue... ' -n1 -s

for r in $(seq $START 1 $REPEAT)
do

#	echo -n "#$r running test calibration (disable usb)..."
#	START_TIME=$SECONDS
#	./wifipower -ip $IP -port $PORT -c -i ra0 -t $TIME -n $SAMP -iw $WAIT -dusb -o dat/COM_calib_r${r}_usboff.dat
#	echo "$(($(($SECONDS - $START_TIME))/60)) min $(($(($SECONDS - $START_TIME))%60)) sec"

#	echo -n "#$r running test calibration (active usb)..."
#	START_TIME=$SECONDS
#	./wifipower -ip $IP -port $PORT -c -i ra0 -t $TIME -n $SAMP -iw $WAIT -o dat/COM_calib_r${r}_usbon.dat
#	echo "$(($(($SECONDS - $START_TIME))/60)) min $(($(($SECONDS - $START_TIME))%60)) sec"  

	echo -n "#$r running test RX (ra0 interface)..."
	START_TIME=$SECONDS
	./wifipower -ip "150.244.59.42" -port $PORT -rx -i ra0 -t $TIME -n $SAMP -iw $WAIT -d $PKTSIZE -max $MAXBITRATE -o dat/COM_RX_r${r}.dat
	echo "$(($(($SECONDS - $START_TIME))/60)) min $(($(($SECONDS - $START_TIME))%60)) sec"

	echo -n "#$r running test TX (ra0 interface)..."
	START_TIME=$SECONDS
	./wifipower -ip "150.244.59.42" -port $PORT -tx -i ra0 -t $TIME -n $SAMP -iw $WAIT -d $PKTSIZE -max $MAXBITRATE -o dat/COM_RX_r${r}.dat
	echo "$(($(($SECONDS - $START_TIME))/60)) min $(($(($SECONDS - $START_TIME))%60)) sec"

		
done


