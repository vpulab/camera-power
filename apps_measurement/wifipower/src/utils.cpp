#include <vector>	// std::vector
#include <fstream>	// std::ostringstream
#include <iostream> // std::cout, std::endl
#include <iomanip> 	// std::setfill, std::setw, std::setprecision
#include <numeric> 	// std::accumulate, std::inner_product

#include <stdlib.h>    // system, NULL, EXIT_FAILURE
#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write, std::sleep

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

	stats.maxBitrate = config.maxBitrate;
	stats.pktSize = config.dataPkt;
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

int test_wifi(sim_config_t config, battery_reads_cam_t &stats)
{
	char message[BUFFER_SIZE], server_reply[BUFFER_SIZE];

	double tmp,tacc=0;
	double time = config.time;
	double samples = config.samples;
	
	if (config.testTX)
		stats.name = "wifiTX";
	if (config.testRX)
		stats.name = "wifiRX";
		
	stats.maxBitrate = config.maxBitrate;
	stats.pktSize = config.dataPkt;
	stats.npackets=0;

	stats.time = time;
	stats.samples = samples;

	stats.n_reads=0;
	stats.energy.clear();
	stats.voltage.clear();
	stats.power.clear();
	
	stats.start_time = (double)getTickCount()/getTickFrequency();
	
	std::cout << "Waiting " << config.initialwait << " secs to get measurements..." << std::endl;
	usleep(1e6*config.initialwait);

	 int sock;
	 struct sockaddr_in server;

	 //Create socket
	 sock = socket(AF_INET , SOCK_STREAM , 0);
	 if (sock == -1)
		 printf("Could not create socket");
	 //puts("Socket created");

	 server.sin_addr.s_addr = inet_addr( config.hostIP.c_str() );
	 server.sin_family = AF_INET;
	 server.sin_port = htons( config.hostPort );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0){
		perror("connect failed. Error");
		return -1;
	}
	else
		std::cout << "Connected to host " << config.hostIP << " using port=" << config.hostPort << std::endl;

	//inform the mode (RX or TX)
	std::cout << "Inform server of mode TX=" << config.testTX << " RX=" << config.testRX << " ( " << config.dataPkt << " B/packet)";

	if (config.testTX)
		message[0]='t';
	if (config.testRX)
		message[0]='r';

	if( send(sock , message , 1 , 0) < 0) {
	        puts("Send failed");
	        return -1;
	}
	recv(sock , server_reply , BUFFER_SIZE , 0);
	std::cout << " --> Server Reply " << server_reply << std::endl;

	if (config.testRX)
	{
		sprintf(message, "%d",stats.pktSize);
		if( send(sock , message , strlen(message) , 0) < 0) {
			        puts("Send failed");
			        return -1;
		}
	}

	std::cout << "Getting measurements:" << std::endl;
	bool keep = true;
	int npackets=0;
	while (keep)
	{
		double t1 = (double)getTickCount()/getTickFrequency();
		if (config.testTX)
			tx_data(sock,stats.pktSize);
		if (config.testRX)
			rx_data(sock,stats.pktSize);
		npackets++;
		double t2 = (double)getTickCount()/getTickFrequency();
		tacc = tacc+t2-t1;
		//std::cout << "npacket=" << npackets << " ( "<< tacc*1000 << " ms)" << std::endl;

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
			stats.npackets = stats.npackets + npackets;
			npackets=0;
			if(stats.n_reads == samples)
				keep = false;
		}
	}

    close(sock);
	stats.end_time = (double)getTickCount()/getTickFrequency();
	return 1;
}

int printStats(battery_reads_cam_t stats)
{
	std::cout << "stats\n\t config: Max bitrate=" <<stats.maxBitrate << " pktSize=" <<stats.pktSize << std::endl;

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
	
	outfile << "iniTX="<< stats.iniTX << std::endl;
	outfile << "endTX="<< stats.endTX << std::endl;
	outfile << "iniRX="<< stats.iniRX << std::endl;
	outfile << "endRX="<< stats.endRX << std::endl;

	outfile << std::setprecision(4) << "MaxBitrate=" << stats.maxBitrate << std::endl;
	outfile << std::setprecision(4) << "nPackets=" << stats.npackets << std::endl;
	outfile << std::setprecision(4) << "pktSize=" << stats.pktSize << std::endl;
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
