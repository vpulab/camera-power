/* /* WIFI POWER CALIBRATION - CLIENT
 * This program implements the server for the wifi calibration method.
 *
 *  INSTRUCTIONS TO RUN THIS APPLICATION
 *  1) Compile this program using the associated makefile
 *  2) Run this program by typing "./wifipower_server PORT" in the command line
 *  3) Compile and run the client 'wifipower' to start receiving data
 *  (alternatively, you may want to use the provided bash script to run several tests)
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
#define HOST_PORT 1030
 
int main(int argc , char *argv[])
{
    int socket_server , socket_client , c , read_size, sent_size;
    struct sockaddr_in server , client;
    char client_message[BUFFER_SIZE];
    int port=HOST_PORT;

    if (argc>1)
    	port = atoi(argv[1]);

    system("clear");
    system("ulimit -s 16000");
    puts("SERVER launched (single thread version) for wifipower");

    //Create socket
    socket_server = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_server == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );
     
    //Bind
    if( bind(socket_server,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(socket_server , 3);

    //Accept and incoming connection
    std::cout << "Waiting for incoming connections on port " << port << std::endl;
    c = sizeof(struct sockaddr_in);
    while( (socket_client = accept(socket_server, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
    	//accept connection from an incoming client

    	if (socket_client < 0)
		{
			perror("accept failed");
			return 1;
		}
		puts("Connection accepted");

		//define timeout
		struct timeval tv;
		tv.tv_sec =2;
		tv.tv_usec= 0;

		//setsockopt(socket_client, SOL_SOCKET,SO_RCVTIMEO, &tv, sizeof(tv));
		//setsockopt(socket_client, SOL_SOCKET,SO_SNDTIMEO, &tv, sizeof(tv));

		//Receive a message from client
		while( (read_size = recv(socket_client , client_message , BUFFER_SIZE , 0)) > 0 )
		{
			if(client_message[0] == 'r')
			{
				sprintf(client_message,"OK RX");
				std::cout << client_message << std::endl;
				write(socket_client, client_message ,strlen(client_message));//ack the message

				read_size = recv(socket_client , client_message , BUFFER_SIZE , 0);
				int bytes = atoi(client_message);
				sprintf(client_message,"OK RX - %d bytes", bytes);
				std::cout << client_message << std::endl;

				bool stop = false;
				while( !stop )
				{
					sent_size = send(socket_client , client_message , bytes , 0);
					if (sent_size ==0)
					{
						perror("client disconnect");
						stop = true;
					}

					if (sent_size ==-1)
					{
						perror("communication failed");
						stop = true;
					}
				}
			}

			if(client_message[0] == 't')
			{
				sprintf(client_message,"OK TX");
				std::cout << client_message << std::endl;
				write(socket_client, client_message ,strlen(client_message));//ack the message

				bool stop = false;
				while( !stop )
				{
					read_size = recv(socket_client , client_message , BUFFER_SIZE , 0);
					if (read_size ==0)
					{
						perror("client disconnect");
						stop = true;
					}
					if (read_size ==-1)
					{
						perror("communication failed");
						stop = true;
					}
				}
			}
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
