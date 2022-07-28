/******************************************************************************
 * C++ source of RPX-100S
 *
 * File:   SDRsamples.h
 * Author: Bernhard Isemann
 *
 * Created on 06 Jan 2022, 12:37
 * Updated on 07 Jan 2022, 17:00
 * Version 1.00
 *****************************************************************************/

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <syslog.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <cstdio>
#include <ctime>
#include <math.h>
#include <complex.h>
#include <time.h>
#include <chrono>
#include <cstring>
#include <bitset>
#include "ini.h"
#include "log.h"
#include "lime/LimeSuite.h"
#include <chrono>
#include <math.h>
#include "liquid/liquid.h"
#include "sockets/ServerSocket.h"
#include "sockets/SocketException.h"
#include <iterator>
#include <signal.h>
#include <pthread.h>
#include "Util.h"
#include "WebSocketServer.h"


#pragma onces

#define NUM_CONNECTS 5 // max number of sockets connections
#define PORT_NUMBER 8084
#define TIMEOUT 500

extern pthread_mutex_t SDRmutex;

// SDR facility
lms_device_t *device = NULL;
int SDRinit(double frequency, double sampleRate, int modeSelector, double normalizedGain);
int SDRfrequency(lms_device_t *device, double frequency);
string exec(string command);
void *startSocketServer(void *threadID);
void *startSDRStream(void *threadID);
void *startSocketConnect(void *threadID);
void *startWebsocketServer(void *threadID);
int error();
extern double frequency;
extern double sampleRate;
extern int modeSelector;
extern double normalizedGain;


// Radio Frontend - Define GPIO settings for CM4 hat module
uint8_t setRX = 0x01;       // GPIO0=LOW - RX, GPIO3=HIGH - PTT off,
uint8_t setTXDirect = 0x02; // GPIO0=HIGH - TX, GPIO3=HIGH - PTT off, GPIO1=HIGH, GPIO2=HIGH
uint8_t setTX6m = 0x06;     // GPIO0=HIGH - TX, GPIO3=HIGH - PTT off, GPIO1=LOW, GPIO2=LOW
uint8_t setTX2m = 0x0E;     // GPIO0=HIGH - TX, GPIO3=HIGH - PTT off, GPIO1=LOW, GPIO2=HIGH
uint8_t setTX70cm = 0x0B;   // GPIO0=HIGH - TX, GPIO3=HIGH - PTT off, GPIO1=HIGH, GPIO2=LOW

uint8_t setTXDirectPTT = 0x07; // GPIO0=HIGH - TX, GPIO3=LOW - PTT on, GPIO1=HIGH, GPIO2=HIGH
uint8_t setTX6mPTT = 0x05;     // GPIO0=HIGH - TX, GPIO3=LOW - PTT on, GPIO1=LOW, GPIO2=LOW
uint8_t setTX2mPTT = 0x01;     // GPIO0=HIGH - TX, GPIO3=LOW - PTT on, GPIO1=LOW, GPIO2=HIGH
uint8_t setTX70cmPTT = 0x03;   // GPIO0=HIGH - TX, GPIO3=LOW - PTT on, GPIO1=HIGH, GPIO2=LOW

string modeName[9] = {"RX", "TXDirect", "TX6m", "TX2m", "TX70cm", "TXDirectPTT", "TX6mPTT", "TX2mPTT", "TX70cmPTT"};
uint8_t modeGPIO[9] = {setRX, setTXDirect, setTX6m, setTX2m, setTX70cm, setTXDirectPTT, setTX6mPTT, setTX2mPTT, setTX70cmPTT};

// Log facility
void print_gpio(uint8_t gpio_val);
std::stringstream msgSDR;
std::stringstream HEXmsgSDR;


// Initialize sdr buffers
const int sampleCnt = 1048;  // complex samples per buffer --> a "sample" is I + Q values in float or int
float buffer[sampleCnt * 2]; // buffer to hold samples (each I + Q) --> buffer size = 2 * no of samples
liquid_float_complex c_buffer[sampleCnt]; // complex buffer to hold SDR sample in complex domain
liquid_float_complex complex_i(0,1);
int samplesRead = 1048;

// Initialize buffer for spectogram
const int nfft = 512;
liquid_float_complex c_sp_buf[sampleCnt]; // complex buffer to hold spectogram data result
float sp_psd[nfft];
int colormap = 3;
double step = sampleRate / (nfft);

bool rxON = true;

// Socket Server facility
int RPX_port = 5254;
string RPX_host = "10.0.0.5";
ServerSocket RPX_socket[NUM_CONNECTS];
int ConCurSocket;
bool socketsON = true;

// For any real project this should be defined separately in a header file
class rpxServer : public WebSocketServer
{
public: 
    rpxServer( int port );
    ~rpxServer( );
    virtual void onConnect(    int socketID                        );
    virtual void onMessage(    int socketID, const string& data    );
    virtual void onDisconnect( int socketID                        );
    virtual void   onError(    int socketID, const string& message );
};