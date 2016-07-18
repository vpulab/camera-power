#include <string>
#include "utils.h"

//http://askubuntu.com/questions/629619/how-to-install-libusb
#include <libusb-1.0/libusb.h>

#ifndef __CMD_UTILS_H__
#define __CMD_UTILS_H__

int enableTestConditions(sim_config_t config);
int disableTestConditions(sim_config_t config);

int disableUSBdevice(sim_config_t config,std::string usbdevice, std::string port);
int enableUSBdevice(sim_config_t config,std::string usbdevice, std::string port);

int parseCmdOptions(int argc, char *argv[], sim_config_t& cfg);
char* getCmdOption(char ** begin, char ** end, const std::string& option);
bool cmdOptionExists(char** begin, char** end, const std::string& option);

int create_output_file(sim_config_t config, std::string name,std::ofstream& outfile);

int list_all_usb_devices();
std::string get_device_id(libusb_device *dev);
std::string get_device_port(libusb_device *dev);
std::string getFullPortUSB(std::string id);
void printdev(libusb_device *dev); //prototype of the function

#endif //__CMD_UTILS_H__
