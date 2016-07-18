# Software for Skin detection by dual maximization of detectors agreement for video monitoring

## Overview 
This software provides Unix programs to get power measurements of (emulated)smart cameras composed of sensing, processing and communication modules.

Check out the [project site](http://www-vpu.ii.uam.es/publications/camera_power/) for all the details like:
- [Sample results](http://www-vpu.eps.uam.es/publications/camera_power/data/)
- Validation results for [sensing](http://www-vpu.ii.uam.es/publications/camera_power/data/SEN_0_active-idle_consumption.pdf) [processing](http://www-vpu.ii.uam.es/publications/camera_power/data/PRO_3_active-vs-idle.pdf) and [communication] (http://www-vpu.ii.uam.es/publications/camera_power/data/COM_1_runs_active.pdf)

## Introduction

These experiments have been divided in two phases:
1. Get the measurements and model fitting using Eq. (4).
We created three C++ programs to get the measurements under a set of common testing conditions to obtain comparable power measurements for modeling the sensing, processing and communication power.
 
2. Design and development of the video application.
A C++ video application has been created to make use of sensing, processing and communication. This application consists on a tracking-by-detection approach based on the upper-body parts of people [1] which transmits visual descriptors that can be used for people re-identification purposes [2]. The OpenCV library was used to implement the visual analysis algorithms.
 
A detailed description of the protocol to get the measurements is available at http://www-vpu.eps.uam.es/publications/camera_power/description_validation.pdf

## License and Citation

 This software is released under the [Academic Public License](https://github.com/vpulab/camera-power/blob/master/LICENSE).).

Please cite the following publication if this software helps your research:

    @article{sanmiguel2016TCSVT,
      Author = {J. SanMiguel and A. Cavallaro},
      Journal = {IEEE Transactions on Circuits and Systems for Video Technology},
      Title = {Energy consumption models for smart-camera networks},
      Year = {2016},
      number = {0},
      issue = {0},
      pages = {1-14},
      Month = {(to appear)}
    }
    

## Requirements

This tool was successfully tested under Ubuntu OS 14.04 64-bit Windows. Matlab scripts have been tested under MS Windows 7 (32-bit and 64-bit) and linux (32-bit) for versions 2012b and 2013a.

## Directory structure

The following directories are included:
- appvideo -> C++ video application for multi-target people tracking (client)
- appvideo_server -> C++ server for multi-target people tracking
- campower -> C++ application for experiments related to sensing
- cpupower -> C++ application for experiments related to processing
- wifipower -> C++ application for experiments related to comms (client)
- wifipower_server -> C++ application for experiments related to comms (server)

Each directory contains four main files:
- Makefile -> compile all the code and generate the executable 
- readme.txt -> more detailed instructions for running the corresponding executable
- config.ini -> configuration file for each application 
		(also executables can be configured from command line)
- *.sh -> bash script to run a set of experiments with different configurations



## Contact information
If you have any queries, please contact juancarlos.sanmiguel@uam.es.
