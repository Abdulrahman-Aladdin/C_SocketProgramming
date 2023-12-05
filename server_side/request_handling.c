#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "request_handling.h"
#include "error_handling.h"
#include <stdio.h>

#define MAX_FILE_NAME 256
#define BUFFER_SIZE 1024

void send_data(int clntSock, char *filename);

// Handle GET request
void handleGET(int clntSock, char *buffer)
{
  char filename[MAX_FILE_NAME];
  sscanf(buffer, "GET /%s", filename);
  // printf("GET %s\n", filename);
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL)
  {
    char *msg = "HTTP/1.1 404 Not Found\r\n\r\n";
    send(clntSock, msg, strlen(msg), 0);
    return;
  }
  fclose(fp);

  send_data(clntSock, filename);
}

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

// Handle POST request the same way as handling Get response
void handlePOST(int clntSock, char *buffer, size_t bytesRecievedInitially)
{
  char filename[MAX_FILE_NAME];
  sscanf(buffer, "POST /%s", filename);
  // printf("POST %s\n", filename);

  size_t content_length;

  if (extractHeaderValue(buffer, "Content-Length", &content_length) == 0)
  {
    DieWithUserMessage("extractHeaderValue()", "invalid first response format");
  }
  // pointer to point to the start of the data
  char *initially_recieved = strstr(buffer, "\r\n\r\n") + 4;

  // this gives the exact size of the initially recieved data in that are sent along with the headers
  unsigned int initially_recieved_length = bytesRecievedInitially - (initially_recieved - buffer);

  // prepare the totalBytesRecieved such that we now got the initial data
  unsigned int totalBytesRcvd = initially_recieved_length;

  // let's open the file to right the data to it
  FILE *fp = fopen(filename, "wb");

  fwrite(initially_recieved, 1, initially_recieved_length, fp);

  if (fp == NULL)
  {
    char *msg = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    send(clntSock, msg, strlen(msg), 0);
    return;
  }

  while (totalBytesRcvd < content_length)
  {
    char buffer[BUFFER_SIZE];
    size_t numBytes = recv(clntSock, buffer, BUFFER_SIZE, 0);

    if (numBytes == 0)
      DieWithUserMessage("recv()", "connection closed prematurely");

    totalBytesRcvd += numBytes;
    fwrite(buffer, 1, numBytes, fp);
  }
  // here means that we've successfully recieved the whole file
  // now we close our file
  fclose(fp);

  // and then notify the client that everything went well
  char *msg = "HTTP/1.1 200 OK\r\n\r\n";
  send(clntSock, msg, strlen(msg), 0);
}

// Send data to client in case of GET request
void send_data(int clntSock, char *filename)
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

  FILE *file = fopen(filename, "rb");
  // Determine the file size
  fseek(file, 0, SEEK_END);
  long int file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  fclose(file);

  // Construct and send the HTTP POST request headers
  char headers[BUFFER_SIZE];
  snprintf(headers, sizeof(headers),
           "HTTP/1.1 200 OK\r\n"
           "Content-Type: %s\r\n"
           "Content-Length: %ld\r\n"
           "\r\n",
           content_type, file_size);

  // the first send(the one that contains the header and the request line)
  // printf("Headers is: %s\n", headers);
  // printf("the header length is: %zu", strlen(headers));
  ssize_t bytes_sent = send(clntSock, headers, strlen(headers), 0);

  if (bytes_sent == -1)
  {
    DieWithSystemMessage("send() failed");
  }

  if (bytes_sent != (int)strlen(headers))
  {
    DieWithUserMessage("send()", "sent unexpected number of bytes");
  }

  file = fopen(filename, "rb");

  // Send the file content in chunks of 1024 bytes
  char buffer[BUFFER_SIZE];
  size_t bytes_read;

  while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0)
  {
    // printf("bytes read = %zu\n", bytes_read);
    // printf("buffer is: %s\n", buffer);
    ssize_t bytes_sent = send(clntSock, buffer, bytes_read, 0);
    if (bytes_sent == -1)
    {
      perror("Error sending file content");
      break;
    }
  }
  // Close the file
  fclose(file);
}
