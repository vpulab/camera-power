#include <vector>
#include <opencv.hpp>

#ifndef __UTILS_H__
#define __UTILS_H__

#define HOST_IP "150.244.59.42"
//#define HOST_IP "127.0.0.1"
#define HOST_PORT 1024
#define BUFFER_SIZE 1000000

struct sim_config_t {
	bool disableUSB;
	bool testTX;
	bool testRX;
	double time,samples,repeat;
	bool calibrate;
	bool wait;
	std::string filesave;
	std::string usbport;
	std::string usbdevice;
	std::string adminPass;
	bool usbmeter;

	double initialwait;
	bool verbose;
	int dataPkt;
	double maxBitrate;

	std::string hostIP;
	int hostPort;

	std::string interface;
};

struct battery_reads_cam_t {
	int pktSize;
	double maxBitrate;
	int npackets;
	int n_reads;
	double time, samples;
	double start_time, end_time;
	std::vector<double> energy;
	std::vector<double> voltage;
	std::vector<double> power;
	std::vector<double> captureT;
	std::vector<double> sleepT;

	std::string name;
	std::string iniTX,iniRX;
	std::string endTX,endRX;
};

//const double TestFPS[] = {5, 10, 15, 20, 25 };
const double TestFPS[] = {5, 15, 25 };

battery_reads_cam_t calibrate(sim_config_t config, battery_reads_cam_t &stats);
int test_wifi(sim_config_t config, battery_reads_cam_t &stats);
int tx_data(int sock, int bytes);
int rx_data(int sock, int bytes);

int printStats(battery_reads_cam_t stats);
int save_run(std::ofstream& outfile,int run, battery_reads_cam_t stats);

#endif // __UTILS_H__


