#include <avr/io.h>
#include <avr/interrupt.h>

#include <idcells.h>
#include <server.h>

#define MULTICAST_ADDR 0x05
#define INIT_ADDR 0x21

#define VER(MAJOR, MINOR) ((uint16_t)((MAJOR << 8)|(MINOR)))
#define DEV_NAME {'t', 'e', 's', 't'}
#define DEV_SW_VER VER(1, 5)
#define DEV_HW_VER VER(0, 1)

#define readByte(word, addr) ((uint8_t)((word >> (addr * 8)) & 0xFF));

template<typename TypeSize>
int8_t writeWord(TypeSize & staticReg, uint8_t addr, uint8_t data)
{
    if(addr == 0) staticReg = 0;
    staticReg |= (TypeSize)(data << (addr * 8));
    if(addr == sizeof(staticReg) - 1) return OK;
    return ERR;
}

#define setbit(port,num,onoff) port=(onoff<<num)|(port&~(1<<num))

#define DDR_LED DDRB
#define PORT_LED PORTB
#define PINNUM_LED 1
#define LED_OFFSTATE false

class LED : public IMemory
{
public:
    Error write(Address addr, uint8_t data, Num num)
    {
        DDR_LED |= _BV(PINNUM_LED);
        setbit(PORT_LED, PINNUM_LED, (bool)data);
        return OK;
    }
    ReadType read(Address addr, Num num)
    {
        return (PORT_LED & _BV(PINNUM_LED));
    }
    size_t size()
    {
        return 1;
    }
};

class ListTest : public Composite<CommonShared, LED>
{
public:
    ListTest(ISettingsGeneral * settings) : Composite(&shared, &led),
        shared(settings) {}

    void setNetworkObject(ISlaveAddress * netIface)
    {
        shared.setNetworkObject(netIface);
    }


private:
    CommonShared shared;
    LED led;

};

class Settings : public ISettingsGeneral
{
public:
    Settings() :
        guid{'b', 'y', ' ', 'S', '.', 'D', '.', ' ',
             0x73, 0xf4, 0x23, 0x75, 0x96, 0xcd, 0x4b, 0xe5
            }, name DEV_NAME {
    }
    uint8_t getDeviceGUID(uint8_t pos) const
    {
        return guid[pos];
    }
    uint8_t getDeviceName(uint8_t pos) const
    {
        return name[pos];
    }
    uint8_t getDeviceSWver(uint8_t pos) const
    {
        return readByte(DEV_SW_VER, pos);
    }
    uint8_t getDeviceHWver(uint8_t pos) const
    {
        return readByte(DEV_HW_VER, pos);
    }

private:
    uint8_t guid[16];
    char name[DEVNAME_SIZE];
};

class SlaveAddressKeeper : public ISlaveAddress
{
public:
    SlaveAddressKeeper(I2CAddress addr) : addr(addr) {}
    void setAddress(I2CAddress addr)
    {
        this->addr = addr;
    }
    I2CAddress getAddress() const
    {
        return this->addr;
    }
private:
    I2CAddress addr;
};

int main(int argc, char * argv[])
{

    Settings settings;
    ListTest  memory(&settings);
    SlaveAddressKeeper addr(INIT_ADDR);


    I2CSlaveServer server(&memory);
    UsiTwiSlave network(USI::instance(), &server, &addr, MULTICAST_ADDR);
    memory.setNetworkObject(&network);
    network.init();

    sei();

    while(1);

}
