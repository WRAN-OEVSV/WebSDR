/******************************************************************************
 * C++ source of RPX-100
 *
 * File:   SDRsamples.cpp
 * Author: Bernhard Isemann
 *
 * Created on 06 Jan 2022, 10:35
 * Updated on 30 Jul 2022, 09:20
 * Version 1.00
 *****************************************************************************/

#include "SDRsamples.h"
#include "channel.h"

using namespace std;

int SDRinit(double frequency, double sampleRate, int modeSelector, double normalizedGain)
{
    // Find devices
    int n;
    lms_info_str_t list[8]; // should be large enough to hold all detected devices
    if ((n = LMS_GetDeviceList(list)) < 0)
    {
        error(); // NULL can be passed to only get number of devices
    }
    msgSDR.str("");
    msgSDR << "Number of devices found: " << n;
    Logger(msgSDR.str()); // print number of devices
    if (n < 1)
    {
        return -1;
    }

    // open the first device
    if (LMS_Open(&device, list[0], nullptr))
    {
        error();
    }
    sleep(1);

    // Initialize device with default configuration
    if (LMS_Init(device) != 0)
    {
        error();
    }
    sleep(1);

    // Set SDR GPIO diretion GPIO0-5 to output and GPIO6-7 to input
    uint8_t gpio_dir = 0xFF;
    if (LMS_GPIODirWrite(device, &gpio_dir, 1) != 0)
    {
        error();
    }

    // Read and log GPIO direction settings
    uint8_t gpio_val = 0;
    if (LMS_GPIODirRead(device, &gpio_val, 1) != 0)
    {
        error();
    }
    msgSDR.str("");
    msgSDR << "Set GPIOs direction to output.\n";
    Logger(msgSDR.str());

    // Set GPIOs to RX mode (initial settings)
    if (LMS_GPIOWrite(device, &modeGPIO[modeSelector], 1) != 0)
    {
        error();
    }

    // Read and log GPIO values
    if (LMS_GPIORead(device, &gpio_val, 1) != 0)
    {
        error();
    }
    msgSDR.str("");
    msgSDR << "GPIO Output to High Level:\n";
    print_gpio(gpio_val);
    Logger(msgSDR.str());

    msgSDR.str("");
    msgSDR << "LimeRFE set to " << modeName[0] << endl;
    Logger(msgSDR.str());

    // Enable TX channel,Channels are numbered starting at 0
    if (LMS_EnableChannel(device, LMS_CH_RX, 0, true) != 0)
    {
        error();
    }

    // Set sample rate
    if (LMS_SetSampleRate(device, 4e6, 0) != 0)
    {
        error();
    }
    msgSDR.str("");
    msgSDR << "Sample rate: " << 4e6 / 1e6 << " MHz" << endl;
    Logger(msgSDR.str());

    // Set center frequency
    if (LMS_SetLOFrequency(device, LMS_CH_RX, 0, 52e6) != 0)
    {
        error();
    }
    msgSDR.str("");
    msgSDR << "Center frequency: " << 52e6 / 1e6 << " MHz" << endl;
    Logger(msgSDR.str());

    // select Low TX path for LimeSDR mini --> TX port 2 (misslabed in MINI, correct in USB)
    if (LMS_SetAntenna(device, LMS_CH_RX, 0, LMS_PATH_LNAL) != 0)
    {
        error();
    }

    // set TX gain
    if (LMS_SetNormalizedGain(device, LMS_CH_RX, 0, 1) != 0)
    {
        error();
    }

    // calibrate Tx, continue on failure
    LMS_Calibrate(device, LMS_CH_RX, 0, 4e6, 0);

    // Wait 12sec and send status LoRa message
    sleep(2);

    return 0;
}

int SDRfrequency(lms_device_t *device, double RXfreq, double TXfreq)
{
    // Set center frequency
    if (LMS_SetLOFrequency(device, LMS_CH_RX, 0, RXfreq) != 0)
    {
        return error();
    }
    if (LMS_SetLOFrequency(device, LMS_CH_TX, 0, TXfreq) != 0)
    {
        return error();
    }
    msgSDR.str("");
    msgSDR << "RX frequency: " << RXfreq / 1e6 << " MHz, TX frequency: " << TXfreq / 1e6 << " MHz" << endl;
    Logger(msgSDR.str());

    return 0;
}

int SDRsampleRate(lms_device_t *device, double sampleR)
{
    // Set sample rate
    if (LMS_SetSampleRate(device, sampleR, 0) != 0)
    {
        return error();
    }
    msgSDR.str("");
    msgSDR << "Sample rate: " << sampleR / 1e6 << " MHz" << endl;
    Logger(msgSDR.str());

    return 0;
}

int error()
{
    msgSDR.str("");
    msgSDR << "ERROR: " << LMS_GetLastErrorMessage();
    Logger(msgSDR.str());
    if (device != nullptr) {
        LMS_Close(device);
    }
    return -1;
}

void print_gpio(uint8_t gpio_val)
{
    for (int i = 0; i < 8; i++)
    {
        bool set = gpio_val & (0x01 << i);
        msgSDR << "GPIO" << i << ": " << (set ? "High" : "Low") << std::endl;
    }
}

