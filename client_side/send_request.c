#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include "send_request.h"
#include "error_handling.h"

#define BUFFER_SIZE 1024

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

// fuction to send the Post request
void send_post_request(int socket, const char *filename)
{
    // Determine content type based on file extension
    const char *content_type;
    if (strstr(filename, ".html") || strstr(filename, ".htm"))
    {
        content_type = "text/html";
    }
    else if (strstr(filename, ".txt"))
    {
        content_type = "text/plain";
    }
    else
    {
        content_type = "image/jpeg";
    }

    // Open the file
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        perror("Error opening file, or maybe the file doesn't exist");
        exit(1);
    }

    // Determine the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Construct and send the HTTP POST request headers
    char headers[BUFFER_SIZE];
    snprintf(headers, sizeof(headers),
             "POST /%s HTTP/1.1\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %ld\r\n"
             "\r\n",
             filename, content_type, file_size);

    // the first send(the one that contains the header and the request line)
    ssize_t bytes_sent = send(socket, headers, strlen(headers), 0);

    if (bytes_sent == -1)
    {
        DieWithSystemMessage("send() failed");
    }

    if (bytes_sent != strlen(headers))
    {
        DieWithUserMessage("send()", "sent unexpected number of bytes");
    }

    // Send the file content in chunks of 1024 bytes
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0)
    {
        ssize_t bytes_sent = send(socket, buffer, bytes_read, 0);
        if (bytes_sent == -1)
        {
            perror("Error sending file content");
            break;
        }
    }

    // Close the file
    fclose(file);
}
