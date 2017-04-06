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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string>
#include <iostream>
using namespace std;
//??include "ReadUSB.cpp" ?

pthread_mutex_t lock;

//tentatively
void parse_request(string request, string& key, string& value) {
	int start = request.find("|");
	string sub = request.substr(start + 1);
	vector<string> tokens;
	split(token, sub, boost::is_any_of(" "), boost::token_compress_on);
	key = token[1];
	value = token[2];
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
    
	while(1) {
	    int sin_size = sizeof(struct sockaddr_in);
	    int listenfd = accept(sock, (struct sockaddr *)&client_addr,(socklen_t *)&sin_size);
	    cout << "Server got a connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << endl;
	      
	    char request_char[1024] = {0};
	      
	    int bytes_received = recv(listenfd,request_char,1024,0);
	    request_char[bytes_received] = '\0';
	    cout << "Here comes the message:" << endl;
	    cout << request_char << endl;
	    string request_str = request_char;

	    string key;
	    string value;
	    parse_request(request_str, key, value);

	    //string reply = "{\n\"name\": \"cit595\"\n}\n";
	    string reply;
	    if (key == "now") {
	    	reply = getNow();
	    }
	    if (key == "max") {
	    	reply = getMax();
	    }
	    if (key == "min") {
	    	reply = getMin();
	    }
	    if (key == "avg") {
			reply = getAvg();
	    }
	    if (key == "unit") {
	    	//pthread_mutex_lock(&lock);
	    	setCF();
	    	//pthread_mutex_unlock(&lock);
	    	//reply = "Sucess?"
	    }
	    if (key == "mode") {

	    }
  
	    send(listenfd, reply.c_str(), reply.length(), 0);
	    printf("Server sent message: %s\n", reply);
	    
	}
	//close(fd);
    close(sock);
    cout << "Server closed connection" << endl;
    return 0;
} 



int main(int argc, char *argv[])
{
	if (argc != 3)
    {
    	cout << endl << "Usage: server [port_number] [USB port]" << endl;
    	exit(0);
    }

  
	initUSB(argv[2]);
	config(fd);

	pthread_t read_thread;
	pthread_create(read_thread, NULL, reading, NULL);

	int PORT_NUMBER = atoi(argv[1]);
	start_server(PORT_NUMBER);
}

