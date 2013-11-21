#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <mysql/mysql.h>
#include <pthread.h>

#define NUM_THREADS     5
 

using namespace std;

int main(int argc, char **argv)
{
	MYSQL_RES *mTabelle;
    MYSQL_ROW mRecord;
    MYSQL *mConnection, mDB;
    int mError;
    mysql_init(&mDB);
    mConnection =
		mysql_real_connect(&mDB,"localhost","main","437280","main",3306,0,0);
    if (mConnection == NULL)  {
        printf("Could not connect. Leaving: %s",mysql_error(&mDB));
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
    
    while((sock = accept(sock_server, (struct sockaddr*)&addr, &addr_len)) >= 0)
    {
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
    return 0;
}

