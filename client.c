#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
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
        perror("send() failed");
        exit(EXIT_FAILURE);
    }

    if (bytes_sent != strlen(get_request))
    {
        perror("send(), sent unexpected number of bytes");
        exit(EXIT_FAILURE);
    }
    // here means that we've successfully send our GET request
}

// Function to send a POST request to the server
void send_post_request(int socket, const char *file_path, const char *file_content)
{
    // TO be done
}

int main()
{
    int sock = 0;                      // integer-valued variable to hold the descriptor of the socket if creted successfully
    struct sockaddr_in server_address; // struct to hold the address information of the remote server (port number, ip address)

    // Attempt to create a TCP socket for IPV4 (AF_INET) address
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket() failed: Socket creation error");
        exit(EXIT_FAILURE);
    }
    // now the socket is creted successfully and we have an integer-valued descriptor of it stored in "sock"

    // Configure server address
    server_address.sin_family = AF_INET;   // to define the IPv4 family for the address
    server_address.sin_port = htons(PORT); // to convert our port "8080" from the custom host byte order to the standardized network byte order (big-endian)

    char *serverIpAddress = "127.0.0.1"; // variable to hold the ip address of the server (can change)

    // Convert IP address to binary form and set it in the server address sin_addr attribute
    // and this format is also in network byte order
    if (inet_pton(AF_INET, serverIpAddress, &server_address.sin_addr) <= 0)
    {
        perror("inet_pton() failed: Invalid address string/Address not supported");
        exit(EXIT_FAILURE);
    }

    // Attempt to establish a TCP connection to the server
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("connect() faild: Connection Failed");
        exit(EXIT_FAILURE);
    }
    // at this point we've established the connection successfully

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
