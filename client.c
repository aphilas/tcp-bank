#include <stdio.h>
#include <stdlib.h>
 
#include <sys/types.h>
#include <sys/socket.h>
 
#include <netinet/in.h>
#include <unistd.h>
 
int main(){
    //create a socket
    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM,0);

    //specify address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9302);
    server_address.sin_addr.s_addr = INADDR_ANY;

    ///connection
    int connection_status = connect(network_socket,(struct sockaddr *) &server_address, sizeof(server_address));
 
    if (connection_status == -1){
			printf("There is an error making a connection to the remote socket \n\n");
    }

    //recieve data
    char server_response[256];
    recv(network_socket, &server_response, sizeof(server_response), 0);
    
		//print the data
    printf("the server sent the data: %s\n", server_response);
		
    //clost the socket
    close(network_socket);
 
    return 0;
}