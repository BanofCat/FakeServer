#include "device_server.h"
namespace fake 
{
/* private */

void device_server::
init()
{

}

void addTimeNS(struct timespec& t_obj, unsigned int nano_time)
{
	t_obj.tv_nsec += nano_time;
	while(t_obj.tv_nsec >= 10E9) {
		t_obj.tv_nsec -= 10E9;
		t_obj.tv_sec++;
	}
}


/* public */

int device_server::
stop(int timeout_us)
{
    unsigned int stop_counter = 0;
    this->setKeepRunning(false);
    while(this->isRunning()) {
        usleep(1000);   // sleep 1000us
        stop_counter++;
        if (stop_counter > WAIT_STOP_TIMES) {
            throw std::exception('wait to stop timeout');
        }
    }
    return OK;
}

int device_server::
run()
{
    fprintf(stderr, "start to run device server!\n");
    int ret = 0;
    try {
        struct timespec time_obj;
        this->setRunning(true);
        while(this->keepRunning()) {
            clock_gettime(CLOCK_MONOTONIC, &time_obj);  // store current time
            this->addTimeNS(time_obj, CYCLE_TIME_NS);   // calculate the target time

            gui_obj.dev_read();     // update gui data before write to rcs

            rc_obj.dev_write();

            rc_obj.dev_read();      // update rcs data before write to gui

            gui_obj.dev_write();

            clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &time_obj, NULL);   // sleep until the target time
        }

    } catch (std::exception e) {
        fprintf(stderr, "catch error: %s\n", e.what());
        ret = RUN_ERR;
    }
    this->setRunning(false);
    fprintf(stderr, "at the end of device server! \n");
    return ret;
}


}   // !namespace fake