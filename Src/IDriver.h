#ifndef IDRIVER_H
#define IDRIVER_H

#include <stdint.h>

class IDriver
{
public:
    virtual void init(unsigned int prescaler) = 0;
    virtual void deinit() = 0;
    virtual void reset() = 0;
    virtual void cmd0_goIdleState() = 0;
    virtual bool cmd8_sendInterfaceConditions() = 0;
    virtual void cmd55_sendAppCommand() = 0;
    virtual bool acmd41_sendAppOpConditions(bool hostSupportSdhc) = 0;
    virtual bool cmd58_readCCS() = 0;

    virtual void cmd2_getCID() = 0;
    virtual uint16_t cmd3_getRCA() = 0;
};

#endif // IDRIVER_H
