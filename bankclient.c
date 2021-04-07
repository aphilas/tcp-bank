#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
 
void displaymenu(int networkSocket);
void askagain(int network_socket);

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
  } else{
    displaymenu(network_socket);
  }
  
  return 0;
}

void askagain(int network_socket) {
  char answer[256];
  printf("Do you want to carry out another transaction? Type yes or no:\n"); 
  scanf("%s", answer); // [^\n]%*
  if ((strcmp(answer, "yes") == 0)){ 
    displaymenu(network_socket);
  }else{
    close(network_socket);
  }
}

void displaymenu(int network_socket){
  char option[3]; // option == "1"\n
  char message[256];
  int amount;
  char server_response[256];

  printf("Select an option\nMENU\n0.Open Account\n1.Deposit\n2.Withdraw\n3.Statement\n4.Close Account\n5.Balance\n");
  scanf("%s",option);

  if((strcmp(option, "1") == 0) || (strcmp(option, "2") == 0)){ 
    printf("Enter your input in the following format: option,amount:\n");
    scanf("%s", message);
    send(network_socket, message, sizeof(message), 6);
    recv(network_socket,&server_response, sizeof(server_response), 0);
    printf("%s\n",server_response);
  } else{
    send(network_socket,option, sizeof(option), 6);
    recv(network_socket,&server_response, sizeof(server_response), 0);
    printf("%s\n",server_response);
  }

  askagain(network_socket);
}
