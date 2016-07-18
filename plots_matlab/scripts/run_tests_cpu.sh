#!/bin/bash

clear
#echo "Deleting files in ./dat && ./logs..."
#rm -rf dat/* && rm -rf logs/*

id=1 #camera id

WAIT=50
TIME=5 #sampling time (in seconds)
SAMP=200 #number of samples to take

START=1 #repeat measurements (start index)
REPEAT=$((START+5)) #repeat measurements

Freqs=(1.2 1.6 2.0 2.4)
NFreqs=${#Freqs[@]}

echo "CPU TEST"
echo "Each test will wait $WAIT secs to get data ($SAMP samples at $TIME secs/sample)."
#read -p 'Reset the measurement device and press any key to continue... ' -n1 -s

for r in $(seq $START 1 $REPEAT)
do
	for i in $(seq 0 1 $((NFreqs-1)))
	do
		f=${Freqs[$i]}				
		echo -n "#$r running calibration for $f GHz (idle)  ..."
		START_TIME=$SECONDS
		./cpupower -c -t $TIME -n $SAMP -iw $WAIT -f $f -o dat/PRO_idle_${f}_r${r}.dat > tmp.tmp
		echo "$(($(($SECONDS - $START_TIME))/60)) min $(($(($SECONDS - $START_TIME))%60)) sec"

		echo -n "#$r running stress cpu  for $f GHz (active)..."
		START_TIME=$SECONDS
		./cpupower -t $TIME -n $SAMP -iw $WAIT -f $f -o dat/PRO_act_${f}_r${r}.dat > tmp.tmp
		echo "$(($(($SECONDS - $START_TIME))/60)) min $(($(($SECONDS - $START_TIME))%60)) sec"
	done
done


