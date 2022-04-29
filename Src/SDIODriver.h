#ifndef SDIODRIVER_H
#define SDIODRIVER_H

#include "IDriver.h"

class SDIODriver: public IDriver
{
public:
	SDIODriver();

    virtual void init(unsigned int prescaler);
    virtual void deinit();
    virtual void reset();
    virtual void cmd0_goIdleState();
    virtual bool cmd8_sendInterfaceConditions();
    virtual void cmd55_sendAppCommand();
    virtual bool acmd41_sendAppOpConditions(bool hostSupportSdhc);
    virtual bool cmd58_readCCS();
    virtual void cmd2_getCID();
    virtual uint16_t cmd3_getRCA();
    virtual void cmd7_selectCard(uint16_t rca);
    virtual void cmd9_getCSD(uint16_t rca);
    virtual void cmd10_getCID(uint16_t rca);

private:
    void get128BitResponse(uint8_t * resp);
};

#endif // SDIODRIVER_H
