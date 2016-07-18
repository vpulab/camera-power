/* VIDEO PROCESSING APPLICATION - CLIENT
 * This program implements a video processing application for multi-target people tracking-by-detection 
 * using the upper body detection [1], implemented using OpenCV. Visual descriptors of tracked people 
 * are generated (RGB, HSV and YCrCb 16-bin histograms) and wirelessly  transmitted to the server 
 * for re-identification applications [2].
 *
 * Refs
 * [1] R. Vezzani et al, “People reidentification in surveillance and forensics: A survey,” ACM Computing Surveys, 2013.
 * [2] R. Mazzon et al, “Person re-identification in crowd,” Pattern Recogn. Lett., 2012.
 *
 *  INSTRUCTIONS TO RUN THIS APPLICATION
 *  1) Compile and run the server 'appvideo_server'
 *  2) Please check the following items in the "config.ini" file
 *  	- Admin password (this program must have admin rights to enable/disable HW)
 *  	- Server IP/Port
 *  	- Configuration for sensing, processing & comms
 *     Alternatively you can change these values via command-line arguments
 *  3) Check the testing conditions in "system_utils.cpp::enableTestConditions" &
 *	    "system_utils.cpp::disableTestConditions" to match your current Hardware specifications
 *  4) Compile this program using the associated makefile
 *  5) Run this program by typing "./appvideo" in the command line
 *
 * Library requirements:
 * - OpenCV library v3.0 http://opencv.org
 * - libusb v1.0 http://www.libusb.org/
 * - Unix utilities: acpitool, ifconfig, cpufreq-tools, wondershaper
 *	
 * If you use this material, please cite the following paper:
 * 	"Energy consumption models for smart-camera networks",
 *	J. SanMiguel and A. Cavallaro,
 *	IEEE Transactions on Circuits and Systems for Video Technology, 2016
 *	http://www-vpu.eps.uam.es/publications/camera_power
 *
 * Author: Juan C. SanMiguel (Universidad Autonoma of Madrid, juancarlos.sanmiguel@uam.es)
 * Date: July 2016
 */

//system libraries
//C
#include <stdio.h>
#include <unistd.h>	// std::sleep

//C++
#include <sstream>
#include <iostream> // std::cout, std::endl
#include <fstream> // std::ifstream

//opencv libraries
#include <opencv2/opencv.hpp>

//VPU libraries
#include <BlobExtractor.hpp>
#include <BasicBlobTracker.hpp>
#include <BasicDescriptor.hpp>

#include "system_utils.h"
#include "PropertyFileReader.h"

//namespaces
using namespace cv; //avoid using 'cv' to declare OpenCV functions and variables (cv::Mat or Mat)
std::string upper_body_cascade_name = "./models/haarcascades/haarcascade_upperbody.xml";

