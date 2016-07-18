/* CAMERA (IMAGE SENSOR) POWER CALIBRATION
 *
 * This program implements the calibration method for USB webcams described in:
 *
 * 	"Energy consumption models for smart-camera networks",
 *	J. SanMiguel and A. Cavallaro, "
 *	IEEE Transactions on Circuits and Systems for Video Technology, 2016
 *
 *	If you use this material, please cite this paper.
 *
 * The objective of this program is to measure:
 * - Power for calibration: disable USB, system power
 * - Power sleep: USB port powered on
 * - Power idle: USB port powered on & camera configured (no streaming)
 * - Power active: USB port powered on & camera configured (streaming)
 *
 * Testing conditions:
 * - Screen brightness set to minimum
 * - Disable all network interfaces, bluetooth, usb ports and CPUs (except CPU0, set to 1.20GHz)
 * - Enable usb port for the USB webcam
 *
 *
 *  INSTRUCTIONS TO RUN THIS APPLICATION
 *  1) Setup the appropiate HW ID of the camera (line main.cpp::70)
 *  2) The current user running this program must have admin rights to enable/disable HW
 *  	Please use your admin password in the file system_utils.cpp
 *  	Change the string "your_pass_here" with your admin password
 *  3) Check the default settings for comms in the "config" struture (dataPkt, maxBitrate,
 *  	interface, hostIP & hostPort) in the function "system_utils.cpp::parseCmdOptions"
 *  4) Please do check the testing conditions in "system_utils.cpp::enableTestConditions" &
 *	    "system_utils.cpp::disableTestConditions" to match your current HW (i.e. the number of cores to disable)
 *  5) Compile this program using the associated makefile
 *  6) Use the bash script "run_tests_640x480.sh" to configure the calibration procedure
 *     Currently you can test active/idle/sleep modes with different framerates, number of
 *     runs, the length of each test and the sampling frequency.
 *     (Current experiments are for 640x480 framesizes using 5, 15 and 25 fps;
 *      each test will wait 50 secs to get data: 200 samples at 5 secs/sample).
 *  7) The result are stored in data files for each run (e.g.  SEN_act_640x480@5_r01.dat)
 *
 *
 * Library requirements:
 * - OpenCV library v3.0 http://opencv.org
 * - libusb v1.0 http://www.libusb.org/
 * - Measurement Unix utils: acpitool, ifconfig, cpufreq-tools
 *
 * This program has been successfully tested on:
 * - Logitech, Inc. QuickCam Ultra Vision
 * - Realtek Semiconductor Corp. USB webcam (Toshiba laptop Portege R700)
 *
 * Author: Juan C. SanMiguel (juancarlos.sanmiguel@uam.es)
 * Date: July 2016
 */
