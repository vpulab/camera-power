//file description
/**
 * \file BasicBlobTracker.cpp 
 * \author Victor Fernandez-Carbajales(vfc)
 * \date 20-05-08		
 * \brief Implementation of BasicBlobTracker class
 * \version 1.1\n
 *			Version history:\n
 *				- 1.0(?): Initial Implementation (vfc)
 *
 */

// Addicional includes
#include "BasicBlobTracker.hpp"
#include "hungarian.h"

#include <iostream> // std::cout, std::endl

/**
 *	Standard class Constructor. 
 *
 */
BasicBlobTracker::BasicBlobTracker() {
	//this->listBlobs = new BlobList<ColourBlob*>;
	//this->listNewBlobs = NULL;
	//this->outputListBlobs = NULL;
	//this->resultImage = NULL;
	this->nFrame = 0;
	this->id_counter = 1;
	this->min_lifespan = 0;
}

/**
 *	Standard class destructor
 *
 */
BasicBlobTracker::~BasicBlobTracker() {
	this->blobListCurr.clear();
	this->blobListNew.clear();

}

/**
*	Main method to process one frame
*
* \param pImg The input frame to process
* \param pFG  The background segmentation result
* \param blobs The input blob's list to process
* \return int Error code
*
*/
int BasicBlobTracker::processFrame(cv::Mat &frame, cv::Mat &mask, std::vector<cvBlob> &blobs) {

	this->nFrame++;
	
	// 0. Eliminate all previous blobs
	blobListNew.clear();

	// 1. Input blobs conversion
	//std::cout << " convertBlobToExtended " << std::endl;
	convertBlobToExtended(frame, blobs);

	// 2. If blobListCurr is empty, fill with new blobs
	if (blobListCurr.getBlobNum() == 0)
		for (int i = 0; i < blobListNew.getBlobNum(); i++) {
			ExtendedBlob *pBlob = blobListNew.getBlob(i)->clone();
			pBlob->ID = this->id_counter++;
			pBlob->IDcolor = cv::Scalar((IN0*rand()) % 256, (IN1*rand()) % 256, (IN2*rand()) % 256, (IN3*rand()) % 256);
			blobListCurr.addBlob(pBlob);
		}

	//3. Determine correspondences between blobs in blobListCurr & blobListNew
	//std::cout << "Determine correspondences between blobs in blobListCurr & blobListNew	" << std::endl;
	cv::Mat dist = cv::Mat::zeros(blobListCurr.getBlobNum(), blobListNew.getBlobNum(), CV_64F); //variable for target-measurement distance
	cv::Mat asoc = cv::Mat::zeros(blobListCurr.getBlobNum(), blobListNew.getBlobNum(), CV_64F);
	cv::Mat cost = cv::Mat::zeros(blobListCurr.getBlobNum(), blobListNew.getBlobNum(), CV_64F);

	for (int i = 0; i < blobListCurr.getBlobNum(); i++) 
		for (int j = 0; j < blobListNew.getBlobNum(); j++) {
			ExtendedBlob *pCur = blobListCurr.getBlob(i);
			ExtendedBlob *pNew = blobListNew.getBlob(j);

			//find the intersecting area between the current and new blobs
			cv::Rect r1 = cv::Rect(cv::Point(pCur->x,pCur->y), cv::Point(pCur->x+pCur->w,pCur->y+pCur->h));
			cv::Rect r2 = cv::Rect(cv::Point(pNew->x,pNew->y), cv::Point(pNew->x+pNew->w,pNew->y+pNew->h));
			cv::Rect  intersect = r1 & r2;
			dist.at<double>(i, j) = 1 - (2 * (double)intersect.area()) / ((double)r1.area() + (double)r2.area());

	}
	//COMPUTE assignment using Hungarian algorithm to find best matching
	//std::cout << "COMPUTE assignment using Hungarian algorithm to find best matching" << std::endl;
	hungarian(dist, &asoc, &cost);
	//std::cout << "dist=" << dist << " matches=" << asoc << " cost=" << cost << endl;

	//4. Update correspondences if colour difference is low
	//std::cout << " Update correspondences " << std::endl;
	cv::Mat newassigned = cv::Mat::zeros(blobListNew.getBlobNum(), 1, CV_8U); //variable for target-measurement distance

	for (int i = 0; i < blobListCurr.getBlobNum(); i++) {
		ExtendedBlob *pCur = blobListCurr.getBlob(i);
		pCur->active = false;
		for (int j = 0; j < blobListNew.getBlobNum(); j++)
			if (asoc.at<double>(i, j) == 1 && dist.at<double>(i, j) < THRESHOLD_DIST)
			{				
				ExtendedBlob *pNew = blobListNew.getBlob(j);
				if (distance_color(pCur->meanColor, pNew->meanColor) < THRESHOLD_COLOR) {
					cvBlob trajectory = initBlob(pCur->ID, pCur->x, pCur->y, pCur->w, pCur->h);
					pCur->addBlobTrayectory(trajectory);
					pCur->update(*pNew); //also updates 'matches' and 'active' variables of the blob					
					pCur->life_count = MAX_VALUE;
					pCur->meanColor = pNew->meanColor;
					pCur->meanMV = 0.5*cv::Point(pNew->x - pCur->x, pNew->y - pCur->y) + 0.5*pCur->meanMV;
					newassigned.at<uchar>(j) = 1;
				}
			}
	}

	//5. Include new blobs not assigned
	//std::cout << "Include new blobs not assigned" << std::endl;
	for (int j = 0; j < blobListNew.getBlobNum(); j++)
		if (newassigned.at<uchar>(j) == 0)
		{
			ExtendedBlob *pBlob = blobListNew.getBlob(j)->clone();
			pBlob->active = true;
			pBlob->life_count = MAX_VALUE;
			pBlob->matches = 0;
			pBlob->ID = this->id_counter++;
			pBlob->IDcolor = cv::Scalar((IN0*rand()) % 256, (IN1*rand()) % 256, (IN2*rand()) % 256, (IN3*rand()) % 256);
			blobListCurr.addBlob(pBlob);
		}

	//6. Decrease lifespan of blobs & delete if needed
	//std::cout << "Decrease lifespan of blobs & delete if needed" << std::endl;
	for (int i = 0; i < blobListCurr.getBlobNum(); i++)
	{
		ExtendedBlob *pBlob = blobListCurr.getBlob(i);
		if (pBlob->active == false)
		{
			pBlob->life_count--;
			if (pBlob->life_count <= 0)
				blobListCurr.delBlob(i);
		}
	}

	// 7. Filter blob list to remove erroneous insertions
	//std::cout << "Filter blob list to remove erroneous insertions" << std::endl;
	for (int i = 0; i < blobListCurr.getBlobNum(); i++)
	{
		ExtendedBlob *p1 = blobListCurr.getBlob(i);
		bool deleted = false;
		for (int j = 0; j < blobListCurr.getBlobNum() && deleted==false; j++)
		{
			//std::cout << "\t getting data1" << std::endl;

			ExtendedBlob *p2 = blobListCurr.getBlob(j);
			cv::Rect r1 = cv::Rect(cv::Point(p1->x, p1->y), cv::Point(p1->x + p1->w, p1->y + p1->h));
			cv::Rect r2 = cv::Rect(cv::Point(p2->x, p2->y), cv::Point(p2->x + p2->w, p2->y + p2->h));
			cv::Rect  intersect = r1 & r2;
			//std::cout << "\t getting data2" << std::endl;
			if ((2 * (double)intersect.area()) / ((double)r1.area() + (double)r2.area()) > 0.6 && i!=j)
			{
				std::cout << "\t getting data3" << std::endl;
				//the two blobs are likely to be same, delete the newer one
				if (p1->matches > p2->matches)
					blobListCurr.delBlob(j);
				else
				{
					blobListCurr.delBlob(i);
					deleted=true;
				}
			}
		}
	}

	//std::cout << "end" << std::endl;
	return 1;
}

