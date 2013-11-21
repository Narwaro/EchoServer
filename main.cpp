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
#include <mysql/mysql.h>

using namespace std;

struct Connection
{
    int sock;
    sockaddr_in addr;
};

string itoa(double i)
{
    stringstream ss;
    ss << i;
    return ss.str();
}

void *thread_function( void *ptr );
MYSQL *mConnection;

int main()
{
		
	MYSQL_RES *mTabelle;
    MYSQL_ROW mRecord;
    int mError;
    mConnection = mysql_init(NULL);
	mysql_real_connect(mConnection,"localhost","main","437280","main",3306,0,0);
		
    if (mConnection == NULL)  {
        printf("Could not connect. Leaving: %s",mysql_error(mConnection));
        return 1;
    } else {
		printf("Connected to MySQL-Server!\n");
    }
    //mysql_close(mConnection);
	int result = mysql_query(mConnection, "SELECT * FROM log ORDER BY id DESC LIMIT 0,10");
    if(result == 0) {
		cout << "Query succeeded.\n" << endl; 
		MYSQL_RES* res = mysql_store_result(mConnection);
		MYSQL_ROW row = mysql_fetch_row(res);
		for(int i = 0; i < mysql_num_fields(res); i++ ) {
			cout << row[i] << "|";
		}
		cout << "\n" << endl;
	} else {
		cout << "ERROR!\n" << mysql_error(mConnection) << endl;
	}	
	

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
	Connection c;
																						    
	while((sock = accept(sock_server, (struct sockaddr*)&addr, &addr_len)) >= 0)
	{
		c.sock = sock;
		c.addr = addr;
		pthread_t thread;
		pthread_create(&thread, NULL, thread_function, &c);
		char buf[4096];
		string sendback;
		int res;
		memset(buf, 0, 4096);	
		//BLA			
	}
}

void *thread_function( void *ptr )
{
	Connection c = *(Connection*)ptr;
	
	char buf[4096];
    string sendback;
    int res;
    memset(buf, 0, 4096);
	
	while((res = recv(c.sock, &buf, 4096, 0)) > 0) {
		
		string data = buf;
		cout << "\"" << buf << "\"\n" << endl;
			
		stringstream ss;
		ss
		<< (c.addr.sin_addr.s_addr & 0x000000FF) << "."
		<< ((c.addr.sin_addr.s_addr & 0x0000FF00) >> 8) << "."
		<< ((c.addr.sin_addr.s_addr & 0x00FF0000) >> 16) << "."
		<< ((c.addr.sin_addr.s_addr & 0xFF000000) >> 24);

		//Unique client ID is 32bit hex string. Client is sending ID in front of the request.
		string cid = data.substr(0,32);
		if(buf[32] == ':') {
			if(cid != "00000000000000000000000000000000") {
				string str;
				string array = "abcdef0123456789";
				for(int i = 0; i < 32; i++) {
					int num = rand() % 16;
					str += array[num];
				}
				int result = mysql_query(mConnection, ("INSERT INTO clients (ip, date, joined, cid) VALUES ('"+ss.str()+"','"+itoa(time(NULL))+"','"+itoa(time(NULL))+"','"+str+"')").c_str());
				if(result == 0) {
					sendback = string() + "#210-OK: id:" + str;
		            send(c.sock, sendback.c_str(), sendback.size(), 0);
				} else {
					sendback = string() + "#510-ER: Query failed." + str;
	                send(c.sock, sendback.c_str(), sendback.size(), 0);
				}
			} else {
				
				int result = mysql_query(mConnection, ("SELECT * FROM clients WHERE cid = '" + cid + "'").c_str());
				MYSQL_RES* res = mysql_store_result(mConnection);
	
				if(mysql_num_rows(res) == 0) {
					//MYSQL_ROW row = mysql_fetch_row(res);
				
				}
			}
		
		} else {
			sendback = string() + "#500-ER: Wrong request sent.";
            send(c.sock, sendback.c_str(), sendback.size(), 0);
		}
		
		if(string(buf) == "\n") {
			sendback = string() + "#300-ER: Empty String given.";
			send(c.sock, sendback.c_str(), sendback.size(), 0);
				
		} else if(string(buf) == "ping") {
			sendback = string()+ "#200-OK: pong";
			send(c.sock, sendback.c_str(), sendback.size(), 0);
				
		}
		memset(buf, 0, 4096);
	}
    close(c.sock);
	
}
