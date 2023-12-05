#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "send_request.h"
#include "error_handling.h"
#include "handle_response.h"

#define BUFFER_SIZE 1024
#define MAX_LINE_LENGTH 256

struct sockaddr_in get_server_address(in_port_t portNumber, char *serverIPv4)
{
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address)); // Zero out structure

    // Configure server address
    server_address.sin_family = AF_INET;         // to define the IPv4 family for the address
    server_address.sin_port = htons(portNumber); // to convert our port "8080" from the custom host byte order to the standardized network byte order (big-endian)

    // Convert IP address to binary form and set it in the server address sin_addr attribute
    // and this format is also in network byte order
    int rtnVal = inet_pton(AF_INET, serverIPv4, &server_address.sin_addr);
    if (rtnVal == 0)
    {
        DieWithUserMessage("inet_pton() failed", "invalid address string");
    }

    else if (rtnVal < 0)
    {
        DieWithSystemMessage("inet_pton() failed");
    }
    // if we got here then we've created the server address successfully
    return server_address;
}

void handle_commands(int sock)
{
    FILE *file = fopen("commands.txt", "r");
    if (file == NULL)
    {
        perror("Error opening file");
        exit(1);
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        char method[MAX_LINE_LENGTH];
        char filename[MAX_LINE_LENGTH];

        // Use sscanf to parse the line and extract method and filename
        if (sscanf(line, "%s %s", method, filename) == 2)
        {
            if (strcmp(method, "GET") == 0)
            {
                // then it's a POST
                send_get_request(sock, filename);

                char buffer[BUFFER_SIZE] = {0};
                size_t numBytes = recv(sock, buffer, BUFFER_SIZE, 0);
                if (numBytes < 0)
                {
                    DieWithSystemMessage("recv() failed");
                }
                else if (numBytes == 0)
                {
                    DieWithUserMessage("recv()", "connection closed prematurely");
                }
                handle_get_response(buffer, filename, sock, numBytes);
            }
            else
            {
                // then it's a POST
                send_post_request(sock, filename);

                char buffer[BUFFER_SIZE] = {0};
                size_t numBytes = recv(sock, buffer, BUFFER_SIZE, 0);
                if (numBytes < 0)
                {
                    DieWithSystemMessage("recv() failed");
                }
                else if (numBytes == 0)
                {
                    DieWithUserMessage("recv()", "connection closed prematurely");
                }
                handle_post_response(buffer, filename);
            }
        }
        else
        {
            printf("Invalid line format: %s", line);
            exit(1);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[])
{

    if (argc < 2 || argc > 3) // Test for correct number of arguments
    {
        DieWithUserMessage("Parameter(s)", "<Server Address> [<Server Port>]");
    }

    char *servIP = argv[1];

    in_port_t servPort = (argc == 3) ? atoi(argv[2]) : 8080;

    int sock = 0;                      // integer-valued variable to hold the descriptor of the socket if creted successfully
    struct sockaddr_in server_address; // struct to hold the address information of the remote server (port number, ip address)

    // Attempt to create a TCP socket for IPV4 (AF_INET) address
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        DieWithSystemMessage("socket() failed");
    }
    // now the socket is creted successfully and we have an integer-valued descriptor of it stored in "sock"

    server_address = get_server_address(servPort, servIP); // TODO change the port number and the local IPv4 address of server

    // Attempt to establish a TCP connection to the server
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        DieWithSystemMessage("connect() failed");
    }
    // at this point we've established the connection successfully

    //------------------Here were we send our requests and see what er get from the other side--------------

    handle_commands(sock);

    close(sock);

    return 0;
}