void *startWebsocketServer(void *port)
{
    rpxServer es = rpxServer(*((int*)port));
    stringstream msgSOCKET;

    while (socketsON)
    {
        // Handle websocket stuff
        es.wait(TIMEOUT);

        // Handle SDR FFT
        msgSOCKET.clear();
        msgSOCKET.str("");
        msgSOCKET << "{\"center\":[";
        msgSOCKET << rxFreq;
        msgSOCKET << "],";
        msgSOCKET << "\"span\":[";
        msgSOCKET << span;
        msgSOCKET << "],";
        msgSOCKET << "\"txFreq\":[";
        msgSOCKET << txFreq;
        msgSOCKET << "],";
        msgSOCKET << "\"s\":[";

        // create spectral periodogram
        spgramcf q = spgramcf_create_default(nfft);

        // write block of samples to spectral periodogram object
        spgramcf_write(q, c_buffer, sampleCnt);

        // compute power spectral density output (repeat as necessary)
        spgramcf_get_psd(q, sp_psd);

        int i = 0;

        while (i < nfft)
        {
            msgSOCKET << to_string((int)sp_psd[i]);
            if (i < nfft - 1)
            {
                msgSOCKET << ",";
            }
            i++;
        }
        msgSOCKET << "]}";
        // msgSOCKET.seekp(-1, std::ios_base::end);
        es.broadcast(msgSOCKET.str());
        spgramcf_destroy(q);
    }

    pthread_exit(nullptr);
}

void *startSDRStream(void *threadID)
{
    // Initialize stream
    lms_stream_t streamId;                        // stream structure
    streamId.channel = 0;                         // channel number
    streamId.fifoSize = 1024 * 1024;              // fifo size in samples
    streamId.throughputVsLatency = 1.0;           // optimize for max throughput
    streamId.isTx = false;                        // RX channel
    streamId.dataFmt = lms_stream_t::LMS_FMT_F32; // 12-bit integers
    if (LMS_SetupStream(device, &streamId) != 0)
        error();

    // Start streaming
    LMS_StartStream(&streamId);

    // Start streaming
    msgSDR.str("");
    msgSDR << "SDR stream started as thread no: " << threadID << " with sampleCnt (I+Q): " << sampleCnt << endl;
    Logger(msgSDR.str());

    while (rxON)
    {
        pthread_mutex_lock(&SDRmutex);
        samplesRead = LMS_RecvStream(&streamId, buffer, sampleCnt, nullptr, 1000);
        int i = 0;

        while (i < samplesRead)
        {
            c_buffer[i] = buffer[2 * i] + buffer[2 * i + 1] * complex_i;
            i++;
        }
        pthread_mutex_unlock(&SDRmutex);
    }

    // Stop streaming
    LMS_StopStream(&streamId);            // stream is stopped but can be started again with LMS_StartStream()
    LMS_DestroyStream(device, &streamId); // stream is deallocated and can no longer be used

    // Close device
    // if (LMS_Close(device) == 0)
    // {
    //     msgSDR.str("");
    //     msgSDR << "Closed" << endl;
    //     Logger(msgSDR.str());
    // }
    // pthread_exit(NULL);
    return nullptr;
}

rpxServer::rpxServer(int port) : WebSocketServer(port)
{
}

rpxServer::~rpxServer() = default;

void rpxServer::onConnect(int socketID)
{
    const string &handle = "User #" + Util::toString(socketID);
    msgSDR.str("");
    msgSDR << "New connection: " << handle << endl;
    Logger(msgSDR.str());
}

