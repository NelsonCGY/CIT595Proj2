#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <pthread.h>
#include <ctime>
using namespace std;

int initUSB();
void* reading(void* p);
void quit();
void setCF();
void showTime();
string getJson();
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
        else if(q == "s")
        {
            setCF();
        }
        else if(q == "t")
        {
            showTime();
        }
    }
    running = false;
    cout << "\nRequest to quit!" << endl;
    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "Please specify port number!" << endl;
        exit(0);
    }
    initUSB();
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
        if(now - pre < 1)
        {
            continue;
        }
        pre = now;
        if(!canGetT())
        {
            cout << "\nDisconnected!" << "\nSend response:\n" << getJson() << endl;
        }
        else
        {
            cout << "\nSend response:\n" << getJson() << endl;
        }
    }

    pthread_join(read, NULL);
    pthread_join(shut, NULL);
    cout << "\nSystem shut down." << endl;
}
