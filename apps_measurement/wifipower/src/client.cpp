/*
    C ECHO client example using sockets
    http://www.binarytides.com/server-client-example-c-sockets-linux/
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>    //strlen
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write

#include "utils.h"

int tx_data(int sock, int bytes)
{
    char message[BUFFER_SIZE];
    if( send(sock , message , bytes , 0) < 0) {
		puts("Send failed");
		return -1;
	}
    return 0;
}

int rx_data(int sock, int bytes)
{
    char server_reply[BUFFER_SIZE];
    if( recv(sock , server_reply , bytes , 0) < 0){
    	puts("recv failed");
        return -1;
    }
    return 0;
 }
