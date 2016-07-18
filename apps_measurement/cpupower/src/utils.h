#include <vector>

#ifndef __UTILS_H__
#define __UTILS_H__

#define MAX_FREQ 2500
#define MIN_FREQ 1100


struct sim_config_t {
	//int camID;
	//bool disableUSB;
	//int width,height;
	double freq;
	double time,samples,repeat;
	std::string filesave;
	std::string adminPass;
	//std::string usbport;
	//std::string usbdevice;
	//bool capture,wait;
	//bool usbmeter;
	bool calibrate;
	bool wait;
	double initialwait;
	bool verbose;
};

struct battery_reads_cam_t {
	int id;
	//double fps;
	double freq;
	//double realfps;
	//int width,height;
	int n_reads;
	double time, samples;
	double start_time, end_time;
	std::vector<double> energy;
	std::vector<double> voltage;
	std::vector<double> power;
	std::string name;
};
int calibrate(sim_config_t config, battery_reads_cam_t &stats);
int testcpu(sim_config_t config, battery_reads_cam_t &stats);

int printStats(battery_reads_cam_t stats);
int save_run(std::ofstream& outfile,int run, battery_reads_cam_t stats);

#endif // __UTILS_H__


