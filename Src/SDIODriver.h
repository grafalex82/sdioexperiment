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

private:
};

#endif // SDIODRIVER_H
