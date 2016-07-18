/**
* \file BasicBlob.cpp
* \author Victor Fernandez-Carbajales (vfc)
* \author Juan Carlos San Miguel Avedillo (jcs)
* \date 21/02/16
* \brief Implementation of BlobExtractor class
* \version 2.0 \n
*			Version history:\n
*				- 1.0 (22/11/07): Initial Implementation C (vfc)
*				- 1.1 (22/05/08): Modification of the write method (jcs)
*				- 2.0 (21/02/16): standalone C++ version - removed depedency with old OpenCV structure 'CvBlob' (jcs)
*
*/

//Additional includes
#include "BasicDescriptor.hpp"
#include "utils.hpp"

#include <iostream>
#include <stdio.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write

/**
 *	Standard class Constructor. 
 *
 */
BasicDescriptor::BasicDescriptor() {
	sock=-1;
	_extractGabor=false;
	//BasicDescriptor("127.0.0.1",8888);

	if (_extractGabor){
		int ksize = 31;
		double sig[] = {1,2};
		double th[] = {0, 3.14/2};
		double lm[] = {4,8};
		double gm[] = {0.3, 0.4}; //0.02
		double ps = 0;

		cv::Mat kernel1 = cv::getGaborKernel(cv::Size(ksize,ksize), sig[1], th[0], lm[0], gm[1], ps);
		cv::Mat kernel2 = cv::getGaborKernel(cv::Size(ksize,ksize), sig[0], th[0], lm[1], gm[0], ps);
		cv::Mat kernel3 = cv::getGaborKernel(cv::Size(ksize,ksize), sig[0], th[0], lm[0], gm[0], ps);
		cv::Mat kernel4 = cv::getGaborKernel(cv::Size(ksize,ksize), sig[1], th[1], lm[0], gm[0], ps);
		cv::Mat kernel5 = cv::getGaborKernel(cv::Size(ksize,ksize), sig[0], th[1], lm[1], gm[0], ps);
		cv::Mat kernel6 = cv::getGaborKernel(cv::Size(ksize,ksize), sig[1], th[1], lm[1], gm[0], ps);
		cv::Mat kernel7 = cv::getGaborKernel(cv::Size(ksize,ksize), sig[0], th[1], lm[0], gm[1], ps);
		cv::Mat kernel8 = cv::getGaborKernel(cv::Size(ksize,ksize), sig[1], th[1], lm[1], gm[1], ps);
		_GKernelList.push_back(kernel1);
		_GKernelList.push_back(kernel2);
		_GKernelList.push_back(kernel3);
		_GKernelList.push_back(kernel4);
		_GKernelList.push_back(kernel5);
		_GKernelList.push_back(kernel6);
		_GKernelList.push_back(kernel7);
		_GKernelList.push_back(kernel8);
	}
}

/**
 *	Standard class destructor. 
 *
 */
BasicDescriptor::~BasicDescriptor() {

	close(sock);
}

int BasicDescriptor::start(std::string host, int port) {

	//Create the socket
	struct sockaddr_in server;
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");

	server.sin_addr.s_addr = inet_addr(host.c_str());
	server.sin_family = AF_INET;
	server.sin_port = htons( port );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return -1;
	}
	puts("Connected\n");
	return 1;
}

