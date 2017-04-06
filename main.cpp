#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <pthread.h>
using namespace std;

int initUSB(char *argv);
void* reading(void* p);
void quit();
void setCF();
double getNow();
double getAvg();
double getMax();
double getMin();

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

    while(running)
    {
        cout<<"Real time temp: "<<getNow()<<"   average temp: "<<getAvg()<<"   max: "<<getMax()<<"   min: "<<getMin()<<endl;
    }

    pthread_join(read, NULL);
    pthread_join(shut, NULL);
    cout << "System shut down." << endl;
}
