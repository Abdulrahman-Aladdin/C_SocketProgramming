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
#include "params_struct.h"
#include "utils.h"

#define BUFFER_SIZE 1024

void *handleClient(void *args)
{
  struct params *params = (struct params *)args;
  int clntSock = params->clntSock;
  pthread_detach(pthread_self()); // Detach thread
  char buffer[BUFFER_SIZE];

  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(clntSock, &readfds);
  int result = select(clntSock + 1, &readfds, NULL, NULL, params->timeout);

  if (result < 0)
  {
    perror("select() failed");
    decrement_active_clients(params->activeClients);
    update_timeout(params->timeout, *(params->activeClients));
    close(clntSock);
    return NULL;
  }

  if (result == 0)
  {
    printf("Client %d timed out\n", clntSock);
    decrement_active_clients(params->activeClients);
    update_timeout(params->timeout, *(params->activeClients));
    close(clntSock);
    return NULL;
  }

  ssize_t numBytesRcvd = recv(clntSock, buffer, BUFFER_SIZE, 0);

  if (numBytesRcvd < 0)
  {
    decrement_active_clients(params->activeClients);
    update_timeout(params->timeout, *(params->activeClients));
    perror("recv() failed");
    close(clntSock);
    return NULL;
  }

  if (numBytesRcvd == 0)
  {
    printf("Client %d closed connection\n", clntSock);
    decrement_active_clients(params->activeClients);
    update_timeout(params->timeout, *(params->activeClients));
    close(clntSock);
    return NULL;
  }

  while (numBytesRcvd > 0)
  {
    buffer[numBytesRcvd] = '\0';
    printf("Received from client %d: %s\n", clntSock, buffer);
    char command[5];
    sscanf(buffer, "%4s", command);

    if (strcmp(command, "quit") == 0)
    {
      printf("Client %d closed connection\n", clntSock);
      break;
    }
    else if (strcmp(command, "GET") == 0)
    {
      handleGET(clntSock, buffer);
    }
    else if (strcmp(command, "POST") == 0)
    {
      handlePOST(clntSock, buffer, numBytesRcvd);
    }
    else
    {
      printf("Invalid command: %s\n", command);
    }

    result = select(clntSock + 1, &readfds, NULL, NULL, params->timeout);

    if (result < 0)
    {
      perror("select() failed");
      break;
    }

    if (result == 0)
    {
      printf("Client %d timed out\n", clntSock);
      break;
    }

    numBytesRcvd = recv(clntSock, buffer, BUFSIZ, 0);
    if (numBytesRcvd < 0)
    {
      perror("recv() failed");
      break;
    }
    if (numBytesRcvd == 0)
    {
      printf("Client %d closed connection\n", clntSock);
      break;
    }
  }
  decrement_active_clients(params->activeClients);
  update_timeout(params->timeout, *(params->activeClients));
  close(clntSock); // Close client socket
  return NULL;
}