std::vector<cvBlob> &BasicBlobTracker::getBlobList()
{
	this->outputBlobs.clear();
	for (int i = 0; i < blobListCurr.getBlobNum(); i++)
	{
		ExtendedBlob *pBlob = blobListCurr.getBlob(i);
		if (pBlob->matches > MIN_LIFE)
			outputBlobs.push_back(initBlob(pBlob->ID, pBlob->x, pBlob->y, pBlob->w, pBlob->h));
	}
	return outputBlobs;
}

/**
*	Returns the tracked blob's list
*
* \return BlobList<ColourBlob*>* The tracked blob's list
*
*/
BlobList<ExtendedBlob*> &BasicBlobTracker::getListExtendedBlob() {
	return this->blobListCurr;
}

/**
 *	Returns the output image of the tracker
 *
 * \return IplImage* The generate image with the output state of the blobs
 *
 */
cv::Mat &BasicBlobTracker::getResultImage(cv::Mat &frame) {

	frame.copyTo(this->tracking_result);
	for (int i = 0; i < blobListCurr.getBlobNum(); i++)
	{
		ExtendedBlob *pBlob = blobListCurr.getBlob(i);
		if (pBlob->matches > MIN_LIFE)
		{
			CvPoint pt1 = cvPoint(pBlob->x,pBlob->y);
			CvPoint pt2 = cvPoint(pBlob->x+pBlob->w, pBlob->y+pBlob->h);
			cv::rectangle(tracking_result, cv::Rect(pt1, pt2), cv::Scalar(pBlob->IDcolor), 2);
		}
	}

	return tracking_result;
}

/**
 *	Method to convert the input blob's list (BasicBlobs) in a useful blob's list (ColourBlobs)
 *
 * \param pImg The input frame to process
 * \param blobs The input blob's list
 *
 */
void BasicBlobTracker::convertBlobToExtended(cv::Mat &frame, std::vector<cvBlob> &blobs) {
	
	for (int i = 0; i < (int)blobs.size(); i++)
	{
		cvBlob blob = blobs.at(i);
		ExtendedBlob *pBlob = new ExtendedBlob(blob);
		pBlob->meanColor = cv::mean(frame(cv::Range(blob.y, blob.y + blob.h), cv::Range(blob.x, blob.x + blob.w)));
		pBlob->meanMV = cvPoint(0,0);
		pBlob->life_count = MAX_VALUE; //maximum lifetime
		pBlob->matches = 0;
		pBlob->maxTray = MAX_TRAYECTORIES;
		this->blobListNew.addBlob(pBlob);
	}	
	return;
}

/**
 *	Method to calculate the difference between the two blob's colour
 *
 * \param c1 The media colour of the first blob
 * \param c2 The media colour of the second blob
 * \return double The calculated difference
 *
 */
double BasicBlobTracker::distance_color(cv::Scalar c1, cv::Scalar c2)
{
	double temp1, temp2, temp3, temp;
	temp1 = c1.val[0] - c2.val[0];
	temp2 = c1.val[1] - c2.val[1];
	temp3 = c1.val[2] - c2.val[2];
	temp = temp1 + temp2 +  temp3;
	return 0.33 * sqrt(temp*temp);
	
	/**/
	//return cv::norm(c1, c2);	
}
