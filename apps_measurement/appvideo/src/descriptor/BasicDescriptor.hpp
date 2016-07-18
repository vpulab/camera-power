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

#ifndef BASICDESCRIPTOR_H_INCLUDE
#define BASICDESCRIPTOR_H_INCLUDE

#include <opencv2/opencv.hpp>
#include <BasicBlob.hpp>

// Class Definition
class BasicDescriptor {
public:
	BasicDescriptor();
	~BasicDescriptor();

	int sock;
	cv::Mat allDescRGB; //concatenate all RGB histograms for a blob

	std::stringstream datain1, datatx1;
	std::stringstream datain2, datatx2;

    std::vector<cv::Mat> _GKernelList; //!< List of kernels for Gabor Features
    bool _extractGabor;

	int start(std::string host, int port);
	void generateDescriptors(int frameID, double timestamp,cv::Mat &frame, std::vector<cvBlob> &blobs);
	void compressDescriptors();
	int sendDescriptors(int &send_bytes, int& recv_bytes);
};
#endif
