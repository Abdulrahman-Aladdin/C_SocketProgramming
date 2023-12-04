#include <sys/time.h>

struct params {
  int clntSock;
  struct timeval *timeout;
  int *activeClients;
};