//file description
/**
 * \file ExtendedBlob.cpp 
 * \author Victor Fernandez-Carbajales(vfc)
 * \date 20-05-08		
 * \brief Implementation of ExtendedBlobr class
 * \version 1.1\n
 *			Version history:\n
 *				- 1.0(?): Initial Implementation (vfc)
 *
 */

// Addicional includes
#include "ExtendedBlob.hpp"

/**
 *	Standard class Constructor. 
 *
 */
ExtendedBlob::ExtendedBlob() {
	this->maxTray = MAX_TRAYECTORIES;
	this->active = true;
	this->matches = 0;
	this->life_count = 0;
}

ExtendedBlob::ExtendedBlob(cvBlob blob)
{
	this->x = blob.x;
	this->y = blob.y;
	this->w = blob.w;
	this->h = blob.h;
}


/**
 *	Standard class destructor
 *
 */
ExtendedBlob::~ExtendedBlob() {
	this->blobTrayec.clear();		
}

/**
 *	Returns the life count of the blob
 *
 * \return int The count life of the blob
 *
 */
int ExtendedBlob::getCount() {
	return this->life_count;
}

/**
 *	Returns the life count of the blob
 *
 * \return int The count life of the blob
 */
bool ExtendedBlob::getAct() {
	return this->active;
}

/**
 *	Returns the group of blob's trayectories
 *
 * \return vector<CvBlob*>* A vector with te blob's trayectories
 */
vector<cvBlob> &ExtendedBlob::getBlobTrayec() {
	return this->blobTrayec;
}

/**
 *	Returns the blob's trayectori in position n
 *
 * \return CvBlob* The blob's trayectory in the position n
 */
cvBlob &ExtendedBlob::getBlobTrayec(int n) {
	return this->blobTrayec.at(n);
}

/**
 *	Returns the maximun number of blob's trayectories
 *
 * \return int The index of the blob's trayectori
 */
int ExtendedBlob::getMaxTray() {
	return this->maxTray;
}

/**
 *	Method to set the life count of the blob.
 *
 * \param count the new value of the life count
 *
 */
void ExtendedBlob::setCount(int count) {
	this->life_count = count;
}

/**
 *	Method to set the blob's trayectories
 *
 * \param treyec The new blob's trayectories
 *
 */
void ExtendedBlob::setBlobTrayec(vector<cvBlob> &trayec) {
	this->blobTrayec = trayec;
}

/**
 *	Method to set a blob's trayectory
 *
 * \param blob The new blob's trayectory
 * \param n The index where the new blob's trayectori must be set
 *
 */
void ExtendedBlob::setBlobTrayec(int n, cvBlob& blob) {
	if (this->blobTrayec.size() < (unsigned int)n)
		return;
	this->blobTrayec.at(n).x = blob.x;
	this->blobTrayec.at(n).y = blob.y;
	this->blobTrayec.at(n).w = blob.w;
	this->blobTrayec.at(n).h = blob.h;
}

/**
 *	Method to set a maximun number of blob's trayectories
 *
 * \param max The new maximun number of blob's trayectories
 * \return int a Code of possible errors
 *
 */
int ExtendedBlob::setMaxTray(int max) {
	// The new maximun number of trayectories must be 1 o bigger
	if (max <= 0)
		return -1;

	this->blobTrayec.resize(max);
	this->maxTray = max;
	
	return 0;	
}


/**
 *	Method to add a new blob's trayectory
 *
 * \param newTrayectory The new trayectori to add
 *
 */
void ExtendedBlob::addBlobTrayectory(cvBlob &newTrayectory) {
	
	// If the blob's trayectories hasn't space erase the oldest trayectory
	if (this->blobTrayec.size() >= (unsigned int)this->getMaxTray())
		this->eraseBlobTrayectory(0);

	// Add the new trayectory
	this->getBlobTrayec().push_back(newTrayectory);
	return;
}

/**
 *	Method to erase a old blob's trayectory
 *
 * \param index The index of the blob's trayectory to erase
 *
 */
void ExtendedBlob::eraseBlobTrayectory(int index) {
	vector<cvBlob>::iterator iter;
	// Can't erase blob's trayectories in out range index
	if ((unsigned int)index > this->getBlobTrayec().size()) 
		return;

	iter = this->getBlobTrayec().begin();
	iter = iter + index;
	this->getBlobTrayec().erase(iter);
	return;
}

ExtendedBlob* ExtendedBlob::clone() {

	
	ExtendedBlob* temp = new ExtendedBlob(initBlob(this->ID,this->x,this->y,this->w,this->h));
	temp->IDcolor = this->IDcolor;
	temp->life_count = this->life_count;
	temp->matches = this->matches;
	temp->active = this->active;
	temp->maxTray = this->maxTray;

	temp->meanColor = this->meanColor;
	temp->meanMV = this->meanMV;

	for (int i = 0; (unsigned int)i < this->blobTrayec.size(); i++) {
		cvBlob trayectory = initBlob(this->ID, blobTrayec.at(i).x, blobTrayec.at(i).y, blobTrayec.at(i).w, blobTrayec.at(i).h);
		temp->addBlobTrayectory(trayectory);
	}

	return temp;
}

void ExtendedBlob::update(ExtendedBlob &blob)
{
	this->x = blob.x;
	this->y = blob.y;
	this->w = blob.w;
	this->h = blob.h;
	this->matches++;	
	this->active = true;
}