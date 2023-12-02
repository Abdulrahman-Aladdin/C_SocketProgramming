#include "client_handling.h"
#include <sys/types.h> // for ssize_t
#include <stdio.h>
#include <sys/socket.h> // for recv(), send(), and close()
#include <unistd.h>     // for close()
#include "error_handling.h"


void handleClient(int clntSock) {
  char buffer[BUFSIZ];
  ssize_t numBytesRcvd = recv(clntSock, buffer, BUFSIZ, 0);
  if (numBytesRcvd < 0)
    DieWithSystemMessage("recv() failed");

  while (numBytesRcvd > 0) {
    ssize_t numBytesSent = send(clntSock, buffer, numBytesRcvd, 0);
    if (numBytesSent < 0)
      DieWithSystemMessage("send() failed");

    numBytesRcvd = recv(clntSock, buffer, BUFSIZ, 0);
    if (numBytesRcvd < 0)
      DieWithSystemMessage("recv() failed");
  }

  close(clntSock); // Close client socket
}