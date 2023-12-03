#include "client_handling.h"
#include <sys/types.h> // for ssize_t
#include <stdio.h>
#include <sys/socket.h> // for recv(), send(), and close()
#include <unistd.h>     // for close()
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "error_handling.h"
#include "request_handling.h"

#define BUFFER_SIZE 16384

void *handleClient(void *args) {
  int clntSock = *(int *)args;
  pthread_detach(pthread_self()); // Detach thread
  char buffer[BUFFER_SIZE];
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
    buffer[numBytesRcvd] = '\0';
    printf("Received from client %d: %s\n", clntSock, buffer);
    char command[5];
    sscanf(buffer, "%4s", command);

    if (strcmp(command, "quit") == 0) {
      printf("Client %d closed connection\n", clntSock);
      break;
    } else if (strcmp(command, "GET") == 0) {
      handleGET(clntSock, buffer);
    } else if (strcmp(command, "POST") == 0) {
      handlePOST(clntSock, buffer);
    } else {
      printf("Invalid command: %s\n", command);
    }

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