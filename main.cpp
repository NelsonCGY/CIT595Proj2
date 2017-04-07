#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <pthread.h>
#include <ctime>
using namespace std;

int initUSB(char *argv);
void* reading(void* p);
void quit();
void setCF();
string getJason();
bool canGetT();

bool running;

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

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cout << "Please specify the name of the serial port (USB) device file and port number!" << endl;
        exit(0);
    }
    char* file_name = argv[1];
    initUSB(file_name);
    pthread_t read; // thread for reading from sensor
    pthread_t shut; // thread for shutting down the system
    if(pthread_create(&read,NULL,reading,NULL)!=0 || pthread_create(&shut,NULL,Quit,NULL)!=0)
    {
        perror("read create failed");
        exit(0);
    }
    running = true;

    time_t now, pre = time(NULL);
    while(running)
    {
        now = time(NULL);
        if(now - pre < 2)
        {
            continue;
        }
        pre = now;
        if(!canGetT())
        {
            cout << "Disconnected!" << endl;
        }
        else
        {
            cout << "Send response:\n" << getJason() << endl;
        }
    }

    pthread_join(read, NULL);
    pthread_join(shut, NULL);
    cout << "System shut down." << endl;
}
