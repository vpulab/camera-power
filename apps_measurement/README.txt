UNIX PROGRAMS FOR POWER MEASUREMENTS
==============================================

If you use this material, please cite:

Energy consumption models for smart-camera networks
Juan C. SanMiguel and A. Cavallaro
IEEE Transactions on Circuits and Systems for Video Technology, 2016
http://www-vpu.eps.uam.es/publications/camera_power/

=== INTRODUCTION ===
This software provides Unix programs to get power measurements obtained as defined in Sec V of the mentioned paper.

These experiments have been divided in two phases:
1. Get the measurements and model fitting using Eq. (4).
We created three C++ programs to get the measurements under a set of common testing conditions to obtain comparable power measurements for modeling the sensing, processing and communication power.
 
2. Design and development of the video application.
A C++ video application has been created to make use of sensing, processing and communication. This application consists on a tracking-by-detection approach based on the upper-body parts of people [1] which transmits visual descriptors that can be used for people re-identification purposes [2]. The OpenCV library was used to implement the visual analysis algorithms.
 
A detailed description of the protocol to get the measurements is available at http://www-vpu.eps.uam.es/publications/camera_power/description_validation.pdf

=== CONTENTS ===
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


=== LICENSE ===
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This software is provided under the terms and conditions of the creative commons public license. Please refer to the file <./License.doc> for more information.

=== CONTACT ===
If you have any queries, please contact juancarlos.sanmiguel@uam.es
