/**
 * \file BasicBlob.hpp 
 * \author Juan Carlos San Miguel Avedillo (jcs)
 * \date 21/02/16		
 * \brief Implementation of basic blob structure
 * \version 2.0 \n
 *			Version history:\n
 *				- 1.0 (22/11/07): Initial Implementation C (vfc)
 *				- 1.1 (22/05/08): Modification of the write method (jcs)
 *				- 2.0 (21/02/16): standalone C version - removed depedency with OpenCV (jcs)
 *
 */

//class description
/**
 * \class BasicBlob
 * \brief Class to describe a basic blob
 *
 * 
 */

#ifndef BASICBLOB_H_INCLUDE
#define BASICBLOB_H_INCLUDE

#include "opencv2/opencv.hpp"

// Maximun number of char in the blob's format
const int MAX_FORMAT = 1024;

struct cvBlob {
	int     ID;  /* blob ID         */
	int   x, y; /* blob position   */
	int   w, h; /* blob sizes      */
	char format[MAX_FORMAT];
	//cv::Mat blobMask; //not used
};

inline cvBlob initBlob(int id, int x, int y, int w, int h)
{
	cvBlob B = { id,x,y,w,h };
	return B;
}
#endif
