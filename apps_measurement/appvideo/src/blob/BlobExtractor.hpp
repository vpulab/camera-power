//file description
/**
* \file BlobExtractor.hpp
* \author Juan Carlos San Miguel Avedillo (jcs)
* \date 21-05-08
* \brief Class to describe a blob extractor from foreground/background masks \n
*		 This file is the header of the BlobExtractor class.
* \version 1.0\n
*			Version history:\n
*				- 1.0(21/05/08): Initial Implementation (jcs)
*				- 2.0(21/02/16): C++ Implementation - removed depedencies with BlobList (jcs)
*
*/	

//class description
/**
 * \class BlobExtractor
 * \brief Extraction of Blobs from binary masks
 *
 * .
 */
#ifndef BLOBEXTRACTOR_H_INCLUDE
#define	BLOBEXTRACTOR_H_INCLUDE

 /**
 *	\def MAX_BLOBS
 *	Default maximum number of blobs to detect
 */
const int MAX_BLOBS = 1000;

/**
 *	\def MIN_WIDTH_DEF
 *	Default minimum width for the blob filtering stage
 */
const int MIN_WIDTH_DEF = 5;

/**
 *	\def MIN_HEIGHT_DEF
 *	Default minimum height for the blob filtering stage
 */
const int MIN_HEIGHT_DEF = 5;

/**
 *	\def MAX_HEIGHT_DEF
 *	Default maximum height for the blob filtering stage
 */
const int MAX_HEIGHT_DEF = 10000;

/**
 *	\def MAX_WIDTH_DEF
*	Default maximum width for the blob filtering stage
 */
const int MAX_WIDTH_DEF = 10000;

/**
 *	\def FILTERING_DEF
 *	Default state of the filtering process
 */
const int FILTERING_DEF = 1;

#include "BasicBlob.hpp"

//Blob Extractor class
class BlobExtractor {
private:	
	cv::Mat label_mask, label_image;
	int minWidth, minHeight, maxWidth, maxHeight;
	int NumBlobs;
	//std::vector<BasicBlob *> pBlobList;
	std::vector<cvBlob> pBlobList;
	bool enable_filtering;

public:
	// Class constructor
	BlobExtractor();

	// Class destructor
	virtual ~BlobExtractor();

	// Blob extraction from an image
	int extractBlobs(cv::Mat &mask, bool extractMask, const cv::Mat *operationMask=NULL);

	// Method to get the internal blob's list
	//std::vector<BasicBlob*>* getBlobList();
	std::vector<cvBlob>* getBlobList();

	// Number of blobs extracted
	int getNumBlobs();

		// Set blob filtering parameters (size constraint)
	void setBlobFilter(int _minWidth=MIN_WIDTH_DEF,int _minHeight=MIN_HEIGHT_DEF,int _maxWidth=MAX_WIDTH_DEF,int _maxHeight=MAX_HEIGHT_DEF);

	// Activate/Deactivate blob filtering
	void setBlobFilterState(bool state);

	cv::Mat &getLabelImage();

	// Number of blobs extracted
	cv::Mat &getBlobBoundingBoxMask(cv::Mat &img);

	// Prints some features of all the blobs in a file
	virtual int PrintBlobs(char *filename);

	// Prints some features on the system console
	void showBlobInfo();
};
#endif
