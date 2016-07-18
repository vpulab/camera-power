#include <stdlib.h>    // system, NULL, EXIT_FAILURE
#include <iostream>    // std::cout, std::endl
#include <iomanip>     // std::setfill, std::setw
#include <fstream>     // std::ofstream
#include <algorithm>

#include <unistd.h>	   // std::sysconf

#include "system_utils.h"
#include "PropertyFileReader.h"

int system_admin(sim_config_t config,const char *str)
{
	std::ostringstream os;
	os << "echo " << config.adminPass << " | " << str;
	if (config.verbose)
		std::cout << os.str().c_str() << std::endl;
	if(system(os.str().c_str()))
		return -1;
	return 1;
}


int disableUSBdevice(sim_config_t config, std::string usbdevice, std::string usbport)
{
	if(config.verbose)
		std::cout << "Disabling usb device: " << usbdevice << " (port: " << usbport << ")" << std::endl;

	//shutdown power for usb device
	std::ostringstream os;
	os << "echo " << "''" << usbport <<"'' | sudo -S tee /sys/bus/usb/drivers/usb/unbind > tmp.txt";

	if(config.verbose)
		std::cout << os.str().c_str() << std::endl;
	if(!system_admin(config,os.str().c_str()))	return -1;

	return 1;
}


int enableUSBdevice(sim_config_t config, std::string usbdevice, std::string usbport)
{
	if(config.verbose)
		std::cout << "Enabling usb device: " << usbdevice << " (port: " << usbport << ")" << std::endl;

	std::ostringstream os;
	os << "echo " << "''" << usbport <<"'' | sudo -S tee /sys/bus/usb/drivers/usb/bind > tmp.txt";

	if(config.verbose)
		std::cout << os.str().c_str() << std::endl;
	if(!system_admin(config,os.str().c_str()))	return -1;

	return 1;
}


int enableTestConditions(sim_config_t config)
{
	//always set the same system settings for calibration
	if(!system("clear")) return -1;//LCD brightness

	//LCD brightness
	if(!system_admin(config,"sudo -S acpitool -l 1 > tmp.tmp"))	return -1;

	//CPU settings (we do consider a multicore CPU)
	//int numCPU = sysconf(_SC_NPROCESSORS_ONLN); //number of processors online
	int numCPU = sysconf(_SC_NPROCESSORS_CONF); //number of processors the operating system configured
	if (config.verbose)
		std::cout << "Disabling " << numCPU-1 << " CPUs" << std::endl;
	for (int i=1;i<numCPU;i++)
	{
		std::ostringstream os;
		os << "echo 0 | sudo -S tee /sys/devices/system/cpu/cpu" << i << "/online > tmp.tmp";
		if(!system_admin(config,os.str().c_str())) return -1; //disable CPU ith
	}
	std::cout << "Setting frequency of CPU0 to 1.2GHz" << std::endl;
	if(!system_admin(config,"sudo -S cpufreq-set -g userspace"))	return -1;//enable CPU governor specified by user

	std::ostringstream os;
	os << "sudo -S cpufreq-set -f 1000MHz";
	if(!system_admin(config,os.str().c_str())) return -1;

	if(system("sudo -S cpufreq-info | grep -i \"Analyzing CPU\"")) return -1;

	//Network settings
	if (config.verbose)
		std::cout << "Disabling all network interfaces (eth0, wlan0, ra0,...)" << std::endl;
	if(!system_admin(config,"sudo -S ifconfig eth0 down"))	return -1;//disable eth0
	if(!system_admin(config,"sudo -S ifconfig wlan0 down"))	return -1;//disable wlan0
	if(!system_admin(config,"sudo -S stop network-manager")) return-1; //disable network manager

	if(system("rfkill block bluetooth")) return -1;//disable bluetooh

	return 1;
}

