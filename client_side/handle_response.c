#include <stdio.h>
#include <string.h>
#include "handle_response.h"

// function to handle the GET response
void handle_get_response(char *response, char *file_name)
{
    // either OK or NOT FOUND
    const char *found = strstr(response, "HTTP/1.1 200 OK");

    if (found == NULL)
    {
        printf("%s, is not found at the server\n", file_name);
        return;
    }
    // here means that we've got our file successfully so we just need to creat the file

    // our content is right after two new lines from the start
    const char *contentStart = strstr(response, "\n\n");

    if (contentStart == NULL)
    {
        printf("Invalid HTTP response format from the server.\n");
        return;
    }

    // Increment contentStart to skip the two newline characters
    contentStart += 2;

    // Call the createFile function with the specified file name and content
    createFile(file_name, contentStart);
}

// function to create a file named filename and fill it with content
void createFile(const char *filename, const char *content)
{
    FILE *file = fopen(filename, "w");

    if (file == NULL)
    {
        perror("Error creating file");
        return;
    }

    // Write the content to the file
    fprintf(file, "%s", content);

    // Close the file
    fclose(file);
}

// function to handle the POST response
void handle_post_response(char *response, char *file_name)
{
    const char *sent = strstr(response, "HTTP/1.1 200 OK");

    if (sent == NULL)
    {
        printf("%s, is not sent successfully to the server\n", file_name);
        return;
    }

    printf("%s, is sent successfully to the server\n", file_name);
}