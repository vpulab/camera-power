/* CPU PROCESSOR POWER CALIBRATION
 *
 * This program implements the calibration method for CPUs in Unix environment described in:
 * 	"Energy consumption models for smart-camera networks",
 *	J. SanMiguel and A. Cavallaro,
 *	IEEE Transactions on Circuits and Systems for Video Technology, 2016
 *	http://www-vpu.eps.uam.es/publications/camera_power
 *
 *	If you use this material, please cite this paper.
 *
 * The objective of this program is to measure the CPU running (and idle) at the desired frequency
 * We assume that power in the sleep state ~0 mW.
 *
 * Testing conditions:
 * - Screen brightness set to minimum
 * - Disable all network interfaces, bluetooth, usb ports and CPUs (except CPU0)
 * - Sets CPU0 to certain frequency values (user selected, “on-demand” settings)
 *
 *  INSTRUCTIONS TO RUN THIS APPLICATION
 *  1) Setup the appropriate HW ID of the camera (line main.cpp::60)
 *  2) Please check the following items in the "config.ini" file
 *  	- Admin password (this program must have admin rights to enable/disable HW)
 *  	- Server IP/Port are not used by this application
 *  	- Configuration for sensing, processing & comms
 *     Alternatively you can change these values via command-line arguments
 *  3) Check the testing conditions in "system_utils.cpp::enableTestConditions" &
 *	    "system_utils.cpp::disableTestConditions" to match your current Hardware specifications
 *  4) Compile this program using the associated makefile
 *  5) Use the bash script "run_tests_cpu.sh" to configure the calibration procedure
 *     Currently you can test active/idle modes with different frequencies, number of
 *     runs, the length of each test and the sampling frequency.
 *     (Current experiments are for 1.2 1.6 2.0 2.4 GHz;each test will wait 50 secs to
 *      get data: 200 samples at 5 secs/sample).
 *  6) The result are stored in data files for each run (e.g.  PRO_act_1.2_r3.dat)
 *
 * Library requirements:
 * - libusb v1.0 http://www.libusb.org/
 * - Measurement Unix utils: acpitool, ifconfig, cpufreq-tools
 *
 * This program has been successfully tested on:
 * - CPU modeling for model Intel® Core™ i5-450M  (1.20 GHz-2.40GHz) (only one core is enabled)
 *
 * Author: Juan C. SanMiguel (juancarlos.sanmiguel@uam.es)
 * Date: July 2016
 */
#include <fstream>
#include <iostream>
#include <vector>

#include <unistd.h> //usleep
//#include <opencv2/opencv.hpp>
#include "utils.h"
#include "system_utils.h"

using namespace std;
//using namespace cv;

//Internal ID of usb device (to obtain the full list, type 'lsusb' in the terminal)
//$ lsusb
//$ Bus 002 Device 013: ID 046d:08c9 Logitech, Inc. QuickCam Ultra Vision
#define usbcamid "046d:08c9"

//$ Bus 001 Device 004: ID 0bda:58f5 Realtek Semiconductor Corp.
//#define usbcamid "0bda:58f5"


int main(int argc, char *argv[]) {

	sim_config_t config;
	battery_reads_cam_t stats;
	std::vector<battery_reads_cam_t> tests;

	parseCmdOptions(argc,argv,config);

	enableTestConditions(config);

	for (int r=0;r<config.repeat;r++) {

		if (config.calibrate == true) {
			std::cout << "IDLE CPU (CALIBRATION MODE)" << std::endl;
			calibrate(config,stats);
			tests.push_back(stats);
		}
		else
		{
			std::cout << "\nACTIVE CPU (TEST MODE)" << std::endl;
			testcpu(config,stats);
			tests.push_back(stats);
		}
	}

    //save measurements
    if(tests.size()>0){
    	std::cout << "\nSave measurements: "<< config.filesave << std::endl;
    	std::ofstream outfile;
		//create_output_file(config, tests[0].name,outfile);
    	if (config.filesave.size() > 0)
    		outfile.open(config.filesave.c_str());


    	for(unsigned int m=0;m<tests.size();m++)
    		save_run(outfile,m,tests[m]);

    	outfile.close();
    }

	disableTestConditions(config);

	return 0;
}
