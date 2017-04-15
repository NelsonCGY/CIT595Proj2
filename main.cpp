#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <pthread.h>
#include <ctime>
using namespace std;

int initUSB(string argv);
void* reading(void* p);
void quit();
void setCF();
void showTime();
void setStandby();
void setThreshold(bool up);
string getJson();
bool canGetT();

pthread_t read_thread; // thread for reading from sensor
pthread_t shut; // thread for shutting down the system

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
        else if(q == "b")
        {
            setStandby();
        }
        else if(q == "u")
        {
            setThreshold(true);
        }
        else if(q == "d")
        {
            setThreshold(false);
        }
    }
    running = false;
    cout << "\nRequest to quit!" << endl;
    pthread_join(read_thread, NULL);
    cout << "\nSystem shut down." << endl;
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cout << "Please specify USB port number and network port number!" << endl;
        exit(0);
    }

    if(pthread_create(&shut,NULL,Quit,NULL)!=0)
    {
        perror("thread create failed");
        exit(1);
    }
    string var = argv[2];
    initUSB(var);

    if(pthread_create(&read_thread,NULL,reading,NULL)!=0)
    {
        perror("thread create failed");
        exit(1);
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

}
