#ifndef __DEVICE_SERVER_DEVICE_SERVER_H__
#define __DEVICE_SERVER_DEVICE_SERVER_H__

namespace fake
{

#define CYCLE_TIME_US 2000 // 2000us
#define CYCLE_TIME_NS 1000 * CYCLE_TIME_US // 2000us = 2000 000 ns
#define WAIT_STOP_TIMES 1000    // wait to stop max times, when exceed it, raise exception

enum ErrorCode {
    OK      = 0,
    RUN_ERR = -1,
    TIMEOUT = -2,
};

class DeviceServer
{
private:
    bool keep_running;  // if need to keep running
    bool is_running;    // if is running now
    void addTimeNS(struct timespec& t_obj, unsigned int nano_time);   
    void setRunning(bool new_state);

public:
    bool keepRunning();
    void setKeepRunning(bool new_state);
    bool isRunning();
    int run();
    int stop();


};  // !class DeviceServer
}   // !namespace fake
#endif  // !__DEVICE_SERVER_DEVICE_SERVER_H__