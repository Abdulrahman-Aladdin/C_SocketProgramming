#include "client_handling.h"
#include <sys/types.h> // for ssize_t
#include <stdio.h>
#include <sys/socket.h> // for recv(), send(), and close()
#include <unistd.h>     // for close()
#include <pthread.h>
#include "error_handling.h"


void *handleClient(void *args) {
  int clntSock = *(int *)args;
  pthread_detach(pthread_self()); // Detach thread
  char buffer[BUFSIZ];
  ssize_t numBytesRcvd = recv(clntSock, buffer, BUFSIZ, 0);
  
  if (numBytesRcvd < 0) {
    perror("recv() failed");
    return NULL;
  }

  if (numBytesRcvd == 0) {
    printf("Client %d closed connection\n", clntSock);
    return NULL;
  }

  while (numBytesRcvd > 0) {
    ssize_t numBytesSent = send(clntSock, buffer, numBytesRcvd, 0);
    if (numBytesSent < 0)
      DieWithSystemMessage("send() failed");

    numBytesRcvd = recv(clntSock, buffer, BUFSIZ, 0);
    if (numBytesRcvd < 0) {
      perror("recv() failed");
      break;
    }
    if (numBytesRcvd == 0) {
      printf("Client %d closed connection\n", clntSock);
      break;
    }
  }

  close(clntSock); // Close client socket
  return NULL;
}