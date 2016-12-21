#include <avr/interrupt.h>

#include <idcells.h>
#include <server.h>

#define MULTICAST_ADDR 0x30
#define INIT_ADDR 0x20


class MemoreTest : public IMemory
{
public:
    int8_t write(uint8_t addr, uint8_t data)
    {
        return CommonShared::write(addr, data);
    }
    int16_t read(uint8_t addr)
    {
        return CommonShared::read(addr);
    }
    uint16_t size()
    {
        return CommonShared::size();
    }

};

class Settings : public ISettingsGeneral
{
public:
    Settings()
    {
        //guid = {"gkrocjqclfhydnge"};
    }
    uint8_t getDeviceGUID(uint8_t pos) const
    {
        return guid[pos];
    }
    uint8_t getDeviceName(uint8_t pos) const
    {
        return 'n';
    }
    uint8_t getDeviceSWver(uint8_t pos) const
    {
        return 's';
    }
    uint8_t getDeviceHWver(uint8_t pos) const
    {
        return 'h';
    }

private:
    uint8_t guid[16];
};


int main(int argc, char * argv[])
{
    MemoreTest test;
    I2CSlaveServer server(&test);
    SlaveAddressKeeper addr(INIT_ADDR);
    UsiTwiSlave network(USI::instance(), &server, &addr, MULTICAST_ADDR);

    Settings settings;
    CommonShared::setSettings(&settings, &network);

    network.init();

    sei();

    while(1);

}
