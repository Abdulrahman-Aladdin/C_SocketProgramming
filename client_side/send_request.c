#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include "error_handling.h"

#define BUFFER_SIZE 4096

// Function to send a GET request to the server
void send_get_request(int socket, const char *file_path)
{
    char get_request[BUFFER_SIZE];
    // Format the GET request ended by /r/n/r/n double CRLFs to indicate the end of the request line and headers
    sprintf(get_request, "GET /%s HTTP/1.1\r\n\r\n", file_path);

    // send the data over the TCP socket
    ssize_t bytes_sent = send(socket, get_request, strlen(get_request), 0);

    if (bytes_sent == -1)
    {
        DieWithSystemMessage("send() failed");
    }

    if (bytes_sent != strlen(get_request))
    {
        DieWithUserMessage("send()", "sent unexpected number of bytes");
    }
    // here means that we've successfully sent our GET request
}

// Function to send a POST request to the server
void send_post_request(int socket, const char *file_path, const char *file_content)
{
    // it's pretty much as the previous function but here we include the content of the file that we need to
    // send to the server

    char post_request[BUFFER_SIZE];
    sprintf(post_request, "POST /%s HTTP/1.1\r\n\r\n%s", file_path, file_content);

    // send the data through the socket and see how many bytes are exactly sent
    ssize_t bytes_sent = send(socket, post_request, strlen(post_request), 0);

    if (bytes_sent == -1)
    {
        DieWithSystemMessage("send() failed");
    }

    if (bytes_sent != strlen(post_request))
    {
        DieWithUserMessage("send()", "sent unexpected number of bytes");
    }
    // here means that we've successfully sent our POST request
}