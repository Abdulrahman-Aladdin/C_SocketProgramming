#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "send_request.h"
#include "error_handling.h"

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 4096

struct sockaddr_in get_server_address(int portNumber, char *serverIPv4)
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

int main()
{
    int sock = 0;                      // integer-valued variable to hold the descriptor of the socket if creted successfully
    struct sockaddr_in server_address; // struct to hold the address information of the remote server (port number, ip address)

    // Attempt to create a TCP socket for IPV4 (AF_INET) address
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        DieWithSystemMessage("socket() failed");
    }
    // now the socket is creted successfully and we have an integer-valued descriptor of it stored in "sock"

    server_address = get_server_address(PORT, SERVER_IP); // TODO change the port number and the local IPv4 address of server

    // Attempt to establish a TCP connection to the server
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        DieWithSystemMessage("connect() failed");
    }
    // at this point we've established the connection successfully

    //------------------Here were we send our requests and see what er get from the other side--------------

    // Send a GET request to the server
    printf("Sending GET request...\n");
    send_get_request(sock, "example_get.txt");

    // Receive and print the server's response
    char buffer[BUFFER_SIZE] = {0};

    // if any message is recieved in the connection then print it and that's the server response
    recv(sock, buffer, BUFFER_SIZE, 0);

    printf("Server response:\n%s\n\n", buffer);

    // TODO do the same for the POST request
    close(sock);

    return 0;
}
