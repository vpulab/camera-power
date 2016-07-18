/* CPU PROCESSOR POWER CALIBRATION
 *
 * This program implements the calibration method for CPUs in Unix environment described in:
 * 	"Energy consumption models for smart-camera networks",
 *	J. SanMiguel and A. Cavallaro"
 *	IEEE Transactions on Circuits and Systems for Video Technology, 2016
 *
 *	If you use this material, please cite this paper.
 *
 * The objective of this program is to measure the CPU running (and idle) at the desired frequency
 * We assume that power in the sleep state ~0 mW.
 *
 * Testing conditions:
 * - Screen brightness set to minimum
 * - Disable all network interfaces, bluetooth, usb ports and CPUs (except CPU0)
 * - Sets CPU0 to certain frequency values (user selected, “on-demand” settings)
 *
 *  INSTRUCTIONS TO RUN THIS APPLICATION
 *  1) Setup the appropriate HW ID of the camera (line main.cpp::60)
 *  2) The current user running this program must have admin rights to enable/disable HW
 *  	Please use your admin password in the file system_utils.cpp
 *  	Change the string "your_pass_here" with your admin password
 *  3) Check the default settings for sensing, processing & comms in the "config" struture
 *  	available in the function "parseCmdOptions" in the file "system_utils.cpp".
 *  4) Please do check the testing conditions in "system_utils.cpp::enableTestConditions" &
 *	    "system_utils.cpp::disableTestConditions" to match your current HW (i.e. the number of cores to disable)
 *  5) Compile this program using the associated makefile
 *  6) Use the bash script "run_tests_cpu.sh" to configure the calibration procedure
 *     Currently you can test active/idle modes with different frequencies, number of
 *     runs, the length of each test and the sampling frequency.
 *     (Current experiments are for 1.2 1.6 2.0 2.4 GHz;each test will wait 50 secs to
 *      get data: 200 samples at 5 secs/sample).
 *  7) The result are stored in data files for each run (e.g.  PRO_act_1.2_r3.dat)
 *
 * Library requirements:
 * - libusb v1.0 http://www.libusb.org/
 * - Measurement Unix utils: acpitool, ifconfig, cpufreq-tools
 *
 * This program has been successfully tested on:
 * - CPU modeling for model Intel® Core™ i5-450M  (1.20 GHz-2.40GHz) (only one core is enabled)
 *
 * Author: Juan C. SanMiguel (juancarlos.sanmiguel@uam.es)
 * Date: July 2016
 */
