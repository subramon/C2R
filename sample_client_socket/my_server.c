// gcc -std=gnu99 -O3 -I../inc/ my_server.c -o server
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "macros.h"
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

// Function designed for chat between client and server.
static void
func(
    int sockfd,
    char *buf, // [szbuf]
    int szbuf
    )
{
  for ( int i = 0; ; i++ ) {
    // read size of message
    read(sockfd, buf, sizeof(int));
    // read the message from client and copy it in buffer
    int n = ((int *)buf)[0];
    // printf("i = %d, n = %d \n", i, n);
    if ( ( n <= 0 ) || ( n > szbuf ) ) { WHEREAMI; exit(-1); }
    int num_to_read = n;
    int num_read = sizeof(int);
    for ( ; num_read < num_to_read; ) { 
      int nr = read(sockfd, buf + num_read, num_to_read); 
      if ( nr == 0 ) { WHEREAMI; exit(-1); }
      num_read += nr; 
    }
    for ( int j = sizeof(int); j < n; j++ ) { 
      if ( buf[j] != 1 ) { WHEREAMI; exit(-1); }
      buf[j] = 2;
    }
    // and send buffer back to client
    int nw = write(sockfd, buf, n);
    if ( nw < 0 ) { WHEREAMI; exit(-1); }
  }
}

// Driver function
int main(
    int argc,
    char **argv
    )
{
  int status = 0;
  int sockfd, connfd;
  struct sockaddr_in servaddr, cli;
  char *buf = NULL;
  int n = 1048576;

  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("socket creation failed...\n"); go_BYE(-1);
  }
  else {
    printf("Socket successfully created..\n");
  }
  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(PORT);

  // Binding newly created socket to given IP and verification
  if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
    printf("socket bind failed...\n"); go_BYE(-1);
  }
  else {
    printf("Socket successfully binded..\n");
  }

  // Now server is ready to listen and verification
  if ((listen(sockfd, 5)) != 0) {
    printf("Listen failed...\n");
    exit(0);
  }
  else {
    printf("Server listening..\n");
  }
  unsigned int len = sizeof(cli);

  // Accept the data packet from client and verification
  connfd = accept(sockfd, (SA*)&cli, &len);
  if (connfd < 0) {
    printf("server accept failed...\n"); go_BYE(-1); 
  }
  else {
    printf("server accept the client...\n");
  }

  // Function for chatting between client and server
  buf = malloc(n); return_if_malloc_failed(buf);
  printf("n = %d \n", n);
  func(connfd, buf, n); 

BYE:
  // After chatting close the socket
  if ( sockfd < 0 ) { close(sockfd); }
  free_if_non_null(buf); 
  return status;
}

