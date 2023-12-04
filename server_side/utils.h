#ifndef UTILS_H
#define UTILS_H

static const int MAX_TIMEOUT = 200; // Timeout in seconds
static const int MIN_TIMEOUT = 30;       // Timeout in seconds
static const int TIMEOUT_USEC = 0;  // Timeout in microseconds

void decrement_active_clients(int *activeClients);
void update_timeout(struct timeval *timeout, int activeClients);

#endif  // UTILS_H