#ifndef REQUEST_HANDLING_H
#define REQUEST_HANDLING_H

void handleGET(int client_sock, char *request);
void handlePOST(int client_sock, char *request);

#endif  // REQUEST_HANDLING_H