void BasicDescriptor::generateDescriptors(int frameId, double timestamp, cv::Mat &frame, std::vector<cvBlob> &blobs)
{
	//DESCRIPTOR PART 1
	//data serialization
	datain1.str(std::string());
	//std::cout << "Descriptor buffer with "<< datain1.str().size() << " bytes for " << (int)blobs.size() << " blobs" << std::endl;

	datain1.write((char*)(&frameId), sizeof(int));//frameID
	datain1.write((char*)(&timestamp), sizeof(double));//timestamp
	int numblobs=(int)blobs.size();
	datain1.write((char*)(&numblobs), sizeof(int));//numbblos

	for (int i=0;i<(int)blobs.size();i++)
	{
		datain1.write((char*)(&blobs.at(i).ID), sizeof(int));//frameID
		datain1.write((char*)(&blobs.at(i).x), sizeof(int));//frameID
		datain1.write((char*)(&blobs.at(i).y), sizeof(int));//frameID
		datain1.write((char*)(&blobs.at(i).w), sizeof(int));//frameID
		datain1.write((char*)(&blobs.at(i).h), sizeof(int));//frameID
	}
	//std::cout << "Generated descriptor 1 (serialized) with "<< datain1.str().size() << " bytes" << std::endl;

	//DESCRIPTOR PART 2
	if ((allDescRGB.cols * allDescRGB.rows) > 0)
		allDescRGB.release(); //release memory for next TX

	/// Establish the number of bins
	int histSize = 16; //typical value for people re-id

	 /// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };
	bool uniform = true; bool accumulate = false;

	std::vector<cv::Mat> bgr_planes; //R,G,B planes of images
	cv::split(frame, bgr_planes);

	cv::Mat frame_hsv;
	cv::cvtColor(frame, frame_hsv, CV_BGR2HSV);
	std::vector<cv::Mat> hsv_planes; //H,S,V planes of images
	cv::split(frame_hsv, hsv_planes);

	cv::Mat frame_ycrcb;
	cv::cvtColor(frame, frame_ycrcb, CV_BGR2YCrCb);
	std::vector<cv::Mat> ycrcb_planes; //H,S,V planes of images
	cv::split(frame_ycrcb, ycrcb_planes);

	//Process each blob
	for (int i = 0; i < (int)blobs.size(); i++)
	{
		cv::Point p1(blobs.at(i).x, blobs.at(i).y);
		cv::Point p2(blobs.at(i).x + blobs.at(i).w, blobs.at(i).y + blobs.at(i).h);
		cv::Rect roi = cv::Rect(p1, p2);

		cv::Mat mask = cv::Mat::zeros(frame.size(), cv::DataType<uchar>::type);  // type of mask is CV_8U
		mask(roi) = cv::Scalar(255, 255, 255);

		// Compute the RGB histograms:
		cv::Mat b_hist, g_hist, r_hist;
		cv::calcHist(&bgr_planes[0], 1, 0, mask, b_hist, 1, &histSize, &histRange, uniform, accumulate);
		cv::calcHist(&bgr_planes[1], 1, 0, mask, g_hist, 1, &histSize, &histRange, uniform, accumulate);
		cv::calcHist(&bgr_planes[2], 1, 0, mask, r_hist, 1, &histSize, &histRange, uniform, accumulate);

		// Concatenate the histograms
		if ((allDescRGB.cols * allDescRGB.rows) == 0)
			b_hist.copyTo(allDescRGB);
		else
			cv::vconcat(allDescRGB, b_hist, allDescRGB);
		cv::vconcat(allDescRGB, g_hist, allDescRGB);
		cv::vconcat(allDescRGB, r_hist, allDescRGB);

		// Compute the HSV histograms:
		cv::Mat h_hist,s_hist,v_hist;;
		cv::calcHist(&hsv_planes[0], 1, 0, mask, h_hist, 1, &histSize, &histRange, uniform, accumulate);
		cv::calcHist(&hsv_planes[1], 1, 0, mask, s_hist, 1, &histSize, &histRange, uniform, accumulate);
		cv::calcHist(&hsv_planes[2], 1, 0, mask, v_hist, 1, &histSize, &histRange, uniform, accumulate);

		cv::vconcat(allDescRGB, h_hist, allDescRGB);
		cv::vconcat(allDescRGB, s_hist, allDescRGB);
		cv::vconcat(allDescRGB, v_hist, allDescRGB);

		// Compute the HSV histograms:
		cv::Mat y_hist,cr_hist,cb_hist;;
		//cv::calcHist(&ycrcb_planes[0], 1, 0, mask, y_hist, 1, &histSize, &histRange, uniform, accumulate);
		cv::calcHist(&ycrcb_planes[1], 1, 0, mask, cr_hist, 1, &histSize, &histRange, uniform, accumulate);
		cv::calcHist(&ycrcb_planes[2], 1, 0, mask, cb_hist, 1, &histSize, &histRange, uniform, accumulate);

		//cv::vconcat(allDescRGB, h_hist, allDescRGB);
		cv::vconcat(allDescRGB, cr_hist, allDescRGB);
		cv::vconcat(allDescRGB, cb_hist, allDescRGB);

		// Compute the Gabor histograms:
		if (_extractGabor){
			// GABOR HISTOGRAM
			//Example at https://cvtuts.wordpress.com/2014/04/27/gabor-filters-a-practical-overview/
			cv::Mat greyMatf, greyMat;
			cv::cvtColor(frame, greyMat, cv::COLOR_BGR2GRAY);
			cv::Mat res,res_acc = cv::Mat::zeros(greyMat.size(), CV_32F);

			greyMat.convertTo(greyMatf,CV_32F);

			for (int k = 0; k<(int)_GKernelList.size()-2;k++){
				std::cout <<"applying gabor filter " << k <<std::endl;
				cv::filter2D(greyMatf, res, CV_32F, _GKernelList[k]);
				cv::max(res, res_acc, res_acc);
			}
			std::cout <<"Finished gabor filters" <<std::endl;


			// Compute the histogram
			cv::Mat gab_hist;
			cv::calcHist( &res_acc, 1, 0, mask, gab_hist, 1, &histSize, &histRange, uniform, accumulate );

			// Concatenate the histograms
			cv::vconcat(allDescRGB, gab_hist, allDescRGB);
		}
	}
	std::stringstream tmp;
	if((int)blobs.size()>0)
		serialize(allDescRGB, &tmp);
	datain2.str(tmp.str());
	//std::cout << "Generated descriptor 2 (serialized) with "<< datain2.str().length() << " bytes" << std::endl;
}

void BasicDescriptor::compressDescriptors()
{
	//int encBits=0;
	//no compression for the first packet
	//datatx1.str(datain1.str());
	//std::cout << "Generated descriptor 1 (compressed) with "<< datatx1.str().length() << " bytes" << std::endl;

	//std::stringstream in;in.str(datain2.str());
	//std::stringstream out;

	//if(datain2.str().size() > 0)
	//	encBits=encode_huffman(in, out);
	//datatx2.str(out.str());

	datatx1.str(datain1.str());
	datatx2.str(datain2.str());

	//std::cout << "Generated descriptor 1 (compressed) with "<< datatx1.str().length() << " bytes" << std::endl;
	//std::cout << "Generated descriptor 2 (compressed) with "<< datatx2.str().length() << " bytes" << std::endl;
}

int BasicDescriptor::sendDescriptors(int &send_bytes, int& recv_bytes)
{
	char server_reply[2000];
	int recv_bytes_tmp=0;
	send_bytes=0;
	recv_bytes=0;

	//compose a unique stream to be sent
	datatx1 << datatx1.str() << datatx2.str();

	//Send some data
	//std::cout << "Sending " << datatx1.str().length() << " bytes to server" << std::endl;
	if( send(sock , datatx1.str().c_str(), datatx1.str().length() , 0) < 0)
	{
		puts("Send failed");
		return -1;
	}

	/*//Receive a reply from the server
	if( (recv_bytes_tmp=recv(sock , server_reply , 2000 , 0)) < 0)
	{
		puts("recv failed");
		return -1;
	}*/
	send_bytes=datatx1.str().length();
	return 0;
}
