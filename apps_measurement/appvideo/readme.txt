/* VIDEO PROCESSING APPLICATION - CLIENT
 * This program implements a video processing application for multi-target people tracking-by-detection 
 * using the upper body detection [1], implemented using OpenCV. Visual descriptors of tracked people 
 * are generated (RGB, HSV and YCrCb 16-bin histograms) and wirelessly  transmitted to the server 
 * for re-identification applications [2].
 *
 * Refs
 * [1] R. Vezzani et al, “People reidentification in surveillance and forensics: A survey,” ACM Computing Surveys, 2013.
 * [2] R. Mazzon et al, “Person re-identification in crowd,” Pattern Recogn. Lett., 2012.
 *
 *  INSTRUCTIONS TO RUN THIS APPLICATION
 *  1) Compile and run the server 'appvideo_server' *
 *  2) The current user running this program must have admin rights to enable/disable HW
 *  	Please use your admin password in the file system_utils.cpp
 *  	Change the string "your_pass_here" with your admin password
 *  3) Check the default settings for sensing, processing & comms in the "config" struture
 *  	available in the function "parseCmdOptions" in the file "system_utils.cpp".
 *  4) Please do check the testing conditions in "system_utils.cpp::enableTestConditions" &
 *	    "system_utils.cpp::disableTestConditions" to match your current HW (i.e. the number of cores to disable)
 *  5) The current user running this program must have admin rights to enable/disable HW
 *  	Please enter your password in the file system_utils.cpp
 *  	Change the string "your_pass_here" with your admin password
 *  6) Compile this program using the associated makefile
 *  7) Run this program by typing "./appvideo" in the command line
 *
 * Library requirements:
 * - OpenCV library v3.0 http://opencv.org
 * - libusb v1.0 http://www.libusb.org/
 * - Measurement Unix utils: acpitool, ifconfig, cpufreq-tools, wondershaper
 *	
 *	If you use this material, please cite the following paper:
 * 		"Energy consumption models for smart-camera networks",
 *		J. SanMiguel and A. Cavallaro, "
 *		IEEE Transactions on Circuits and Systems for Video Technology, 2016
 *
 * Author: Juan C. SanMiguel (juancarlos.sanmiguel@uam.es)
 * Date: July 2016
 */

