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
 
#include <iostream>
#include <stdio.h>      /* printf, fgets */
#include <stdlib.h>     /* atoi */
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write

#define BUFFER_SIZE 1000000
#define HOST_PORT 8888
 
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[BUFFER_SIZE];
    int port=HOST_PORT;

    if (argc>1)
    	port = atoi(argv[1]);

    system("clear");
    system("ulimit -s 16000");
    puts("SERVER launched (single thread version) for AppVideo (multi-target people tracking)");

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    std::cout << "Waiting for incoming connections on port " << port << std::endl;
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
    	//accept connection from an incoming client

    	if (client_sock < 0)
		{
			perror("accept failed");
			return 1;
		}
		puts("Connection accepted");

		//Receive a message from client
		int bytes=0;
		while( (read_size = recv(client_sock , client_message , BUFFER_SIZE , 0)) > 0 )
		{
			write(client_sock , "OK" , strlen("OK"));
		}

		if(read_size == 0)
		{
			puts("Client disconnected");
			fflush(stdout);
		}
		else if(read_size == -1)
		{
			perror("recv failed");
		}
		std::cout << "Waiting for incoming connections on port " << port << std::endl;
    }
     
    system("ulimit -s 8192");
    return 0;
}
