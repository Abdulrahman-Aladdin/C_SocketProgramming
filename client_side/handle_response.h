#ifndef HANDLE_RESPONSE_H
#define HANDLE_RESPONSE_H

void handle_get_response(char *response, char *file_name);

void createFile(const char *filename, const char *content);

void handle_post_response(char *response, char *file_name);

#endif // HANDLE_RESPONSE_H