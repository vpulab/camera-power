#include <vector>	// std::vector
#include <fstream>	// std::ostringstream
#include <iostream> // std::cout, std::endl
#include <iomanip> 	// std::setfill, std::setw, std::setprecision
#include <numeric> 	// std::accumulate, std::inner_product

#include <unistd.h>	// std::sleep

#include "utils.h"
#include "system_utils.h"
#include "PropertyFileReader.h"

using namespace std;
using namespace cv;

battery_reads_cam_t calibrate(sim_config_t config, battery_reads_cam_t &stats)
{
	double tmp, time2;
	double time = config.time;
	double samples = config.samples;

	if(config.disableUSB == true)
		stats.name = "system";
	else
		stats.name = "system_usb";

	stats.id=-1;
	stats.width = 0;
	stats.height = 0;
	stats.fps = time;
	stats.realfps = time;
	stats.time = time;
	stats.samples = samples;

	stats.n_reads=0;
	stats.energy.clear();
	stats.voltage.clear();
	stats.power.clear();

	std::cout << "Waiting " << config.initialwait << " secs to get measurements..." << std::endl;
	usleep(1e6*config.initialwait);

	stats.start_time = (double)getTickCount()/getTickFrequency();
	for(int i=0;i<samples;i++)
	{
		usleep(time*1e6);
		time2 = (double)getTickCount()/getTickFrequency();
		stats.n_reads++;
		std::cout << (int)(time2 - stats.start_time) << " " << std::flush;

		//get battery stats
		PropertyFileReader filereader("/sys/class/power_supply/BAT1/uevent");
		filereader.getProperty("POWER_SUPPLY_ENERGY_NOW" , &tmp); stats.energy.push_back(tmp);
		filereader.getProperty("POWER_SUPPLY_VOLTAGE_NOW", &tmp); stats.voltage.push_back(tmp);
		filereader.getProperty("POWER_SUPPLY_POWER_NOW"  , &tmp); stats.power.push_back(tmp);
	}
	stats.end_time = (double)getTickCount()/getTickFrequency();
	return stats;
}


