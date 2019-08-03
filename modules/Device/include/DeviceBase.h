#ifndef __DEVICE_DEVICE_BASE_H__
#define __DEVICE_DEVICE_BASE_H__
namespace fake
{
class DeviceBase
{
public:
    virtual int dev_read(void *data, unsigned int start, unsigned int len_byte) = 0;
    virtual int dev_write(void *data, unsigned int start, unsigned int len_byte) = 0;
}

}   // ！namespace fake



#endif // !__DEVICE_DEVICE_BASE_H__