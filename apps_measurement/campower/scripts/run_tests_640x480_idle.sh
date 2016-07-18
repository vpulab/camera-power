#!/bin/bash

clear
#echo "Deleting files in ./dat && ./logs..."
#rm -rf dat/* && rm -rf logs/*

id=1 #camera id

WAIT=30
TIME=5 #sampling time (in seconds)
SAMP=10 #number of samples to take

START=1 #repeat measurements (start index)	
REPEAT=$((START+4)) #repeat measurements

WIDTHS=(640)
HEIGHTS=(480)
NRES=${#WIDTHS[@]}
#FPS=(5 10 15 20 25)
FPSs=(5 10 15 20 25 30)
NFPS=${#FPSs[@]}

echo "IDLE TEST"
echo "Each test will wait $WAIT secs to get data ($SAMP samples at $TIME secs/sample)."
#read -p 'Reset the measurement device and press any key to continue... ' -n1 -s

for r in $(seq $START 1 $REPEAT)
do
	for s in $(seq 0 1 $((NRES-1)))
	do
		w=${WIDTHS[$s]}
		h=${HEIGHTS[$s]}

		echo -n "#$r-$s running test calibration (disable usb)..."
		START_TIME=$SECONDS
		#./campower -usb -c -t $TIME -n $SAMP -iw $WAIT -o dat/Res_${w}x${h}_r${r}_calib.dat > logs/tmp.txt
		./campower -c -t $TIME -n $SAMP -iw $WAIT -dusb -o dat/SEN_idle_${w}x${h}_r${r}_calib_usboff.dat > logs/tmp.tmp
		echo "$(($(($SECONDS - $START_TIME))/60)) min $(($(($SECONDS - $START_TIME))%60)) sec" 

		echo -n "#$r-$s running test calibration (active usb)..."
		START_TIME=$SECONDS
		#./campower -usb -c -t $TIME -n $SAMP -iw $WAIT -o dat/Res_${w}x${h}_r${r}_calib.dat > logs/tmp.txt
		./campower -c -t $TIME -n $SAMP -iw $WAIT -o dat/SEN_idle_${w}x${h}_r${r}_calib_usbon.dat > logs/tmp.tmp
		echo "$(($(($SECONDS - $START_TIME))/60)) min $(($(($SECONDS - $START_TIME))%60)) sec" 

		for f in $(seq 0 1 $((NFPS-1)))
		do

			fps=${FPSs[$f]}
			echo -n "#$r-$s running test ${w}x${h} at ${fps} fps..."
			START_TIME=$SECONDS
			./campower -s -fps $fps -w $w -h $h -t $TIME -n $SAMP -iw $WAIT -i $id -nc -o dat/SEN_idle_${w}x${h}\@${fps}_r${r}.dat > logs/tmp.tmp
			echo "$(($(($SECONDS - $START_TIME))/60)) min $(($(($SECONDS - $START_TIME))%60)) sec"    
		done
	done
done
