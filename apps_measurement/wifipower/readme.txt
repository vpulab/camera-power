/* WIFI POWER CALIBRATION - CLIENT
 *
 * This program implements the calibration method for WIFI modules in Unix environment described in:
 * 	"Energy consumption models for smart-camera networks",
 *	J. SanMiguel and A. Cavallaro"
 *	IEEE Transactions on Circuits and Systems for Video Technology, 2016
 *
 *	If you use this material, please cite this paper.
 *
 * The objective of this program is to measure the CPU running (and idle) at the desired frequency
 * We assume that power in the sleep state ~0 mW.
 *
 * This program is part of a server & client system to get power measurements for USB-wifi adapters.
 * The server only receives and sends data, being located on a network computer.
 * The client is installed in the laptop and connected to a wireless router (< 1m distance), reducing
 * the power to transmit/receive signals.
 *
 * The objective is to measure:
 * -  Power sleep: USB port powered on
 * -  Power idle: USB port powered on & adapter configured (no rx/tx)
 * -  Power active RX: USB port powered on & receive data
 * -  Power active TX: USB port powered on & transmit data
 *
 * Testing conditions:
 * -  Screen brightness set to minimum
 * - Disable all network interfaces, bluetooth, usb ports and CPUs
 	 (except CPU0, set to 1.20GHz)
 * -  Enable usb port for the USB Adapter
 * -  Enable wireless interface “ra0” (the one of the USB Adapter)
 * -  Sets the maximum bitrate to 1Mbps (rx & tx)
 *
 * Testing conditions:
 * - Screen brightness set to minimum
 * - Disable all network interfaces, bluetooth, usb ports and CPUs (except CPU0)
 * - Sets CPU0 to certain frequency values (user selected, “on-demand” settings)
 *
 *  INSTRUCTIONS TO RUN THIS APPLICATION
 *  1) Setup the appropriate HW ID of the usb-wifi adapter (line main.cpp::57)
 *  2) The current user running this program must have admin rights to enable/disable HW
 *  	Please use your admin password in the file system_utils.cpp
 *  	Change the string "your_pass_here" with your admin password
 *  3) Check the default settings for sensing, processing & comms in the "config" struture
 *  	available in the function "parseCmdOptions" in the file "system_utils.cpp".
 *  4) Please do check the testing conditions in "system_utils.cpp::enableTestConditions" &
 *	    "system_utils.cpp::disableTestConditions" to match your current HW (i.e. the number of cores to disable)
 *  5) Compile this program using the associated makefile
 *  6) Configure, compile & run the server "wifipower_server"
 *  5) Use the bash script "run_tests_wifi.sh" to configure the calibration procedure
 *     Currently you can test tx/rx/idle/sleep modes with different bitrates & packet sizes, number of
 *     runs, the length of each test and the sampling frequency.
 *     (Current experiments are for a constrant bitrate of 1Mbps;each test will wait 50 secs to
 *      get data: 200 samples at 5 secs/sample).
 *  6) The result are stored in data files for each run (e.g. COM_RX_r1.dat)
 *
 * Library requirements:
 * - libusb v1.0 http://www.libusb.org/
 * - Measurement Unix utils: acpitool, ifconfig, cpufreq-tools, wondershaper
 *
 * This program has been successfully tested on:
 * - AC600® Wireless Dual Band USB Adapter (http://goo.gl/Mv9COc)
 *
 * Author: Juan C. SanMiguel (juancarlos.sanmiguel@uam.es)
 * Date: July 2016
 */
