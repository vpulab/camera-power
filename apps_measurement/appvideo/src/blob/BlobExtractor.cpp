//file description
/**
 * \file BlobExtractor.cpp 
 * \author Juan Carlos San Miguel Avedillo (jcs)
 * \date 21/02/16		
 * \brief Implementation of BlobExtractor class
 * \version 1.0\n
 *			Version history:\n
 *				- 1.0(21/05/08): Initial Implementation (jcs)
 *				- 2.0(21/02/16): C++ Implementation  - removed depedencies with BlobList (jcs)
 *
 */

//Additional includes
#include "BlobExtractor.hpp"

/**
 *	Standard class Constructor. 
 *	Filtering option is activated by default
 *
 */
BlobExtractor::BlobExtractor() {
	this->maxHeight = MAX_HEIGHT_DEF;
	this->maxWidth = MAX_WIDTH_DEF;
	this->minHeight = MIN_HEIGHT_DEF;
	this->minWidth = MIN_WIDTH_DEF;
	this->enable_filtering = FILTERING_DEF;
	this->NumBlobs = 0;
}

/**
 *	Standard class destructor
 *
 */
BlobExtractor::~BlobExtractor() 
{	
	pBlobList.clear();
}

/**
 *	Blob extraction from 1-channel image. The extraction is performed based
 *	on 	the analysis of the connected components
 *
 * \param mask Foreground/Background segmentation mask (1-channel)
 * \param extractMask Parameter to determine if blob masks are extracted or not (default=false)
 * \param operationMask Image Mask to select the area of the image to be processed (if NULL, all the image will be processed)
 *
 * \return Operation code (negative if not succesful operation) 
 */
 // Blob extraction from an image
int BlobExtractor::extractBlobs(cv::Mat &mask, bool extractMask, const cv::Mat *operationMask)
{
	//clear previous blob list
	pBlobList.clear(); 

	//apply operation mask
	if (operationMask != NULL)
		cv::multiply(mask, *operationMask, mask);

	//copy input mask to start extraction
	cv::Mat binary;
	cv::threshold(mask, binary, 127, 1.0, cv::THRESH_BINARY); //0-nothing & 1-blob data to be labeled
	binary.convertTo(label_mask, CV_32FC1);

	//Connected component analysis
	NumBlobs = 0;

	for (int y = 0; y<label_mask.rows; y++)
		for (int x = 0; x<label_mask.cols; x++)
			if (label_mask.at<float>(cv::Point(x,y)) == 1.0)
			{				
				cv::Rect rect;
				NumBlobs++;

				//blob extraction using connected component analysis
				cv::floodFill(label_mask, cv::Point(x,y), cv::Scalar(NumBlobs+1), &rect, cv::Scalar(0), cv::Scalar(0), 4);
				
				//check limits
				if (rect.width >= minWidth && rect.width <= maxWidth && rect.height >= minHeight && rect.height <= maxHeight)
				{
					//BasicBlob *pBlob = new BasicBlob(NumBlobs,rect.x, rect.y, rect.width, rect.height);
					cvBlob pBlob = initBlob(NumBlobs, rect.x, rect.y, rect.width, rect.height);

					//creat blob mask if requested
					/*cv::Mat blobMask;
					if (extractMask){
						cv::compare(label_mask, NumBlobs, blobMask, cv::CMP_EQ);
						pBlob.blobMask = blobMask;
					}*/
					pBlobList.push_back(pBlob);
				}				
			}
	return 1;
}

/**
 *	Returns the internal blobs list with all basic blob extract from the foregorund image is always marked with the 0 value.
 *
 * \return BlobList<cvBlob>* The blob's list
 */
std::vector<cvBlob> *BlobExtractor::getBlobList() {
	return &(this->pBlobList);
}

/**
 *	Returns an image containing the bounding box mask of the blobs previously extracted.
 *
 * \param frame Image to paint the bounding boxes
 *
 * \return Bounding Box blob mask
 */
cv::Mat &BlobExtractor::getBlobBoundingBoxMask(cv::Mat &frame)
{
	frame.copyTo(this->label_image);
	for (int i = 0; i < (int)this->pBlobList.size(); i++)
	{
		cvBlob blob = pBlobList.at(i);
		CvPoint pt1 = cvPoint(blob.x, blob.y);
		CvPoint pt2 = cvPoint(blob.x+ blob.w, blob.y+ blob.h);
		cv::rectangle(label_image, cv::Rect(pt1, pt2), cv::Scalar(blob.ID), 2);
	}

	return label_image;
}

/**
 *	Method to get the number of blobs previously extracted
 *
 * \return Number of blobs extracted
 *
 */
int BlobExtractor::getNumBlobs()
{
	return pBlobList.size();
}
/**
 *	Method to set Blob Filtering parameters. Currently, filtering blob process is based on imposing 
 *	constraints in the dimmensions of the blob.
 *
 * \param _minWidth Minimum width of the blob to be detected
 * \param _minHeight Minimum height of the blob to be detected
 * \param _maxWidth Maximum width of the blob to be detected
 * \param _maxHeight Maximum height of the blob to be detected
 *
 */
void BlobExtractor::setBlobFilter(int _minWidth,int _minHeight,int _maxWidth,int _maxHeight)
{
	this->minWidth  = _minWidth;
	this->minHeight = _minHeight;
	this->maxWidth  = _maxWidth;
	this->maxHeight = _maxHeight;
}

/**
 *	Method to activate/deactivate blob filtering state (activated by default)
 *
 * \param state Boolean value indicating the new status of the filtering process
 *
 */
void BlobExtractor::setBlobFilterState(bool state)
{
	this->enable_filtering = state;
}

/**
 *	Prints some features of all the blobs in a file
 *
 * \param filename output filename to print blob extraction results
 *
 */

int BlobExtractor::PrintBlobs( char *filename)
{
	FILE *pf=NULL;
	pf = fopen(filename,"a+");
	
	if (pf!=NULL)	{
		//fprintf(pf,"Frame %d:\n ",frameID);
		for(int i=0;i<this->getNumBlobs();i++)
		{
			cvBlob blob = pBlobList.at(i);
			fprintf(pf,"Blob %d: (%d,%d,%d,%d)\n", blob.ID, blob.x, blob.y, blob.w, blob.h);
		}
		
		fclose (pf);

		return 1;
	}
	return 1;
}

/**
 *	Method to show blob extraction info in output console
 *
 */
void BlobExtractor::showBlobInfo()
{
	printf("Frame %d\n\tNum Blobs %d:\n ",1,getNumBlobs());
		for(int i=0;i<this->getNumBlobs();i++)
		{
			cvBlob blob = pBlobList.at(i);
			printf("Blob %d: (%d,%d,%d,%d)\n", blob.ID, blob.x, blob.y, blob.w, blob.h);
		}
}

/**
*	Returns the image with the labels assigned to each blob (i.e., their IDs)
*
*/
cv::Mat &BlobExtractor::getLabelImage()
{
	return this->label_mask;
}