#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
 
int main(){
  char server_response[256];

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

  //receive data
  recv(network_socket, &server_response, sizeof(server_response), 0);
  printf("%s\n", server_response);

  ////////////////////////////

  char client_message[256]="0";
  send(network_socket, client_message, sizeof(client_message), 6);

  recv(network_socket, &server_response, sizeof(server_response), 0);
  printf("%s\n", server_response);

  // .............

  char client_message2[256]="1,1000000.00";
  send(network_socket, client_message2, sizeof(client_message2), 6);

  recv(network_socket, &server_response, sizeof(server_response), 0);
  printf("%s\n", server_response);

  // ...........

  char client_message3[256]="2,50000.00";
  send(network_socket, client_message3, sizeof(client_message3), 6);

  recv(network_socket, &server_response, sizeof(server_response), 0);
  printf("%s\n", server_response);

  // ...........

  char client_message4[256]="5";
  send(network_socket, client_message4, sizeof(client_message4), 6);

  recv(network_socket, &server_response, sizeof(server_response), 0);
  printf("%s\n", server_response);

  //////////////////////////////

  //close the socket
  close(network_socket);

  return 0;
}