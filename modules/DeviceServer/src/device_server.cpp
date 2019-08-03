#include "device_server.h"
namespace fake 
{

device_server::
init()
{

}


int device_server::
run()
{
    try {

        rc_obj.dev_write();
        
        rc_obj.dev_read();


    } catch (Exception e) {

    }
}


}   // !namespace fake