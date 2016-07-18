/**
 * \file BasicBlobTracker.h 
 * \author Victor Fernandez-Carbajales(vfc)
 * \date 20-05-08		
 * \brief Class to describe a basic blob tracker \n
 *		 This file is the header of the BasicBlobTracker class.
 * \version 1.0\n
 *			Version history:\n
 *				- 1.0(?): Initial Implementation (vfc)
 *
 */

//class description
/**
 * \class BasicBlobTracker
 * \brief Class to describe the most basic blob tracker
 *
 * 
 */
#ifndef BASICBLOBTRACKER_H
#define BASICBLOBTRACKER_H

// Additional includes
#include "ExtendedBlob.hpp"
#include "BlobList.hpp"
#include <algorithm>

// Constanst values of the class
// Max number of life count
const int MAX_VALUE = 10;

// Max number of life count
const int MIN_LIFE = 5;

// Threshold of height difference
const int UMBRAL_H = 30;
// Threshold of width difference
const double THRESHOLD_DIST = 0.75;
// Threshold of colour difference
const int THRESHOLD_COLOR = 50;
// Basic value of maximun distancia of searchs
const double RADIO = 3.0;
// The most high value of the ID
const int MAX_ID = 60000;
// Constant values of random colour
const int IN0 = 0;
const int IN1 = 85;
const int IN2 = 170;
const int IN3 = 255;

// Definition of pair value (distance and Index)
typedef pair<double,int> parDistanciaIndice;

// Class Definition
class BasicBlobTracker {
private:
	int max_life_no_matches; //maximum number of consecutive no matches
	int min_lifespan; //minimum number of matches to appear
	int nFrame;  // Number of process frames

	// List of blobs tracked
	std::vector<cvBlob> outputBlobs;
	BlobList<ExtendedBlob*> blobListNew;
	BlobList<ExtendedBlob*> blobListCurr;

	// Result Image of the tracking
	cv::Mat tracking_result;

	int id_counter;

public:
	// Default constructor
	BasicBlobTracker();
	// Default destructor
	virtual ~BasicBlobTracker();

	// Main method to process one frame
	int processFrame(cv::Mat &frame, cv::Mat &mask, std::vector<cvBlob> &blobs);

	//Method to get the tracked list of blobs
	BlobList<ExtendedBlob*> &getListExtendedBlob();
	std::vector<cvBlob> &getBlobList();

	// Method to get the output image of the tracker
	cv::Mat &getResultImage(cv::Mat &frame);

	//Method to get the list of new blobs in the current frame
	//BlobList<ColourBlob*>* getListNewBlob();
	//Method to get the output list of blobs
	//BlobList<CountBlob*>* getOutputListBlob();
	
	//Method to set the tracked list of blobs
	//void setListBlob(BlobList<ColourBlob*>* blobs);
	//Method to set the list of new blobs in the current frame
	//void setListNewBlob(BlobList<ColourBlob*>* blobs);
	//Method to set the output list of blobs
	//void setOutputListBlob(BlobList<CountBlob*>* blobs);

	// Method to converse the basicblob's list to colourblob's list
	void convertBlobToExtended(cv::Mat &frame, std::vector<cvBlob> &blobs);
	// Method to converse the colourblob's list to countblob's list
	//BlobList<ExtendedBlob*>* OutputConversionBlob();

	// Method to calculate distances or differences
	double distance_color(cv::Scalar c1, cv::Scalar c2);


	// Method to paint the result of the tracker
	//void paintBlobs(IplImage* pImg);

};

#endif