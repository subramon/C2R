#include "incs.h"
#include "aux_rcli.h"

int
main(
    int argc,
    char **argv
    )
{
  int status = 0;
  int sock;
  const char * server = NULL; int portnum;

  if ( argc == 3 ) {
    server  = argv[1];
    portnum = atoi(argv[2]);
  }
  else {
    server = "127.0.0.1";
    portnum = 6311;
  }
  status = rconnect(server, portnum, 0, 0, &sock); cBYE(status);
  printf("Established connection to %s:%d\n", server, portnum);
  status = rshutdown(sock); cBYE(status);
  printf("Shut down server\n"); 
BYE:
  return status;
}
