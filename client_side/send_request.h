#ifndef SEND_REQUEST_H
#define SEND_REQUEST_H

// the prototype of the send_get_request function
void send_get_request(int socket, const char *file_path);

// the prototype of the send_post_request function
void send_post_request(int socket, const char *file_path, const char *file_content);

#endif