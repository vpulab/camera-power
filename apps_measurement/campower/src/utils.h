#include <vector>
#include <opencv.hpp>

#ifndef __UTILS_H__
#define __UTILS_H__

#define MAX_FPS 35.0
#define MAX_CAMERAS 20

std::vector<CvSize> getSupportedResolutions(int id);
std::vector<double> getSupportedFPS(int id);
std::vector<int> detectNumAttachedCameras();

struct sim_config_t {
	int camID;
	bool disableUSB;
	int width,height;
	double fps;
	double time,samples,repeat;
	bool calibrate,single_test,full_test;
	bool capture,wait;
	std::string filesave;
	std::string usbport;
	std::string usbdevice;
	std::string adminPass;
	bool usbmeter;

	double initialwait;
	bool verbose;
};

struct battery_reads_cam_t {
	int id;
	double fps;
	double realfps;
	int width,height;
	int n_reads;
	double time, samples;
	double start_time, end_time;
	std::vector<double> energy;
	std::vector<double> voltage;
	std::vector<double> power;
	std::vector<double> captureT;
	std::vector<double> sleepT;

	std::string name;
};

//const double TestFPS[] = {5, 10, 15, 20, 25 };
const double TestFPS[] = {5, 15, 25 };

battery_reads_cam_t calibrate(sim_config_t config, battery_reads_cam_t &stats);
int test_camera(int id, CvSize res, double fps, sim_config_t config, battery_reads_cam_t &stats);
int printStats(battery_reads_cam_t stats);
int save_run(std::ofstream& outfile,int run, battery_reads_cam_t stats);

#endif // __UTILS_H__


