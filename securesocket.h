#ifndef SECURESOCKET_H_INCLUDED
#define SECURESOCKET_H_INCLUDED

#include "aes256.h"

struct secure_socket
{
    int fd;
    aes256_context ctx;
    char* buf;
};

bool secure_init(struct secure_socket* sock, int fd, unsigned char* key);
void secure_close(struct secure_socket* sock);
int secure_read(struct secure_socket* sock);
int secure_send(struct secure_socket* sock, const char* data, short size);

#endif //SECURESOCKET_H_INCLUDED
