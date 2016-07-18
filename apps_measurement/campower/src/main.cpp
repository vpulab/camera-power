/* CAMERA (IMAGE SENSOR) POWER CALIBRATION
 *
 * This program implements the calibration method for USB webcams described in:
 *
 * 	"Energy consumption models for smart-camera networks",
 *	J. SanMiguel and A. Cavallaro,
 *	IEEE Transactions on Circuits and Systems for Video Technology, 2016
 *  http://www-vpu.eps.uam.es/publications/camera_power
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
 *  2) Please check the following items in the "config.ini" file
 *  	- Admin password (this program must have admin rights to enable/disable HW)
 *  	- Server IP/Port are not used by this application
 *  	- Configuration for sensing, processing & comms
 *     Alternatively you can change these values via command-line arguments
 *  3) Check the testing conditions in "system_utils.cpp::enableTestConditions" &
 *	    "system_utils.cpp::disableTestConditions" to match your current Hardware specifications
 *  4) Compile this program using the associated makefile
 *  5) Use the bash script "run_tests_640x480.sh" to configure the calibration procedure
 *     Currently you can test active/idle/sleep modes with different framerates, number of
 *     runs, the length of each test and the sampling frequency.
 *     (Current experiments are for 640x480 framesizes using 5, 15 and 25 fps;
 *      each test will wait 50 secs to get data: 200 samples at 5 secs/sample).
 *  6) The result are stored in data files for each run (e.g.  SEN_act_640x480@5_r01.dat)
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
//$ Bus 002 Device 013: ID 046d:08c9 Logitech, Inc. QuickCam Ultra Vision
#define usbcamid "046d:08c9"

//$ Bus 001 Device 004: ID 0bda:58f5 Realtek Semiconductor Corp.
//#define usbcamid "0bda:58f5"

int main(int argc, char *argv[]) {

	sim_config_t config;
	std::vector<battery_reads_cam_t> tests;
	battery_reads_cam_t stats;

	parseCmdOptions(argc,argv,config);

	//get port of usb device under test
	config.usbdevice = usbcamid;
	config.usbport = getFullPortUSB(usbcamid);

	if(!enableTestConditions(config))
			exit(-1);

	if(config.usbmeter) {
	 	std::cout << "Reset the measurement device and press any key to continue..." << std::endl;
		getchar();
 	 }

	if (config.disableUSB)
		disableUSBdevice(config,config.usbdevice,config.usbport);

	//calibration mode
    if (config.calibrate) {

    	std::cout << "SYSTEM CALIBRATION MODE" << std::endl;
    	for (int r=0;r<config.repeat;r++) {
    		calibrate(config,stats);
    		tests.push_back(stats);
    	}
    }

    //single test mode
    if (config.single_test) {
    	std::cout << "SINGLE TEST MODE" << std::endl;

		for (int r=0;r<config.repeat;r++) {
			std::cout << "Cam" << config.camID << " - run=" << r << std::endl;
			test_camera(config.camID,cvSize(config.width,config.height),config.fps,config,stats);
			tests.push_back(stats);
		}
    }

    //full test mode
    if (config.full_test) {
    	std::cout << "FULL TEST MODE" << std::endl;

        //get resolution&fps to test
        int nFPS = sizeof(TestFPS) / sizeof(TestFPS[0]);

        //get power measurements
        for (int r=0;r<config.repeat;r++)
        	for (int j=0;j<nFPS;j++) {
        		std::cout << "Cam" << config.camID << " - run=" << r << " test="<< j << std::endl;
        		test_camera(config.camID,cvSize(config.width,config.height),TestFPS[j],config,stats);
        		tests.push_back(stats);
        	}
    }

    //save measurements
    if(tests.size()>0){
    	std::cout << "Save measurements" << std::endl;
    	std::ofstream outfile;
    	//create_output_file(config, tests[0].name,outfile);
    	if (config.filesave.size() > 0)
    				outfile.open(config.filesave.c_str());
		else
		{
			std::ostringstream os;
			os << tests[0].name << "_" <<config.width<<"x"<<config.height;
			if (config.capture==false) os << "_nc";
			if (config.wait==false)    os << "_nw";
			os << ".dat";
			outfile.open(os.str().c_str());
		}

    	for(unsigned int m=0;m<tests.size();m++)
    		save_run(outfile,m,tests[m]);

    	outfile.close();
    }

	disableTestConditions(config);

	if (config.disableUSB)
		enableUSBdevice(config,config.usbdevice,config.usbport);

	return 0;
}
