#!/bin/bash

clear
#echo "Deleting files in ./dat && ./logs..."
#rm -rf dat/* && rm -rf logs/*

IP="150.244.59.43"
#IP='127.0.0.1'
PORT=7777

#WAIT=1
#TIME=1 #sampling time (in seconds)
#SAMP=20 #number of samples to take

WAIT=50
TIME=5 #sampling time (in seconds)
SAMP=100 #number of samples to take
#SAMP=100 #number of samples to take

MAXBITRATE=8192
PKTSIZE=50000

START=1 #repeat measurements (start index)
REPEAT=$((START+5)) #repeat measurements

echo "Each test will wait $WAIT secs to get data ($SAMP samples at $TIME secs/sample)."
#read -p 'Reset the measurement device and press any key to continue... ' -n1 -s

for r in $(seq $START 1 $REPEAT)
do
	echo -n "#$r running appvideo (25 fps)..."
	START_TIME=$SECONDS
	./appvideo -ip $IP -port $PORT -t $TIME -n $SAMP -iw $WAIT -wifi ra0 -cam 1 -nodisplay -fps 25 -Bmax $MAXBITRATE -o dat/appvideo_25_r${r}.dat > dat/appvideo_25_r${r}.log
	echo "$(($(($SECONDS - $START_TIME))/60)) min $(($(($SECONDS - $START_TIME))%60)) sec"

echo -n "#$r running appvideo (5 fps)..."
	START_TIME=$SECONDS
	./appvideo -ip $IP -port $PORT -t $TIME -n $SAMP -iw $WAIT -wifi ra0 -cam 1 -nodisplay -fps 5 -Bmax $MAXBITRATE -o dat/appvideo_05_r${r}.dat > dat/appvideo_05_r${r}.log
	echo "$(($(($SECONDS - $START_TIME))/60)) min $(($(($SECONDS - $START_TIME))%60)) sec"

	echo -n "#$r running appvideo (15 fps)..."
	START_TIME=$SECONDS
	./appvideo -ip $IP -port $PORT -t $TIME -n $SAMP -iw $WAIT -wifi ra0 -cam 1 -nodisplay -fps 15 -Bmax $MAXBITRATE -o dat/appvideo_15_r${r}.dat > dat/appvideo_15_r${r}.log
	echo "$(($(($SECONDS - $START_TIME))/60)) min $(($(($SECONDS - $START_TIME))%60)) sec"  

done


