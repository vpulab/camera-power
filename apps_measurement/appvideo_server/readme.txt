/* VIDEO PROCESSING APPLICATION - SERVER
 * This program implements the server for a video processing application performing
 * multi-target people tracking-by-detection using the upper body detection [1], implemented using OpenCV.
 *
 * Visual descriptors of tracked people are generated (RGB, HSV and YCrCb 16-bin histograms) and wirelessly
 * transmitted to the server for re-identification applications [2].
 *
 * Refs
 * [1] R. Vezzani et al, “People reidentification in surveillance and forensics: A survey,” ACM Computing Surveys, 2013.
 * [2]R. Mazzon et al, “Person re-identification in crowd,” Pattern Recogn. Lett., 2012.
 *
 *  INSTRUCTIONS TO RUN THIS APPLICATION
 *  1) Compile this program using the associated makefile
 *  2) Run this program by typing "./appvideo_server PORT" in the command line
 *  3) Compile and run the client 'appvideo' to start receiving data
 *
 * If you use this material, please cite the following paper:
 * 	"Energy consumption models for smart-camera networks",
 *	J. SanMiguel and A. Cavallaro, "
 *	IEEE Transactions on Circuits and Systems for Video Technology, 2016
 *
 * Author: Juan C. SanMiguel (juancarlos.sanmiguel@uam.es)
 * Date: July 2016
 */
 
