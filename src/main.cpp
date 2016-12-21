#include <avr/interrupt.h>

#include <idcells.h>
#include <server.h>

#define MULTICAST_ADDR 0x50


class MemoreTest : public IMemory
{
public:
    int8_t write(uint8_t addr, uint8_t data)
    {
        return OK;
    }
    int16_t read(uint8_t addr)
    {
        return '-';
    }
    uint16_t size()
    {
        return 256;
    }
};



int main(int argc, char * argv[])
{
    MemoreTest test;
    I2CSlaveServer server(&test);
    SlaveAddressKeeper addr(0x20);
    UsiTwiSlave network(USI::instance(), &server, &addr, MULTICAST_ADDR);

    network.init();

    sei();

    while(1);

}
