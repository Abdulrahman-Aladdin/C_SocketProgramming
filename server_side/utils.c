#include <sys/time.h>
#include <stdio.h>
#include "utils.h"

void decrement_active_clients(int *activeClients) {
  (*activeClients)--;
  printf("active clients -> %d\n", *activeClients);
}

void update_timeout(struct timeval *timeout, int activeClients) {
  if (activeClients < 2) {
    timeout->tv_sec = MAX_TIMEOUT;
    timeout->tv_usec = TIMEOUT_USEC;
  } else {
    timeout->tv_sec = MAX_TIMEOUT / activeClients;
    timeout->tv_usec = 0;
  }
  if (timeout->tv_sec < MIN_TIMEOUT) {
    timeout->tv_sec = MIN_TIMEOUT;
    timeout->tv_usec = TIMEOUT_USEC;
  }
  printf("...udated timeout -> %ld sec.\n", timeout->tv_sec);
}