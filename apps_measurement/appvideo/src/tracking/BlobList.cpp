//file description
/**
 * \file BlobList.cpp 
 * \author Victor Fernandez-Carbajales(vfc)
 * \author Juan Carlos San Miguel Avedillo (jcs)
 * \date 1/06/08	
 * \brief Implementation of BlobExtractor class
 * \version 1.1\n
 *			Version history:\n
 *				- 1.0(20/05/08): Initial Implementation (vfc)
 *				- 1.1(1/06/08): Modification (jcs)
 *
 */


#ifndef BLOBLIST_CPP
#define BLOBLIST_CPP

//Additional includes
#include "BlobList.hpp"
#include <stdlib.h>

/**
 *	Standard class Constructor. 
 *
 */
template <class T> BlobList<T>::BlobList() {
	this->pBlob = new vector<T>;
}

/**
 *	Class Constructor with an initial blob array
 *
 * \param pBlob Initial vector of blobs
 *
 */
template <class T> BlobList<T>::BlobList(vector<T> *pblob) {
	this->pBlob = pblob;
}

/**
 *	Standard class destructor
 *
 */
template <class T> BlobList<T>::~BlobList() {
	if(this->getBlobNum() > 0)
		this->clear();
	if (this->pBlob != NULL) {
		delete this->pBlob;
		this->pBlob = NULL;
	}
}

/**
 *	Method to add a blob in the blob's list
 *
 * \param blob Blob to add. The type of the blob depends on the blob list inizialization.
 *
 * \return Returns operation code (if error returns a value < 0)
 */
template <class T>
int BlobList<T>::addBlob(T blob) {
	this->pBlob->push_back(blob);
	return 0;
}

/**
 *	Method to clear all the blobs in the list
 *
 * \return Returns operation code (if error returns a value < 0)
 */
template <class T>
int BlobList<T>::clear() {
	for(int i = 0; i < this->getBlobNum(); i++) {
		delete this->getBlob(i);
	}
	this->pBlob->clear();
	return 0;
}

/**
 *	Method to delete a blob from the blob's list
 *
 * \param n Position of the blob to delete
 *
 * \return Returns operation code (if error returns a value < 0)
 */
template <class T>
int BlobList<T>::delBlob(int n) {
	delete this->getBlob(n);
	typename std::vector<T>::iterator iter;
	iter = this->pBlob->begin();
	iter = iter+n;
	this->pBlob->erase(iter);
	return 0;
}

/**
 *	Method to delete a blob from the blob's list
 *
 * \param ID Blob identification number for blob to delete
 *
 * \return Returns operation code (if error returns a value < 0)
 */
template <class T> int BlobList<T>::delBlobByID(int ID)
{
	int i = 0;

	typename vector<T>::iterator iter;
	iter = this->pBlob->begin();
	
	for(i = 0; i < (int)this->pBlob->size(); i++) {
		if (this->pBlob->at(i)->getID() == ID) {
			delete this->getBlob(i);
			iter = iter + i;
			this->pBlob->erase(iter);
			return 0;
		}
	}
	return -1;	
}

/**
 *	Method to get the numbers of blobs stored in the blob list
 *
 * \return Returns the numbers of blobs
 */
template <class T>
int BlobList<T>::getBlobNum() {
	return (int)this->pBlob->size();
}

/**
 *	Method to get a blob from the blob's list.
 *
 * \param n Blob position  for blob to get
 *
 * \return Blob type stored in the list
 */
template <class T>
T BlobList<T>::getBlob(int n) {
	return this->pBlob->at(n);	
}

/**
 *	Method to get a blob from the blob's list
 *
 * \param n Blob ID for blob to get
 *
 * \return Blob type stored in the list
 */
template <class T>
T BlobList<T>::getBlobByID(int ID) {
	int i = 0;
	
	for(i = 0; i < (int)this->pBlob->size(); i++) {
		if (this->pBlob->at(i)->getID() == ID) {
			return this->pBlob->at(i);
		}
	}
	return NULL;	
}

/**
 *	Method to set a blob in the blob's list.
 *
 * \param n Blob position  for blob to set
 * \param blob Blob (same type as the blob type stored in the list)
 *
 * \return Operation code
 */
template <class T> int BlobList<T>::setBlob(int n, T Blob)
{
	if (n > (int)this->pBlob->size())
		return -1;
	typename vector<T>::iterator iter = this->pBlob->begin();
	iter = iter+n;
	delete this->pBlob->at(n);
	this->pBlob->insert(iter,Blob);
	return 0;
}

/**
 *	Method to set a blob in the blob's list.
 *
 * \param n Blob ID  for blob to set
 * \param blob Blob (same type as the blob type stored in the list)
 *
 * \return Operation code
 */
template <class T>
int BlobList<T>::setBlobByID(int ID, T Blob) {
	int i = 0;
	typename vector<T>::iterator iter = this->pBlob->begin();
	
	for(i = 0; i < (int)this->pBlob->size(); i++) {
		if (this->pBlob->at(i)->getID() == ID) {
			delete this->pBlob->at(i);
			this->pBlob->insert(iter,Blob);
			return 0;
		}
		iter++;
	}
	return -1;
}
	

/**
 *	Prints some features of all the blobs in a file
 *
 * \param filename output filename to print blob extraction results
 *
 * \return Operation code
 */
template <class T>
int BlobList<T>::write(char *filename) {
	return -1;
}

#endif
