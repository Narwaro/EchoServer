#include "securesocket.h"

#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
using namespace std;

bool secure_init(struct secure_socket* sock, int fd, unsigned char* key)
{
    sock->fd = fd;
    aes256_init(&sock->ctx, key);
    sock->buf = 0;
}

void secure_close(struct secure_socket* sock)
{
    if(!sock) return;
    close(sock->fd);
    sock->fd = 0;
    aes256_done(&sock->ctx);
    if(sock->buf) free(sock->buf);
    sock->buf = 0;
}

int secure_read(struct secure_socket* sock)
{
    //clear buffer
    if(sock->buf)
    {
        free(sock->buf);
        sock->buf = 0;
    }

    //read size
    short i, rdc, size, bufsize;
    unsigned char buf[32];
	i = 0;
	while(i < 32)
	{
		if((rdc = recv(sock->fd, (char*)buf + i, 32 - i, 0)) == -1)
			return -1;
		i += rdc;
	}
    aes256_decrypt_ecb(&sock->ctx, buf);
    size = *(short*)&buf[0];
    for(int i = 1; i < 32 / sizeof(size); i++)
        if(((short*)buf)[i] != (short)(size * (i + 1)))
            return -1;
    if(size < 0) return -1;
    if(!size) return 0;
    bufsize = (size & (~(short)31)) + !!(size & 31) * 32;
    sock->buf = (char*)malloc(bufsize);

    //read data
	i = 0;
    while(i < bufsize)
    {
        if((rdc = recv(sock->fd, sock->buf + i, bufsize - i, 0)) == -1)
        {
            free(sock->buf);
            sock->buf = 0;
            return -1;
        }
		i += rdc;
    }

    //decrypt data
    for(i = 0; i < bufsize; i += 32)
        aes256_decrypt_ecb(&sock->ctx, (unsigned char*)sock->buf + i);

    //return size
    return size;
}

int secure_send(struct secure_socket* sock, const char* data, short size)
{
    //send size
    unsigned char buf[4096];
    memset(buf, 0, 32);
    memcpy(buf, (char*)&size, sizeof(size));
    for(int i = 1; i < 32 / sizeof(size); i++)
        ((short*)buf)[i] = (short)(size * (i + 1));
    aes256_encrypt_ecb(&sock->ctx, buf);
    send(sock->fd, (char*)buf, 32, 0);

    //encrypt & send data
    int bufsize = 0;
    for(int i = 0; i < size; i += 32)
    {
        //copy data to buffer
        if(size - i >= 32)
            memcpy(&buf[bufsize], &data[i], 32);
        else
        {
            memset(&buf[bufsize], 0, 32);
            memcpy(&buf[bufsize], &data[i], size - i);
        }
        //encrypt
        aes256_encrypt_ecb(&sock->ctx, (unsigned char*)buf + bufsize);
        bufsize += 32;
        //send in 4kb blocks
        if(bufsize == 4096 || i + 32 >= size)
        {
            if(send(sock->fd, (char*)buf, bufsize, 0) == -1)
                return -1;
            bufsize = 0;

        }
    }
    return size;
}