int test_camera(int id, CvSize res, double fps, sim_config_t config, battery_reads_cam_t &stats)
{
	cv::Mat frame;
	double tmp,t1,t2,t3;
	int count=0,record_frames=0;

	VideoCapture cap(id); // open the second camera
	if(!cap.isOpened()) // check if we succeeded
	{
		printf("Could not open camera!");
	    return -1;
	}

	cap.set(CV_CAP_PROP_FRAME_WIDTH,(double)res.width);
	if(res.width!=(int)cap.get(CV_CAP_PROP_FRAME_WIDTH))
		printf("Width property cannot be set (default %d)\n", (int)cap.get(CV_CAP_PROP_FRAME_WIDTH));

	cap.set(CV_CAP_PROP_FRAME_HEIGHT,(double)res.height);
	if(res.height!=(int)cap.get(CV_CAP_PROP_FRAME_HEIGHT))
		printf("Height property cannot be set (default %d)\n", (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));

	cap.set(CV_CAP_PROP_FPS, fps);
	if(fps != cap.get(CV_CAP_PROP_FPS) )
		printf("FPS property cannot be set (default %.2f)\n", cap.get(CV_CAP_PROP_FPS));

	printf("here!");
	stats.id=id;
	std::ostringstream os;
	os << "cam"<<stats.id;
	stats.name=os.str().c_str();
	stats.width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	stats.height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	stats.fps = cap.get(CV_CAP_PROP_FPS);
	stats.n_reads=0;
	stats.time=config.time;
	stats.samples=config.samples;
	stats.energy.clear();
	stats.voltage.clear();
	stats.power.clear();

	record_frames=(int)(stats.fps*config.time);
	cap >> frame; // init the camera capture (initialization up to 1sec)
	std::cout << "\tSettings width=" << stats.width << " height=" << stats.height << " fps=" << stats.fps << "\nGetting measurements..." << std::endl;

	std::cout << "Waiting " << config.initialwait << " secs to get measurements..." << std::endl;
	usleep(1e6*config.initialwait);

	//loop to get measurements
	stats.start_time = (double)getTickCount()/getTickFrequency();
	for(;;)
	{
		t1 = (double)getTickCount()/getTickFrequency();
		count++;
		if(config.capture)
			cap >> frame; // get a new frame from camera
		//cap.grab(); // call the faster method VideoCapture::grab() to capture the frame
		//cap.retrieve(frame); // call the slower method VideoCapture::retrieve() to decode and get frame from each camera
		t2 = (double)getTickCount()/getTickFrequency();

		if (count%record_frames==0)
		{
			//get battery stats
			stats.n_reads++;
		    PropertyFileReader filereader("/sys/class/power_supply/BAT1/uevent");
		    filereader.getProperty("POWER_SUPPLY_ENERGY_NOW" , &tmp); stats.energy.push_back(tmp);
		    filereader.getProperty("POWER_SUPPLY_POWER_NOW"  , &tmp); stats.power.push_back(tmp);
		    filereader.getProperty("POWER_SUPPLY_VOLTAGE_NOW", &tmp); stats.voltage.push_back(tmp);

		    stats.captureT.push_back(1e3*(t2-t1));
		}
		t3 = (double)getTickCount()/getTickFrequency();

		double delay = 1/stats.fps - (t3-t1);//seconds
		if (config.wait && delay>0)
			usleep(1e6*delay);

		if (count%record_frames==0){
			stats.sleepT.push_back(1e3*delay);
			if(config.verbose){
				std::cout << "\n\tcapture=" << (t2-t1)*1e3 << "ms proc=" << (t3-t2)*1e3 << "ms sleep=" << delay*1e3 << " ms";
				std::cout << "-> " << count << " " << std::flush;
			}
		}

		if((t2-stats.start_time) > config.time*config.samples)
			break;
	}
	stats.end_time = (double)getTickCount()/getTickFrequency();
	if (config.capture)
		stats.realfps = count/(stats.end_time-stats.start_time);
	else
		stats.realfps = 0;

	std::cout << " done. (" << stats.n_reads << " reads, realfps=" << stats.realfps << ")"  << std::endl;
	printStats(stats);
	/*vector<int> compression_params;
	compression_params.push_back(IMWRITE_PNG_COMPRESSION);
	//        compression_params.push_back(9);

	std::ostringstream os1;
	os1 << "cam"<<stats.id << ".png";

	cv::imwrite(os1.str().c_str(),frame,compression_params);
	*/

	cap.release();
	return 1;
}
int printStats(battery_reads_cam_t stats)
{
	std::cout << "Camera" << stats.id << "stats\n\t config: width=" << stats.width << " height=" << stats.height << " fps=" << stats.fps << std::endl;

	double sum = std::accumulate(stats.power.begin(), stats.power.end(), 0.0);
	double mean = sum / stats.power.size();

	double sq_sum = std::inner_product(stats.power.begin(), stats.power.end(), stats.power.begin(), 0.0);
	double stdev = std::sqrt(sq_sum / stats.power.size() - mean * mean);

	std::cout << "\tPower (w): mean=" << mean << " var=" << stdev << std::endl;

	return 1;
}

int save_run(std::ofstream& outfile,int run, battery_reads_cam_t stats)
{
	//outfile << std::fixed;
	outfile << "RUN=" << run << std::endl;
	outfile << "name=" << stats.name << std::endl;
	outfile << "width=" << stats.width << std::endl;
	outfile << "height=" << stats.height << std::endl;
	outfile << std::setprecision(4) << "fps=" << stats.fps << std::endl;
	outfile << std::setprecision(4) << "realfps=" << stats.realfps << std::endl;
	outfile << "reads=" << stats.n_reads << std::endl;
	outfile << std::setprecision(2) << "time=" << stats.time << std::endl;
	outfile << std::setprecision(2) << "samples=" << stats.samples << std::endl;
	outfile << std::setprecision(6) << "start_time=" << stats.start_time << std::endl;
	outfile << std::setprecision(6) << "end_time=" << stats.end_time << std::endl;

	outfile << "energy=";
	for (std::vector<double>::const_iterator i = stats.energy.begin(); i != stats.energy.end(); ++i)
		outfile << std::setprecision(8) << *i << ' ';

	outfile << "\npower=";
	for (std::vector<double>::const_iterator i = stats.power.begin(); i != stats.power.end(); ++i)
		outfile << std::setprecision(8) << *i << ' ';

	outfile << "\nvoltage=";
	for (std::vector<double>::const_iterator i = stats.voltage.begin(); i != stats.voltage.end(); ++i)
		outfile << std::setprecision(8) << *i << ' ';

	outfile << "\ncaptureT=";
		for (std::vector<double>::const_iterator i = stats.captureT.begin(); i != stats.captureT.end(); ++i)
			outfile << std::setprecision(8) << *i << ' ';

    outfile << "\nsleepT=";
		for (std::vector<double>::const_iterator i = stats.sleepT.begin(); i != stats.sleepT.end(); ++i)
			outfile << std::setprecision(8) << *i << ' ';
	outfile << std::endl;
	return 1;
}
