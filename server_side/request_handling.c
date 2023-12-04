#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "request_handling.h"
#include <stdio.h>

#define MAX_FILE_NAME 256
#define BUFFER_SIZE 16384


void write_data(int clntSock, FILE *fp, char *buffer, char *filename);
void send_data(int clntSock, FILE *fp);

// Handle GET request
void handleGET(int clntSock, char *buffer) {
  char filename[MAX_FILE_NAME];
  sscanf(buffer, "GET %s", filename);
  printf("GET %s\n", filename);
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL) {
    char *msg = "HTTP/1.1 404 Not Found\r\n\r\n";
    send(clntSock, msg, strlen(msg), 0);
    return;
  }
  send_data(clntSock, fp);
}

// Handle POST request
void handlePOST(int clntSock, char *buffer) {
  char filename[MAX_FILE_NAME];
  sscanf(buffer, "%*s %s", filename);
  printf("POST %s\n", filename);
  FILE *fp = fopen(filename, "wb");
  if (fp == NULL) {
    char *msg = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    send(clntSock, msg, strlen(msg), 0);
    return;
  }
  write_data(clntSock, fp, buffer, filename);
}


// Send data to client in case of GET request
void send_data(int clntSock, FILE *fp) {
  char *msg = "HTTP/1.1 200 OK\r\n\r\n";
  send(clntSock, msg, strlen(msg), 0);

  // determine file size
  fseek(fp, 0L, SEEK_END);
  long fsize = ftell(fp);
  rewind(fp);

  char *buffer = (char *)malloc(fsize + 1);

  if (buffer == NULL) {
    char *msg = "HTTP/1.1 500 Internal Server Error\r\n\r\n";
    send(clntSock, msg, strlen(msg), 0);
    perror("malloc failed");
    return;
  }

  fread(buffer, fsize, 1, fp);
  buffer[fsize] = '\0';
  fclose(fp);

  send(clntSock, buffer, fsize, 0);
}


// Write data to file in case of POST request
void write_data(int clntSock, FILE *fp, char *buffer, char *filename) {
  char *msg = "HTTP/1.1 200 OK\r\n\r\n";
  send(clntSock, msg, strlen(msg), 0);

  // Skip the first line
  int t_size = 4 + 1 + strlen(filename) + 1 + 8 + 4;

  char body[BUFFER_SIZE];
  strcpy(body, buffer + t_size + 4);
  if (body == NULL) {
    printf("No body\n");
    return;
  }
  for (int i = 0; i < (int)strlen(body); i++) {
    if (body[i] == '\\' && body[i + 1] == 'n') {
      fputs("\n", fp);
      i++;
    } else {
      fputc(body[i], fp);
    }
  }
  fclose(fp);
}