void rpxServer::onMessage(int socketID, const string &data)
{
    // Send the received message to all connected clients in the form of 'User XX: message...'
    msgSDR.str("");
    msgSDR << "User click: " << data << endl;
    Logger(msgSDR.str());
    string cmd;
    if (data.find_first_of(':') > 0)
    {
        cmd = data.substr(0, data.find_first_of(':'));
    }
    int par = 0;
    if (data.find_first_of(':') > 0)
    {
        par = stoi(data.substr(data.find_first_of(':') + 1));
    }
    if (cmd == "band")
    {
        handle_command_band(par);
    }

    if (cmd == "bandwidth")
    {
        handle_command_bandwidth(par);
    }

    if (cmd == "channel")
    {
        handle_command_channel(par);
    }

    // const string &message = this->getValue(socketID, "handle") + ": " + data;

    // this->broadcast(message);
}
void rpxServer::handle_command_channel_single(double rxFrequency, double txFrequency, double span0) const {
    // TODO - this array gets destoyed - do we really want that?
    pthread_t threads[5];
    pthread_mutex_init(&SDRmutex, nullptr);
    rxON = false;
    sleep(1);
    if (SDRfrequency(device, Ch4_RX, Ch4_TX) != 0)
    {
        msgSDR.str("");
        msgSDR << "ERROR setting Frequency for SDR";
        Logger(msgSDR.str());
    }
    if (SDRsampleRate(device, Ch4_SR) !=0)
    {
        msgSDR.str("");
        msgSDR << "ERROR setting SampleRate for SDR";
        Logger(msgSDR.str());
    }
    rxFreq = rxFrequency;
    txFreq = txFrequency;
    span = span0;
    rxON = true;
    if (pthread_create(&threads[1], nullptr, startSDRStream, (void *)1) != 0)
    {
        msgSDR.str("");
        msgSDR << "ERROR starting thread 1";
        Logger(msgSDR.str());
    }
}
void rpxServer::handle_command_channel(int par) const {
    switch (par)
    {
        // TODO: why no channel 1-3?
        case 4:
            this->handle_command_channel_single(Ch4_RX, Ch4_TX, Ch4_SR);
            break;
        case 5:
            this->handle_command_channel_single(Ch5_RX, Ch5_TX, Ch5_SR);
            break;
        case 6:
            this->handle_command_channel_single(Ch6_RX, Ch6_TX, Ch6_SR);
            break;
        case 7:
            this->handle_command_channel_single(Ch7_RX, Ch7_TX, Ch7_SR);
            break;
        case 8:
            this->handle_command_channel_single(Ch8_RX, Ch8_TX, Ch8_SR);
            break;
        case 9:
            this->handle_command_channel_single(Ch9_RX, Ch9_TX, Ch9_SR);
            break;
        case 10:
            this->handle_command_channel_single(Ch10_RX, Ch10_TX, Ch10_SR);
            break;
        case 11:
            this->handle_command_channel_single(Ch11_RX, Ch11_TX, Ch11_SR);
            break;
        case 12:
            this->handle_command_channel_single(Ch12_RX, Ch12_TX, Ch12_SR);
            break;
        case 13:
            this->handle_command_channel_single(Ch13_RX, Ch13_TX, Ch13_SR);
            break;
        case 14:
            this->handle_command_channel_single(Ch14_RX, Ch14_TX, Ch14_SR);
            break;
        case 15:
            this->handle_command_channel_single(Ch15_RX, Ch15_TX, Ch15_SR);
            break;
        case 16:
            this->handle_command_channel_single(Ch16_RX, Ch16_TX, Ch16_SR);
            break;
        case 17:
            this->handle_command_channel_single(Ch17_RX, Ch17_TX, Ch17_SR);
            break;
        case 18:
            this->handle_command_channel_single(Ch18_RX, Ch18_TX, Ch18_SR);
            break;
        case 19:
            this->handle_command_channel_single(Ch19_RX, Ch19_TX, Ch19_SR);
            break;
        default:
            // TODO: Why number 4 as default?
            this->handle_command_channel_single(Ch4_RX, Ch4_TX, Ch4_SR);
            break;
    }
}
void rpxServer::handle_command_bandwidth_single(double spn) const {

    // TODO - this array gets destoyed - do we really want that?
    pthread_t threads[5];
    pthread_mutex_init(&SDRmutex, nullptr);
    rxON = false;
    sleep(1);
    if (SDRsampleRate(device, spn) !=0)
    {
        msgSDR.str("");
        msgSDR << "ERROR setting SampleRate for SDR";
        Logger(msgSDR.str());
    }
    span = spn;
    rxON = true;
    if (pthread_create(&threads[1], nullptr, startSDRStream, (void *)1) != 0)
    {
        msgSDR.str("");
        msgSDR << "ERROR starting thread 1";
        Logger(msgSDR.str());
    }
}
void rpxServer::handle_command_bandwidth(int par) const {
    switch (par)
    {
        case 1:
        case 2:
        case 4:
        case 8:
        case 16:
            this->handle_command_bandwidth_single(par * 1e6);
            break;
        default:
            this->handle_command_bandwidth_single(4 * 1e6);
            break;
    }
}

void rpxServer::handle_command_band(int par) const {
    pthread_t threads[5];
    switch (par)
    {
    case 1:
        this->handle_command_channel_single(Ch1_RX, Ch1_TX, Ch1_SR);
        break;

    case 2:
        this->handle_command_channel_single(Ch2_RX, Ch2_TX, Ch2_SR);
        break;

    case 3:
        this->handle_command_channel_single(Ch3_RX, Ch3_TX, Ch3_SR);
        break;

    default:
        this->handle_command_channel_single(Ch4_RX, Ch4_TX, Ch4_SR);
        break;
    }
}

void rpxServer::onDisconnect(int socketID)
{
    const string &handle = this->getValue(socketID, "handle");
    msgSDR.str("");
    msgSDR << "Disconnected: " << handle << endl;
    Logger(msgSDR.str());

    // Let everyone know the user has disconnected
    // const string &message = handle + " has disconnected.";
    // for (map<int, Connection *>::const_iterator it = this->connections.begin(); it != this->connections.end(); ++it)
    //     if (it->first != socketID)
    //         // The disconnected connection gets deleted after this function runs, so don't try to send to it
    //         // (It's still around in case the implementing class wants to perform any clean up actions)
    //         this->send(it->first, message);
}

void rpxServer::onError(int socketID, const string &message)
{
    msgSDR.str("");
    msgSDR << "Error: " << message << endl;
    Logger(msgSDR.str());
}
