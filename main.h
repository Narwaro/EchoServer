#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <time.h>
#include <vector>
#include <fstream>
#include <mysql/mysql.h>
#include "rsa.h"
#include "padding.h"
#include "aes256.h"

using namespace std;
using namespace YansLibrary;

struct Connection
{ 
    int sock;
	string client_id;
    sockaddr_in addr;
	int pipefd[2];
	int pipefd_sync[2];
	char* buf;
};

//pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *thread_function( void *ptr );
void *client_thread( void *ptr );
void *client_thread_function( void *ptr );
int readline ( int socket, string& str );

string itoa(long i);
int update_client();
MYSQL* mysql();
int secure_send(int sock, aes256_context* ctx, const char* data, int size);
int secure_read(int sock, aes256_context* ctx, char** data);
