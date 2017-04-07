/* 
This code primarily comes from 
http://www.prasannatech.net/2008/07/socket-programming-tutorial.html
and
http://www.binarii.com/files/papers/c_sockets.txt
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <boost/algorithm/string.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string>
#include <iostream>
#include <ctime>
#include <vector>
using namespace std;
using namespace boost;

int initUSB(char *argv);
void* reading(void* p);
void quit();
void setCF();
string getJason();
bool canGetT();

int start_server(int);
void* Quit(void*);

bool running;

int main(int argc, char *argv[])
{
	if (argc != 3) {
    		cout << endl << "Usage: server [port_number] [USB port]" << endl;
    		exit(0);
	}

	initUSB(argv[2]);

	pthread_t read_thread;
	pthread_t shut; // thread for shutting down the system
    	if(pthread_create(&read_thread,NULL,reading,NULL)!=0 || pthread_create(&shut,NULL,Quit,NULL)!=0) {
        	perror("read create failed");
        	exit(0);
    	}
	running = true;

	int PORT_NUMBER = atoi(argv[1]);
	start_server(PORT_NUMBER);

	pthread_join(read_thread, NULL);
	pthread_join(shut, NULL);
}

//tentatively
void parse_request(string request, string& key, string& value) {
	int start = request.find("|");
	string sub = request.substr(start + 1);
	vector<string> tokens;
	split(tokens, sub, boost::is_any_of(" "), boost::token_compress_on);
	key = tokens[1];
	value = tokens[2];
}

int start_server(int PORT_NUMBER)
{

    struct sockaddr_in server_addr,client_addr;    
      
    int sock; // socket descriptor

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
    }
    int temp;
    if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&temp,sizeof(int)) == -1) {
		perror("Setsockopt");
		exit(1);
    }

    server_addr.sin_port = htons(PORT_NUMBER); // specify port number
    server_addr.sin_family = AF_INET;         
    server_addr.sin_addr.s_addr = INADDR_ANY; 
    bzero(&(server_addr.sin_zero),8); 
      
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("Unable to bind");
	    exit(1);
    }

    if (listen(sock, 5) == -1) {
	    perror("Listen");
	    exit(1);
    }
         
    cout << endl << "Server configured to listen on port " << PORT_NUMBER << endl;
    fflush(stdout);
    
	while(running) {
	    int sin_size = sizeof(struct sockaddr_in);
	    int listenfd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
	    cout << "Server got a connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << endl;
	      
	    char request_char[1024] = {0};
	      
	    int bytes_received = recv(listenfd,request_char,1024,0);
	    request_char[bytes_received] = '\0';
	    cout << "Here comes the message:" << endl;
	    cout << request_char << endl;

	    /* parse post reqeust */
	    char* token;
	    char key[1024];
	    strtok(request_char, "\n");
	    while(token != NULL) {
	    	strcpy(key, token);
	    	token = strtok(NULL, "\n");
	    }

	    /* CF conversion */
	    if (strncmp(key, "unit", 4) == 0) setCF();

	    string reply = getJason();
	    send(listenfd, reply.c_str(), reply.size(), 0);
    	printf("Server sent message: %s\n", reply.c_str());
	}
	    
    close(sock);
    cout << "Server closed connection" << endl;
    return 0;
} 

void* Quit(void* p)
{
    string q;
    while(1)
    {
        cin >> q;
        if(q == "q")
        {
            quit();
            break;
        }
    }
    running = false;
    cout << "Request to quit!" << endl;
    return NULL;
}
