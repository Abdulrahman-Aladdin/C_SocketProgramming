#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "handle_response.h"
#include "error_handling.h"

#define BUFFER_SIZE 1024

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
void handle_get_response(char *response, char *file_name, int sock, size_t responseLength)
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

    // pointer to point to the start of the data
    char *initially_recieved = strstr(response, "\r\n\r\n") + 4;

    // this gives the exact size of the initially recieved data in that are sent along with the headers
    unsigned int initially_recieved_length = responseLength - (initially_recieved - response);

    // prepare the totalBytesRecieved such that we now got the initial data
    unsigned int totalBytesRcvd = initially_recieved_length;

    // let's open the file to right the data to it
    FILE *fp = fopen(file_name, "wb");

    if (fp == NULL)
    {
        DieWithSystemMessage("Error opening the file for writing");
    }

    // write the initially recieved data
    fwrite(initially_recieved, 1, initially_recieved_length, fp);

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