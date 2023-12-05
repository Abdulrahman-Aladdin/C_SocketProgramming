#include <stdio.h>      // for printf() and fprintf()
#include <stdlib.h>     // for atoi()
#include <string.h>     // for memset()
#include <sys/types.h>  // for type definitions
#include <sys/socket.h> // for socket(), bind(), and connect()
#include <netinet/in.h> // for sockaddr_in and inet_ntoa()
#include <arpa/inet.h>  // for inet_ntop()
#include <unistd.h>     // for close()
#include <pthread.h>
#include <sys/time.h>
#include "error_handling.h"
#include "client_handling.h"
#include "params_struct.h"
#include "utils.h"

static const int MAXPENDING = 5; // Maximum outstanding connection requests

struct sockaddr_in get_sockAddr_in(int port)
{
  struct sockaddr_in servAddr;                  // Local address
  memset(&servAddr, 0, sizeof(servAddr));       // Zero out structure
  servAddr.sin_family = AF_INET;                // IPv4 address family
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
  servAddr.sin_port = htons(port);              // Local port
  return servAddr;
}

int main(int argc, char *argv[])
{
  if (argc != 2) // Test for correct number of arguments
    DieWithUserMessage("Parameter(s)", "<Server Port>");

  in_port_t servPort = atoi(argv[1]); // First arg: local port
  int servSock;                       // Socket descriptor for server
  if ((servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    DieWithSystemMessage("socket() failed");

  // Construct local address structure
  struct sockaddr_in servAddr = get_sockAddr_in(servPort);

  // Bind to the local address
  if (bind(servSock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
    DieWithSystemMessage("bind() failed");

  // Mark the socket so it will listen for incoming connections
  if (listen(servSock, MAXPENDING) < 0)
    DieWithSystemMessage("listen() failed");

  int activeClients = 0;
  struct timeval timeout;

  while (1)
  {

    puts("Waiting for client to connect...");

    struct sockaddr_in clntAddr; // Client address
    // Set length of client address structure (in-out parameter)
    socklen_t clntAddrLen = sizeof(clntAddr);

    // Wait for a client to connect
    int clntSock = accept(servSock, (struct sockaddr *)&clntAddr, &clntAddrLen);
    if (clntSock < 0)
    {
      perror("accept() failed");
      continue;
    }

    // clntSock is connected to a client!
    char clntName[INET_ADDRSTRLEN]; // String to contain client address
    if (inet_ntop(AF_INET, &clntAddr.sin_addr.s_addr, clntName, sizeof(clntName)) != NULL)
      printf("Handling client %s/%d\n", clntName, ntohs(clntAddr.sin_port));
    else
      puts("Unable to get client address");

    activeClients++;
    printf("...Active clients -> %d\n", activeClients);

    update_timeout(&timeout, activeClients);

    struct params *args = (struct params *)malloc(sizeof(struct params));
    args->clntSock = clntSock;
    args->timeout = &timeout;
    args->activeClients = &activeClients;

    pthread_t threadID;
    int returnValue = pthread_create(&threadID, NULL, handleClient, (void *)args);
    if (returnValue != 0)
    {
      perror("pthread_create() failed");
      activeClients--;
      close(clntSock);
      continue;
    }

    if (pthread_detach(threadID) != 0)
    {
      perror("pthread_detach() failed");
      activeClients--;
      close(clntSock);
      continue;
    }
  }
}
