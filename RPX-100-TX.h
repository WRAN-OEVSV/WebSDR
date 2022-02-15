/******************************************************************************
 * C++ source of RPX-100-TX
 *
 * File:   RPX-100-TX.h
 * Author: Bernhard Isemann
 *         Marek Honek
 *
 * Created on 19 Sep 2021, 12:37
 * Updated on 10 Feb 2022, 17:00
 * Version 2.00
 *****************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sstream>
#include <syslog.h>
#include <string.h>
#include <iostream>
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
#include "alsa/asoundlib.h"
#include "liquid/liquid.h"
#include "sockets/ServerSocket.h"
#include "sockets/SocketException.h"
#include <iterator>
#pragma once

#define NUM_THREADS 5 // max number of main threads
pthread_mutex_t SDRmutex;

// SDR facility

int SDRinit(double frequency, double sampleRate, int modeSelector, double normalizedGain);
int SDRinitTX(double frequency, double sampleRate, int modeSelector, double normalizedGain);
int SDRfrequency(lms_device_t *device, double frequency);
void *startSocketServer(void *threadID);
void *startSDRStream(void *threadID);
void *startSocketConnect(void *threadID);
void *startWebsocketServer(void *threadID);
void *sendBeacon(void *threadID);
int error();

// Log facility
void print_gpio(uint8_t gpio_val);
std::stringstream msg;
std::stringstream HEXmsg;

// SDR values
double frequency = 52.8e6;
double sampleRate = 2e6;
int modeSelector;
double normalizedGain = 1;
string mode = "TX6m";
int modeSel = 2;