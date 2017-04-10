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
#include <ctime>
#include <vector>
#include <string.h>
using namespace std;

int initUSB(string argv);
void* reading(void* p);
void quit();
void setCF();
string getJson();
bool canGetT();

pthread_t read_thread;
pthread_t shut; // thread for shutting down the system

int start_server(int);
void* Quit(void*);

bool running;

int main(int argc, char *argv[])
{
	if (argc != 3) {
    		cout << endl << "Usage: server [port_number] [USB port]" << endl;
    		exit(0);
	}

	if (pthread_create(&shut,NULL,Quit,NULL)!=0) {
		perror("create shut failed");
		exit(0);
	}

	string var = argv[2];
	initUSB(var);

	if(pthread_create(&read_thread,NULL,reading,NULL)!=0) {
    	perror("read create failed");
    	exit(0);
	}

	running = true;

	int PORT_NUMBER = atoi(argv[1]);
	start_server(PORT_NUMBER);

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
	    cout << "Here comes the message:" << endl;
		
	    char line[1024] = {0};
	    char len_char[256] = {0};
	    string content;
	    FILE *fp = fdopen(listenfd, "r");
	    if (fp != NULL) {
			while(fgets(line, 1024, fp) != NULL) {
			    cout << line;
			    if (strncmp(line, "Content-Length", 14) == 0) {
		    		strtok(line, " ");
					strcpy(len_char, strtok(NULL, " "));
					break;
			    }
			    bzero(line, 1024);
			}
	    }
	
	    /* CF conversion */
	    if (atoi(len_char) == 6) setCF();

	    cout << "after setCF\n";
	    string reply = getJson();
	    printf("Json Got\n");

	    int bytes_sent;
	    if ((bytes_sent = send(listenfd, reply.c_str(), reply.size(), 0)) > 0)
	    	printf("Success\n");;
    	printf("Server sent message: %s\n", reply.c_str());
    	close(listenfd);
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
    pthread_join(read_thread, NULL);
    cout << "System shut down." << endl;
    exit(0);
}