int disableTestConditions(sim_config_t config)
{
	if (config.verbose)
		std::cout << "disableTestConditions()" << std::endl;

	//always set the same system settings for calibration
	if(!system_admin(config,"sudo -S acpitool -l 7 > tmp.tmp"))	return -1;//LCD brightness
	if(!system_admin(config,"sudo -S ifconfig wlan0 up"))	return -1;//enable wlan0
	if(!system_admin(config,"sudo -S ifconfig eth0 up"))	return -1;//enable eth0
	if(!system_admin(config,"sudo -S start network-manager"))	return -1;//enable network manager
	if(!system_admin(config,"sudo -S cpufreq-set -g ondemand"))	return -1;//change CPU governor

	int numCPU = sysconf(_SC_NPROCESSORS_CONF); //number of processors the operating system configured
	if (config.verbose)
		std::cout << "Enabling " << numCPU-1 << " CPUs" << std::endl;
	for (int i=1;i<numCPU;i++)
	{
		std::ostringstream os;
		os << "echo 1 | sudo -S tee /sys/devices/system/cpu/cpu" << i << "/online > tmp.tmp";
		if(!system_admin(config,os.str().c_str())) return -1; //disable CPU ith
	}

	//enableUSBdevice(config.usbdevice,config.usbport);
	return 1;
}

int printhelp(){

	return 1;
}
int parseCmdOptions(int argc, char *argv[], sim_config_t& config)
{
	char aux[256];
	PropertyFileReader configreader("config.ini");

	//sensing - configuration
	configreader.getProperty("SEN_CAMID", &(config.camID));
	configreader.getProperty("SEN_WIDTH", &(config.width));
	configreader.getProperty("SEN_HEIGHT", &(config.height));
	configreader.getProperty("SEN_FPS", &(config.fps));

	//processing - configuration (NOT USED)
	//configreader.getProperty("PRO_FREQ", &(config.freq));

	//comms - configuration (NOT USED)
	//configreader.getProperty("COM_MaxBITRATE", &(config.maxBitrate));
	//configreader.getProperty("COM_INTERFACE", aux); //network interface
	//config.interface=aux;
	//configreader.getProperty("COM_HOSTIP", aux); //IP for the appvideo_server
	//config.hostIP=aux;
	//configreader.getProperty("COM_HOSTPORT", &(config.hostPort)); //Port for the appvideo_server

	//simulation settings
	configreader.getProperty("POWER_MEAS_REPEAT", &(config.repeat));
	configreader.getProperty("POWER_MEAS_TIME", &(config.time));
	configreader.getProperty("POWER_MEAS_INITTIME", &(config.initialwait));
	configreader.getProperty("POWER_MEAS_SAMPLES", &(config.samples));

	//system settings
	configreader.getProperty("ADMIN_PASSWORD", aux);
	config.adminPass=aux;

	config.filesave="result.dat";

	int tmp;
	configreader.getProperty("VERBOSE", &tmp);
	config.verbose=(bool)tmp;

	config.capture = true;
	config.wait = true;
	config.usbmeter = false;
	config.disableUSB = false;

	config.full_test = false;
	config.single_test = false;
	config.calibrate = false;

	if(cmdOptionExists(argv, argv+argc, "-help"))
		printhelp();

	if(cmdOptionExists(argv, argv+argc, "-r"))
		config.repeat = atof(getCmdOption(argv, argv + argc, "-r"));

	if(cmdOptionExists(argv, argv+argc, "-t"))
		config.time = atof(getCmdOption(argv, argv + argc, "-t"));

	if(cmdOptionExists(argv, argv+argc, "-w"))
		config.width = atoi(getCmdOption(argv, argv + argc, "-w"));

	if(cmdOptionExists(argv, argv+argc, "-h"))
		config.height = atof(getCmdOption(argv, argv + argc, "-h"));

	if(cmdOptionExists(argv, argv+argc, "-fps"))
		config.fps = atof(getCmdOption(argv, argv + argc, "-fps"));

	if(cmdOptionExists(argv, argv+argc, "-i"))
		config.camID = atof(getCmdOption(argv, argv + argc, "-i"));

	if(cmdOptionExists(argv, argv+argc, "-n"))
		config.samples = atof(getCmdOption(argv, argv + argc, "-n"));

	if(cmdOptionExists(argv, argv+argc, "-o"))
		config.filesave=getCmdOption(argv, argv + argc, "-o");

	if(cmdOptionExists(argv, argv+argc, "-v"))
			config.verbose = true;

	if(cmdOptionExists(argv, argv+argc, "-c"))
		config.calibrate = true;

	if(cmdOptionExists(argv, argv+argc, "-s"))
		config.single_test = true;

	if(cmdOptionExists(argv, argv+argc, "-f"))
		config.full_test = true;

	if(cmdOptionExists(argv, argv+argc, "-usb"))
		config.usbmeter = true;

	if(cmdOptionExists(argv, argv+argc, "-dusb"))
		config.disableUSB = true;

	if(cmdOptionExists(argv, argv+argc, "-nc"))
		config.capture = false;

	if(cmdOptionExists(argv, argv+argc, "-nw"))
		config.wait = false;

	if(cmdOptionExists(argv, argv+argc, "-iw"))
		config.initialwait = atof(getCmdOption(argv, argv + argc, "-iw"));

	if (!config.full_test && !config.single_test && !config.calibrate){
		config.calibrate=true;
		std::cout << "No mode selected - doing calibration (default)" << std::endl;
	}

	return 1;
}

