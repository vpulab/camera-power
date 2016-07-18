
#if !defined(PROPERTYFILE_READER_H_INCLUDED_)
#define PROPERTYFILE_READER_H_INCLUDED_

#include <stdio.h>

#define PROPS_OK		1
#define PROPS_NO_EXIST	0
#define PROPS_ERROR		-1


class PropertyFileReader
{
	char filename[50];
	FILE *filePtr;

public:
	PropertyFileReader(const char *filename, bool create=false);
	virtual ~PropertyFileReader();

	int getProperty(const char *name, char *valor);
	int getProperty(const char *name, int *valor);
	int getProperty(const char *name, long *valor);
	int getProperty(const char *name, double *valor);
	int getProperty(const char *name, float *value);


	void readLines();

	int setProperty(const char *name, char *valor);
	int setProperty(const char *name, int valor);
	int setProperty(const char *name, long valor);
	int setProperty(const char *name, double value);
};

#endif // !defined(PROPERTYFILE_READER_H_INCLUDED_)