//main function
int main(int argc, char ** argv) 
{
	Mat frame, fgMaskMOG2; // current Frame
	double tSen=0,tPro=0,tCom=0,tTot=0,tTotSim=0,tacc=0,tmp=0; //variable for execution time
	int count=0,bytesTX=0,bytesRX=0;

	//stats structures
	sim_config_t config;
	battery_reads_cam_t stats;
	stats.n_reads=0;
	stats.name="appvideo";

	//parse command line
	parseCmdOptions(argc,argv,config);
	stats.config=config;

	//enable testing conditions
	enableTestConditions(config);

	//video modules
	VideoCapture cap;//reader to grab videoframes
	Ptr<BackgroundSubtractor> pMOG2 = cv::createBackgroundSubtractorMOG2(); //MOG2 approach;
	BlobExtractor pBlobext;
	BasicBlobTracker tracker;
	BasicDescriptor descriptor;
	std::vector<cvBlob> blobs;

	CascadeClassifier upper_body_cascade;
	if (!upper_body_cascade.load(upper_body_cascade_name)){ printf("--(!)Error loading\n"); return -1; };

	//std::string inputvideo = "/home/jcs/Videos/ETRI_od_A.avi"; //path for the videofile to process
	std::string inputvideo = "/home/jcs/Videos/LGW_20071101_E1_CAM1.mpeg"; //path for the videofile to process
	
	//INIT capture settings
	if (config.camID != -1){

		//capture from USB camera
		cap.open(config.camID);
		if (!cap.isOpened()) {
			std::cout << "Could not open camera " << config.camID << std::endl;
			return -1;
		}

		cap.set(CV_CAP_PROP_FRAME_WIDTH,(double)config.width);
		if(config.width!=(int)cap.get(CV_CAP_PROP_FRAME_WIDTH))
			printf("Width property cannot be set (default %d)\n", (int)cap.get(CV_CAP_PROP_FRAME_WIDTH));

		cap.set(CV_CAP_PROP_FRAME_HEIGHT,(double)config.height);
		if(config.height!=(int)cap.get(CV_CAP_PROP_FRAME_HEIGHT))
			printf("Height property cannot be set (default %d)\n", (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));

		cap.set(CV_CAP_PROP_FPS, config.fps);
		if(config.fps != cap.get(CV_CAP_PROP_FPS) )
			printf("FPS property cannot be set (default %.2f)\n", cap.get(CV_CAP_PROP_FPS));
	}
	else
	{
		//capture from videofile
		cap.open(inputvideo);
		if (!cap.isOpened()) {
			std::cout << "Could not open video file " << inputvideo << std::endl;
			return -1;
		}
	}
	cap >> frame; //get first video frame

	//INIT wifi settings
	std::ostringstream str1;
	str1 << "ifconfig " << config.interface << " | grep 'RX bytes' > init_netstat.txt";
	if(system(str1.str().c_str()))
		return -1;
	std::ifstream myFile1;
	myFile1.open("init_netstat.txt");
	if (myFile1.is_open()) {
		myFile1 >> stats.iniRX;
		myFile1 >> stats.iniTX;
		myFile1.close();
	}
	//std::cout <<  stats.iniRX <<  " " << stats.iniTX << std::endl;

	//std::cout << "Waiting " << config.initialwait << " secs to get measurements..." << std::endl;
	usleep(1e6*config.initialwait);

	//main loop
	bool do_processing=true;
	double tFrame = 1/config.fps;//time allocated to process a single frame
	stats.start_time = (double)getTickCount()/getTickFrequency();
	descriptor.start(config.hostIP, config.hostPort);

	while(do_processing) {
		//std::cout << "capture" << std::endl;
		count++;
		blobs.clear();
		//SENSING
		tSen = (double)getTickCount();
		//get frame & check if we achieved the end of the file (e.g. frame.data is empty)
		cap >> frame;
		cv::resize(frame,frame,cv::Size(320,240));
		if (!frame.data)
			break;
		tSen = ((double)getTickCount() - tSen)/ cv::getTickFrequency();

		//PROCESSING
		tPro = (double)getTickCount();

		// update the background model
		//pMOG2->apply(frame, fgMaskMOG2);
		//cv::erode(fgMaskMOG2, fgMaskMOG2, Mat(), Point(-1, -1), 1, 1, 1);
		//cv::dilate(fgMaskMOG2, fgMaskMOG2, Mat(), Point(-1, -1), 2, 1, 1);

		// extrack the blobs & filter the mask
		//pBlobext.extractBlobs(fgMaskMOG2, false);
		//std::cout << "Num blobs=" << pBlobext.getNumBlobs() << std::endl;
		//std::cout << "detect" << std::endl;
		std::vector<Rect> found; //detected people (as rectangles)
		Mat frame_gray;
		cvtColor(frame, frame_gray, CV_BGR2GRAY);
		equalizeHist(frame_gray, frame_gray);
		//-- Detect faces
	    upper_body_cascade.detectMultiScale(frame_gray, found, 1.5, 1, 0 | CV_HAAR_SCALE_IMAGE, Size(20, 20));

	    //draw bounding boxes
	    for(size_t i = 0; i < found.size(); i++ )
	    	blobs.push_back(initBlob(i,found[i].x,found[i].y,found[i].width,found[i].height));

	    double ttrack = (double)getTickCount();
		// perform blob tracking
	    //std::cout << "tracking" << std::endl;
		tracker.processFrame(frame, fgMaskMOG2,blobs);
	    //tracker.processFrame(frame, fgMaskMOG2, *(pBlobext.getBlobList()));
		ttrack = ((double)getTickCount() -ttrack)/getTickFrequency();
		
		// generate visual descriptors
		double tdesc = (double)getTickCount();
		//std::cout << "describe" << std::endl;
		descriptor.generateDescriptors(count,(double)getTickCount(),frame,tracker.getBlobList());
		descriptor.compressDescriptors();
		tdesc = ((double)getTickCount()-tdesc)/getTickFrequency();

		if(config.display)
		{
			//get the frame number and write it on the current frame
			std::stringstream ss;
			ss << count; //ss << cap.get(CAP_PROP_POS_FRAMES);
			putText(frame, ss.str().c_str(), cv::Point(15, 15),FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255)); //text in red

			//imshow("FG Mask MOG2", fgMaskMOG2);
			imshow("tracking", tracker.getResultImage(frame));

			//draw bounding boxes
			for(size_t i = 0; i < found.size(); i++ )
				rectangle(frame, found[i].tl(), found[i].br(), cv::Scalar(0,255,0), 3);

			imshow("frame",frame);
		}
		tPro = ((double)getTickCount() - tPro)/ cv::getTickFrequency();

		//COMMUNICATION
		tCom = (double)getTickCount();
		// transmit visual descriptors
		descriptor.sendDescriptors(bytesTX,bytesRX);
		tCom = ((double)getTickCount() - tCom)/ cv::getTickFrequency();

		//Time measurement
		tTot=tSen+tPro+tCom;
		tacc=tacc+tTot;
		tTotSim = tTotSim + tTot;

		printf("Total time = %gms (sen=%gms,pro=%gms(track=%gms+des=%gms),com=%gms)\n", tTot*1e3,tSen*1e3,tPro*1e3,ttrack*1e3,tdesc*1e3,tCom*1e3);
		//wait for next frame if we process faster than the sensing rate
		if(tTot<tFrame)
		{
			//std::cout << "waiting " << (tFrame-tTot)*1e3 << "ms"<< std::endl;
			usleep((tFrame-tTot)*1e6);
			tacc=tacc+(tFrame-tTot);
		}
		else
			std::cout << "exceeded tFrame " << (tFrame-tTot)*1e3 << "ms"<< std::endl;

		//PERFORM POWER MEASUREMENT
		if(tacc>config.time)
		{
			tacc=0;
			stats.n_reads++;
			double time = (double)getTickCount()/getTickFrequency();
			std::cout << (time - stats.start_time) << " " << std::flush;

			//get battery stats
			PropertyFileReader filereader("/sys/class/power_supply/BAT1/uevent");
			filereader.getProperty("POWER_SUPPLY_ENERGY_NOW" , &tmp); stats.energy.push_back(tmp);
			filereader.getProperty("POWER_SUPPLY_VOLTAGE_NOW", &tmp); stats.voltage.push_back(tmp);
			filereader.getProperty("POWER_SUPPLY_POWER_NOW"  , &tmp); stats.power.push_back(tmp);

			if(stats.n_reads == config.samples)
				do_processing = false;
		}
		stats.tSenAct.push_back(tSen);
		stats.tProAct.push_back(tPro);
		stats.tComAct.push_back(tCom);
		stats.ComActTX.push_back(bytesTX);
		stats.ComActRX.push_back(bytesRX);


		//exit if ESC key is pressed
		if(waitKey(30) == 27) break;
	}
	stats.end_time = (double)getTickCount()/getTickFrequency();
	stats.realfps = count/tTotSim;
	stats.nframes = count;

	std::ostringstream str2;
	str2 << "ifconfig " << config.interface << " | grep 'RX bytes' > end_netstat.txt";
	if(system(str2.str().c_str()))
		return -1;
	std::ifstream myFile2;
	myFile2.open("end_netstat.txt");
	myFile2 >> stats.endRX;	myFile2 >> stats.endRX;
	myFile2 >> stats.endTX;	myFile2 >> stats.endTX;	myFile2 >> stats.endTX;	myFile2 >> stats.endTX;
	myFile2.close();

	//SAVE POWER MEASUREMENT
	save_run(config,stats);

	//release all resources
	cap.release();
	destroyAllWindows();
	disableTestConditions(config);

	return 0;
}
