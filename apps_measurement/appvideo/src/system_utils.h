#include <string>
#include <vector>

//http://askubuntu.com/questions/629619/how-to-install-libusb
#include <libusb-1.0/libusb.h>

#ifndef __CMD_UTILS_H__
#define __CMD_UTILS_H__

struct sim_config_t {

	//time-measurement settings
	double time,samples,repeat;

	//USB-webcam settings
	int camID;
	int width,height;
	double fps;

	//CPU settings
	double freq;

	//Network settings
	std::string interface;
	double maxBitrate;
	std::string hostIP;
	int hostPort;

	std::string filesave;
	std::string usbport;
	std::string usbdevice;
	std::string adminPass;
	bool usbmeter;

	double initialwait;
	bool verbose;

	bool display;
};

struct battery_reads_cam_t {
	std::string name;

	sim_config_t config;
	double realfps;
	std::string iniRX, iniTX;
	std::string endRX, endTX;

	int nframes;
	int n_reads;
	double start_time, end_time;

	std::vector<double> energy;
	std::vector<double> voltage;
	std::vector<double> power;

	std::vector<double> tSenAct;
	std::vector<double> tProAct;
	std::vector<double> tComAct;
	std::vector<double> ComActTX;
	std::vector<double> ComActRX;
};

int enableTestConditions(sim_config_t config);
int disableTestConditions(sim_config_t config);

int disableUSBdevice(sim_config_t config,std::string usbdevice, std::string port);
int enableUSBdevice(sim_config_t config,std::string usbdevice, std::string port);

int parseCmdOptions(int argc, char *argv[], sim_config_t& cfg);
char* getCmdOption(char ** begin, char ** end, const std::string& option);
bool cmdOptionExists(char** begin, char** end, const std::string& option);

int save_run(sim_config_t config, battery_reads_cam_t stats);

int list_all_usb_devices();
std::string get_device_id(libusb_device *dev);
std::string get_device_port(libusb_device *dev);
std::string getFullPortUSB(std::string id);
void printdev(libusb_device *dev); //prototype of the function

#endif //__CMD_UTILS_H__
