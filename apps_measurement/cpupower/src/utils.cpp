#include <vector>	// std::vector
#include <fstream>	// std::ostringstream
#include <iostream> // std::cout, std::endl
#include <iomanip> 	// std::setfill, std::setw, std::setprecision
#include <numeric> 	// std::accumulate, std::inner_product

#include <unistd.h>	// std::sleep
#include <opencv2/opencv.hpp>

#include "utils.h"
#include "system_utils.h"
#include "PropertyFileReader.h"

using namespace std;
using namespace cv;

int calibrate(sim_config_t config, battery_reads_cam_t &stats)
{
	double tmp, time2;
	double freq = config.freq;
	double time = config.time;
	double samples = config.samples;

	stats.name = "cpuidle";

	stats.id=-1;
	stats.freq = freq;
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
		std::cout << std::setprecision(2) << (int)(time2 - stats.start_time) << " " << std::flush;

		//get battery stats
		PropertyFileReader filereader("/sys/class/power_supply/BAT1/uevent");
		filereader.getProperty("POWER_SUPPLY_ENERGY_NOW" , &tmp); stats.energy.push_back(tmp);
		filereader.getProperty("POWER_SUPPLY_VOLTAGE_NOW", &tmp); stats.voltage.push_back(tmp);
		filereader.getProperty("POWER_SUPPLY_POWER_NOW"  , &tmp); stats.power.push_back(tmp);
	}
	stats.end_time = (double)getTickCount()/getTickFrequency();
	return 1;
}

int testcpu(sim_config_t config, battery_reads_cam_t &stats)
{
	double tmp,tacc=0;
	double freq = config.freq;
	double time = config.time;
	double samples = config.samples;

	stats.name = "cpuact";

	stats.id=-1;
	stats.freq = freq;
	stats.time = time;
	stats.samples = samples;

	stats.n_reads=0;
	stats.energy.clear();
	stats.voltage.clear();
	stats.power.clear();

	std::cout << "Waiting " << config.initialwait << " secs to get measurements..." << std::endl;
	usleep(1e6*config.initialwait);

	cv::Mat A(1000, 1000, CV_32F, Scalar(5.0));
	stats.start_time = (double)getTickCount()/getTickFrequency();

	bool keep = true;
	while (keep)
	{
		double t1 = (double)getTickCount()/getTickFrequency();
		for(int i=0;i<50;i++)
			cv::Mat B = A+A;
		double t2 = (double)getTickCount()/getTickFrequency();
		tacc = tacc+t2-t1;
		//std::cout << tacc << " ms" << std::endl;

		if(tacc>time)
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

			if(stats.n_reads == samples)
				keep = false;
		}
	}
	stats.end_time = (double)getTickCount()/getTickFrequency();
	return 1;
}

int printStats(battery_reads_cam_t stats)
{
	std::cout << "Camera" << stats.id << "stats\n\t config: freq=" << stats.freq << std::endl;

	double sum = std::accumulate(stats.power.begin(), stats.power.end(), 0.0);
	double mean = sum / stats.power.size();

	double sq_sum = std::inner_product(stats.power.begin(), stats.power.end(), stats.power.begin(), 0.0);
	double stdev = std::sqrt(float(sq_sum / stats.power.size() - mean * mean));

	std::cout << "\tPower (w): mean=" << mean << " var=" << stdev << std::endl;

	return 1;
}

int save_run(std::ofstream& outfile,int run, battery_reads_cam_t stats)
{
	//outfile << std::fixed;
	outfile << "RUN=" << run << std::endl;
	outfile << "name=" << stats.name << std::endl;
	//outfile << "width=" << stats.width << std::endl;
	//outfile << "height=" << stats.height << std::endl;
	//outfile << std::setprecision(4) << "fps=" << stats.fps << std::endl;
	outfile << std::setprecision(4) << "freq=" << stats.freq*1000 << std::endl;
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

	outfile << std::endl;
	return 1;
}
