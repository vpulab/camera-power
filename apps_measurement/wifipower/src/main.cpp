/* WIFI POWER CALIBRATION - CLIENT
 *
 * This program implements the calibration method for WIFI modules in Unix environment described in:
 *
 * 		"Energy consumption models for smart-camera networks",
 *		J. SanMiguel and A. Cavallaro,
 *		IEEE Transactions on Circuits and Systems for Video Technology, 2016
 *  	http://www-vpu.eps.uam.es/publications/camera_power
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
 *  2) Please check the following items in the "config.ini" file
 *  	- Admin password (this program must have admin rights to enable/disable HW)
 *  	- Server IP/Port
 *  	- Configuration for sensing, processing & comms
 *     Alternatively you can change these values via command-line arguments
 *  3) Check the testing conditions in "system_utils.cpp::enableTestConditions" &
 *	    "system_utils.cpp::disableTestConditions" to match your current Hardware specifications
 *  4) Compile this program using the associated makefile
 *  5) Configure, compile & run the server "wifipower_server"
 *  6) Use the bash script "run_tests_wifi.sh" to configure the calibration procedure
 *     Currently you can test tx/rx/idle/sleep modes with different bitrates & packet sizes, number of
 *     runs, the length of each test and the sampling frequency.
 *     (Current experiments are for a constrant bitrate of 1Mbps;each test will wait 50 secs to
 *      get data: 200 samples at 5 secs/sample).
 *  7) The result are stored in data files for each run (e.g. COM_RX_r1.dat)
 *
 * Library requirements:
 * - libusb v1.0 http://www.libusb.org/
 * - Unix utils: acpitool, ifconfig, cpufreq-tools, wondershaper
 *
 * This program has been successfully tested on:
 * - AC600® Wireless Dual Band USB Adapter (http://goo.gl/Mv9COc)
 *
 * Author: Juan C. SanMiguel (Universidad Autonoma of Madrid, juancarlos.sanmiguel@uam.es)
 * Date: July 2016
 */
#include <fstream>
#include <iostream>
#include <vector>

#include <unistd.h> //usleep

#include "opencv2/opencv.hpp"
#include "utils.h"
#include "system_utils.h"

using namespace std;
using namespace cv;

//Internal ID of usb device (to obtain the full list, type 'lsusb' in the terminal)
//$ lsusb
//$ Bus 002 Device 027: ID 148f:761a Ralink Technology, Corp.
//$ ...
//$ Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
#define usbwlanid "148f:761a"

int main(int argc, char *argv[]) {

	sim_config_t config;
	std::vector<battery_reads_cam_t> tests;
	battery_reads_cam_t stats;

	parseCmdOptions(argc,argv,config);

	//get port of usb device under test
	config.usbdevice = usbwlanid;
	config.usbport = getFullPortUSB(usbwlanid);

	if(!enableTestConditions(config))
			exit(-1);

	if(config.usbmeter) {
	 	std::cout << "Reset the measurement device and press any key to continue..." << std::endl;
		getchar();
 	 }

	if (config.disableUSB){
		disableUSBdevice(config,config.usbdevice,config.usbport);
	}

	//calibration mode
    if (config.calibrate) {

    	std::cout << "WIFI: CALIBRATION MODE" << std::endl;
    	for (int r=0;r<config.repeat;r++) {
    		calibrate(config,stats);
    		tests.push_back(stats);
    	}
    }
	else
	{
		//single test mode    
    	std::cout << "WIFI: TEST MODE" << std::endl;

		for (int r=0;r<config.repeat;r++) {
			std::cout << " RUN=" << r << std::endl;

			std::ostringstream str1,str2;
			str1 << "ifconfig " << config.interface << " | grep 'RX bytes' > init_netstat.txt";
			if(system(str1.str().c_str()))
				return -1;
			std::ifstream myFile1;
			myFile1.open("init_netstat.txt");
			myFile1 >> stats.iniRX;	myFile1 >> stats.iniRX;
			myFile1 >> stats.iniTX;	myFile1 >> stats.iniTX;	myFile1 >> stats.iniTX;	myFile1 >> stats.iniTX;
			myFile1.close();

			test_wifi(config,stats);

			str2 << "ifconfig " << config.interface << " | grep 'RX bytes' > end_netstat.txt";
			if(system(str2.str().c_str()))
				return -1;
			std::ifstream myFile2;
			myFile2.open("end_netstat.txt");

			myFile2 >> stats.endRX;	myFile2 >> stats.endRX;
			myFile2 >> stats.endTX;	myFile2 >> stats.endTX;	myFile2 >> stats.endTX;	myFile2 >> stats.endTX;
			myFile2.close();

			tests.push_back(stats);
		}
    }

    //save measurements
    if(tests.size()>0){
    	std::cout << "\nSave measurements" << std::endl;
    	std::ofstream outfile;
    	//create_output_file(config, tests[0].name,outfile);
    	outfile.open(config.filesave.c_str());
		
    	for(unsigned int m=0;m<tests.size();m++)
    		save_run(outfile,m,tests[m]);

    	outfile.close();
    }

	disableTestConditions(config);

	if (config.disableUSB)
		enableUSBdevice(config,config.usbdevice,config.usbport);

	return 0;
}
