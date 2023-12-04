#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "handle_response.h"
#include "error_handling.h"

#define BUFFER_SIZE 1024

// void printHex(const char *data, size_t size)
// {
//     for (size_t i = 0; i < size; ++i)
//     {
//         printf("%02X ", (unsigned char)data[i]);
//     }
//     printf("\n");
// }

int extractHeaderValue(const char *response, const char *headerField, size_t *value)
{
    const char *headerStart = strstr(response, headerField);
    if (headerStart == NULL)
    {
        // Header field not found
        return 0;
    }

    // Find the colon after the header field
    const char *colon = strchr(headerStart, ':');
    if (colon == NULL)
    {
        // Malformed header
        return 0;
    }

    // Parse the value after the colon
    if (sscanf(colon + 1, " %zu", value) != 1)
    {
        // Parsing failed
        return 0;
    }

    return 1; // Successfully extracted and parsed the value
}

// function to handle the GET response
void handle_get_response(char *response, char *file_name, int sock)
{
    // either OK or NOT FOUND
    const char *found = strstr(response, "HTTP/1.1 200 OK");

    if (found == NULL)
    {
        printf("%s, is not found at the server\n", file_name);
        return;
    }
    // here means that we've got our file successfully so we just need to creat the file

    // first we get the content length from the content length field
    size_t content_length;

    if (extractHeaderValue(response, "Content-Length", &content_length) == 0)
    {
        DieWithUserMessage("extractHeaderValue()", "invalid first response format");
    }
    // here means that we got out content_length value successfully

    unsigned int totalBytesRcvd = 0;

    FILE *fp = fopen(file_name, "wb");

    if (fp == NULL)
    {
        DieWithSystemMessage("Error opening the file for writing");
    }

    while (totalBytesRcvd < content_length)
    {
        char buffer[BUFFER_SIZE];
        size_t numBytes = recv(sock, buffer, BUFFER_SIZE, 0);

        if (numBytes < 0)
            DieWithSystemMessage("recv() failed");

        else if (numBytes == 0)
            DieWithUserMessage("recv()", "connection closed prematurely");

        totalBytesRcvd += numBytes;
        fwrite(buffer, 1, numBytes, fp);
    }
    // here means that we've successfully recieved the whole file
    // now we close our file
    fclose(fp);
}

// function to handle the POST response
void handle_post_response(char *response, char *file_name)
{
    const char *sent = strstr(response, "HTTP/1.1 200 OK");

    if (sent == NULL)
    {
        printf("%s is not sent successfully to the server\n", file_name);
        return;
    }

    printf("%s is sent successfully to the server\n", file_name);
}