char* getCmdOption(char ** begin, char ** end, const std::string& option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

int create_output_file(sim_config_t config, std::string name,std::ofstream& outfile)
{
	if (!outfile.is_open()){

		if (config.filesave.size() > 0)
			outfile.open(config.filesave.c_str());
		else
		{
			std::ostringstream os;
			os << name << "_" <<config.width<<"x"<<config.height;
			if (config.capture==false)
				os << "_nc";
			if (config.wait==false)
				os << "_nw";
			os << ".dat";
			outfile.open(os.str().c_str());
		}
	}

	return 1;
}

//http://www.dreamincode.net/forums/topic/148707-introduction-to-using-libusb-10/
int list_all_usb_devices() {

	libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
	libusb_context *ctx = NULL; //a libusb session
	int r; //for return values
	ssize_t cnt; //holding number of devices in list
	r = libusb_init(&ctx); //initialize a library session
	if(r < 0) {
		std::cout<<"Init Error "<<r<<std::endl; //there was an error
				return 1;
	}

	libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation
	cnt = libusb_get_device_list(ctx, &devs); //get the list of devices

	if(cnt < 0)
		std::cout<<"Get Device Error"<<std::endl; //there was an error

	std::cout<<cnt<<" Devices in list."<<std::endl; //print total number of usb devices
	ssize_t i; //for iterating through the list

	for(i = 0; i < cnt; i++)
		printdev(devs[i]); //print specs of this device

		libusb_free_device_list(devs, 1); //free the list, unref the devices in it
		libusb_exit(ctx); //close the session

		return 0;
}

void printdev(libusb_device *dev) {

	libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor(dev, &desc);
	if (r < 0) {
		std::cout<<"failed to get device descriptor"<<std::endl;
		return;
	}
	std::cout<<"Number of possible configurations: "<<(int)desc.bNumConfigurations<<"  ";
	std::cout<<"Device Class: "<<(int)desc.bDeviceClass<<"  ";
	std::cout<<"VendorID: "<< desc.idVendor<<"(int) ";
	std::cout<<"ProductID: "<<desc.idProduct<<"(int)"<<std::endl;
	//std::cout<<"Address: "<<libusb_get_device_address(dev)<<std::endl;
	//std::cout<<"Bus number: "<<libusb_get_bus_number(dev)<<std::endl;

	std::cout << "as hex-> "<<std::hex << std::setfill('0') << std::setw(4)<< desc.idVendor<<":";
	std::cout <<std::hex << std::setfill('0') << std::setw(4)<< std::hex <<desc.idProduct;
	printf("(bus %d, device %d)\n",libusb_get_bus_number(dev), libusb_get_device_address(dev));

	uint8_t path[8];
	r = libusb_get_port_numbers(dev, path, sizeof(path));
	if (r > 0) {
		printf(" path: %d", path[0]);
		for (int j = 1; j < r; j++)
			printf(".%d", path[j]);
	}
	printf("\n");

/*
	libusb_config_descriptor *config;
	libusb_get_config_descriptor(dev, 0, &config);
	std::cout<<"Interfaces: "<<(int)config->bNumInterfaces<<" ||| ";

	const libusb_interface *inter;
	const libusb_interface_descriptor *interdesc;
	const libusb_endpoint_descriptor *epdesc;

	for(int i=0; i<(int)config->bNumInterfaces; i++) {
		inter = &config->interface[i];
		std::cout<<"Number of alternate settings: "<<inter->num_altsetting<<" | ";
		for(int j=0; j<inter->num_altsetting; j++) {
			interdesc = &inter->altsetting[j];
			std::cout<<"Interface Number: "<<(int)interdesc->bInterfaceNumber<<" | ";
			std::cout<<"Number of endpoints: "<<(int)interdesc->bNumEndpoints<<" | ";

			for(int k=0; k<(int)interdesc->bNumEndpoints; k++) {
				epdesc = &interdesc->endpoint[k];
				std::cout<<"Descriptor Type: "<<(int)epdesc->bDescriptorType<<" | ";
				std::cout<<"EP Address: "<<(int)epdesc->bEndpointAddress<<" | ";
			}
		}
	}
	std::cout<<std::endl<<std::endl<<std::endl;
	libusb_free_config_descriptor(config);
	*/
}

std::string getFullPortUSB(std::string id) {

	std::string port;

	libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
	libusb_context *ctx = NULL; //a libusb session
	int r; //for return values
	ssize_t cnt; //holding number of devices in list
	r = libusb_init(&ctx); //initialize a library session
	if(r < 0) {
		std::cout<<"Init Error "<<r<<std::endl; //there was an error
		return NULL;
	}

	libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation
	cnt = libusb_get_device_list(ctx, &devs); //get the list of devices

	if(cnt < 0)
		std::cout<<"Get Device Error"<<std::endl; //there was an error

	std::cout<<cnt<<" Devices in list."<<std::endl; //print total number of usb devices

	for(ssize_t i = 0; i < cnt; i++)
	{
		std::string tmpid = get_device_id(devs[i]);
		if (tmpid.compare(id)==0)
		{
			port = get_device_port(devs[i]);
			std::cout << " usb device " << tmpid << " found with port=" << port<< std::endl;
		}
	}

	libusb_free_device_list(devs, 1); //free the list, unref the devices in it
	libusb_exit(ctx); //close the session

	return port;
}

std::string get_device_id(libusb_device *dev)
{
	libusb_device_descriptor desc;
	int r = libusb_get_device_descriptor(dev, &desc);
	if (r < 0) {
		std::cout<<"failed to get device descriptor"<<std::endl;
		return NULL;
	}
	std::ostringstream os;
	os << std::hex << std::setfill('0') << std::setw(4)<< desc.idVendor<<":";
	os << std::hex << std::setfill('0') << std::setw(4)<< desc.idProduct;

	return os.str();
}

std::string get_device_port(libusb_device *dev)
	{

	int bus = libusb_get_bus_number(dev);
	//int address = libusb_get_device_address(dev);

	std::ostringstream os;
	os << bus << "-";
	uint8_t path[8];
	int r = libusb_get_port_numbers(dev, path, sizeof(path));
	if (r > 0) {
		//printf(" path: %d", path[0]);
		os << (int)path[0];
		for (int j = 1; j < r; j++)
			os << "." << (int)path[j];
			//printf(".%d", path[j]);
	}
	return os.str();
}
