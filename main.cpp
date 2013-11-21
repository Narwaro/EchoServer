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
#include <mysql/mysql.h>

using namespace std;

void *thread_function( void *ptr );

int main()
{
	int sock_server = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_server < 0)
		{
		    cout << "Error: Could not create socket!" << endl;
		    return 1;
		}
	int opt = 1;
	setsockopt(sock_server, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

	struct sockaddr_in addr_server;
	memset(&addr_server, 0, sizeof(sockaddr_in));

	addr_server.sin_family = AF_INET;
	addr_server.sin_addr.s_addr = INADDR_ANY;
	addr_server.sin_port = htons(22222);


	if(bind(sock_server, (struct sockaddr*)&addr_server, sizeof(sockaddr_in)) < 0)
	{
	    cout << "Error: Could not bind socket!" << endl;
	    return 1;
	}
	listen(sock_server, 5);
	cout << "Server started!" << endl;

	int sock;
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(sockaddr_in);
																						    
	while((sock = accept(sock_server, (struct sockaddr*)&addr, &addr_len)) >= 0)
	{
		pthread_t thread;
		pthread_create(&thread, NULL, thread_function, &sock);
		char buf[4096];
		string sendback;
		int res;
		memset(buf, 0, 4096);	
		//BLA			
	}
}

void *thread_function( void *ptr )
{
	int sock = *(int*)ptr;
	
	char buf[4096];
    string sendback;
    int res;
    memset(buf, 0, 4096);
	
	while((res = recv(sock, &buf, 4096, 0)) > 0) {

		cout << "\"" << buf << "\"" << endl;

		if(string(buf) == "\n") {
			sendback = string() + "#300-ERROR: Empty String given." + buf;
			send(sock, sendback.c_str(), sendback.size(), 0);
				
		} else {
			sendback = string()+ "#200-OK: " + buf;
			send(sock, sendback.c_str(), sendback.size(), 0);
				
		}
		memset(buf, 0, 4096);
	}
    close(sock);
	
}

