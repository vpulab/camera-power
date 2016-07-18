# This Makefile compiles the "appvideo" video processing application. 
# Further details can be found in the source files (*.cpp) in the "src" directory
#
# If you use this material, please cite the following paper:
# 	"Energy consumption models for smart-camera networks", 
#	J. SanMiguel and A. Cavallaro, "
#	IEEE Transactions on Circuits and Systems for Video Technology, 2016
#
# Author: Juan C. SanMiguel (juancarlos.sanmiguel@uam.es)
# Date: July 2016

CC=g++
CFLAGS=-c -Wno-unused-variable -Wno-unused-but-set-variable -Wno-comment
LDFLAGS=

#Executable name with the same as the directory
#EXECUTABLE=appvideo
EXECUTABLE = $(notdir $(shell pwd))

#Source files to compile
SRC_DIR=./src/
SRC_PATTERN ='*.cpp'
SOURCES = $(shell find $(SRC_DIR) -name $(SRC_PATTERN))
OBJECTS=$(SOURCES:.cpp=.o)

#Includes
OPENCV_INCLUDES = /usr/local/include/ /usr/local/include/opencv/ /usr/local/include/opencv2/ 
LOCAL_INCLUDES = ./src/blob/ ./src/tracking/ ./src/descriptor/ 
INCLUDE=$(OPENCV_INCLUDES) $(LOCAL_INCLUDES)
#Add '-I' to each include
INC_PARAMS=$(foreach d, $(INCLUDE), -I$d)

PATH_LIB = /usr/local/lib/
LIBS = -lopencv_core -lopencv_highgui -lopencv_video -lopencv_videoio -lopencv_objdetect -lopencv_imgproc -lopencv_imgcodecs -lusb-1.0

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	@echo $(SOURCES)
	@echo $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ -L$(PATH_LIB) $(LIBS) -lm

.cpp.o:
	$(CC) $(CFLAGS) $(INC_PARAMS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
