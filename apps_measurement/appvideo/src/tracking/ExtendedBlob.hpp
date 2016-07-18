/**
 * \file CountBlob.h 
 * \author Victor Fernandez-Carbajales(vfc)
 * \date 20-05-08		
 * \brief Class to describe a new type of blob description \n
 *		 This file is the header of the CountBlob class.
 * \version 1.0\n
 *			Version history:\n
 *				- 1.0(?): Initial Implementation (vfc)
 *
 */

//class description
/**
 * \class CountBlob
 * \brief Class to describe a blob with information of its life count and blob trayectories
 *
 * 
 */

#ifndef EXTENDEDBLOB_H_INCLUDE
#define EXTENDEDBLOB_H_INCLUDE

// Additional includes
#include "BasicBlob.hpp"
#include <vector>

// Necessary namespace
using namespace std;

// Default max value of blob's trayectories
const int MAX_TRAYECTORIES = 25;

// Class Definition
class ExtendedBlob: public cvBlob {
public:
	//representative blob color to associate to its ID 
	cv::Scalar IDcolor;
	
	// Life count of the blob
	int life_count;
	int matches;

	// Flag to indicate whether the blob is active or not
	bool active;
	// Blob's trayectories
	std::vector<cvBlob> blobTrayec;
	// Max number of blob's trayectories
	int maxTray;

	//mean blob colour
	cv::Scalar meanColor; 

	//mean motion vector
	cv::Point meanMV;

public:
	// Default constructor
	ExtendedBlob();

	ExtendedBlob(cvBlob blob);

	// Default destructor
	virtual ~ExtendedBlob();

	// Method to get the life count of blob
	int	 getCount();
	// Method to get the blob state
	bool getAct();
	// Method to get the blob's trayectories
	vector<cvBlob> &getBlobTrayec();
	// Method to get a blob trayectory
	cvBlob &getBlobTrayec(int n);
	// Method to get the maximun number of blob's trayectories
	int getMaxTray();
	// Method to set the blob's trayectories
	void setBlobTrayec(vector<cvBlob> &trayec);
	// Method to set a blob trayectory
	void setBlobTrayec(int n, cvBlob &blob);
	// Method to set the life count of the blob
	void setCount(int count);
	// Method to set the maximun number of trayectories
	int setMaxTray(int max);

	// Useful Methods
	// Method to increase the life count
	//void incrCount();
	// Method to decrease the life count
	//void decrCount();
	// Method to deny the state of the blob
	//void negAct();
	// Method to afirmate the state of the blob
	//void affAct();

	// Method to add a new blob trayectory
	void addBlobTrayectory(cvBlob &newTrayectory);
	// Method to erase a old blob trayectory
	void eraseBlobTrayectory(int index);

	ExtendedBlob *clone();

	void update(ExtendedBlob &blob);
};

#endif
