// gcc -std=gnu99 -O3 -I../inc/ my_client.c -o client
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "macros.h"

uint64_t
RDTSC(
    void
    )
{
  unsigned int lo, hi;
  asm volatile("rdtsc" : "=a" (lo), "=d" (hi));
  return ((uint64_t)hi << 32) | lo;
}
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
static int
func(
    int sockfd,
    char *sendbuf,
    char *rcvbuf,
    int bufsz
    )
{
  int status = 0;
  char buff[MAX];
  int n = ((int *)sendbuf)[0];
  if ( ( n <= 0 ) || ( n > bufsz ) ) { go_BYE(-1); }
  // printf(" sending n = %d \n", n);
  write(sockfd, sendbuf, bufsz);
  read(sockfd, rcvbuf, sizeof(int));
  n = ((int *)rcvbuf)[0];
  int num_to_read = n;
  int num_read = sizeof(int);
  for ( ; num_read < num_to_read; ) { 
    int nr = read(sockfd, rcvbuf+num_read, num_to_read); 
    num_read += nr;
  }
  for ( int j = sizeof(int); j < n; j++ ) {
    if ( rcvbuf[j] != 2 ) { go_BYE(-1); }
  }
BYE:
  return status;
}

int 
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  int sockfd = -1;
  struct sockaddr_in servaddr;
  int n = 65536;
  int niters = 16;
  char *sendbuf = NULL; char *rcvbuf = NULL;


  if ( argc >= 2 ) {
    n = atoi(argv[1]); if ( n <= 0 ) { go_BYE(-1); }
  }
  if ( argc >= 3 ) {
    niters = atoi(argv[2]); if ( niters <= 0 ) { go_BYE(-1); }
  }
  printf("n = %d, niters = %d \n", n, niters);
  sendbuf = malloc(n); return_if_malloc_failed(sendbuf);
  rcvbuf = malloc(n); return_if_malloc_failed(rcvbuf);

  // socket create and varification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("socket creation failed...\n"); go_BYE(-1);
    exit(0);
  }
  else {
    printf("Socket successfully created..\n");
  }
  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(PORT);

  // connect the client socket to server socket
  if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
    printf("connection with the server failed...\n");
    go_BYE(-1); 
  }
  else {
    printf("connected to the server..\n");
  }

  uint64_t t_start = RDTSC();

  for ( int i = 0; i < n; i++ ) { 
    sendbuf[i] = 1; // some bogus initialization
  }
  // to tell the server how much to expect
  ((int *)sendbuf)[0] = n;
  for ( int i = 0; i < niters; i++ ) { 
    func(sockfd, sendbuf, rcvbuf, n);
    // printf("Client: Sending %d \n", i);
  }
  uint64_t t_stop = RDTSC();
  printf("Bandwidth in MBytes/sec = %lf \n",
      (niters * n / 1048576.0)/((t_stop - t_start)/1000000.0));
  // close the socket
BYE:
  if ( sockfd < 0 ) { close(sockfd); }
  return status;
}
