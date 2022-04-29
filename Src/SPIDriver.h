#ifndef SPIDRIVER_H
#define SPIDRIVER_H

#include <cstddef>
#include <stdint.h>

#include "IDriver.h"

class SPIDriver: public IDriver
{
public:
	SPIDriver();

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
    void transmit(const uint8_t * buf, size_t len);
    void receive(uint8_t * buf, size_t len);
    void readData(uint8_t * buf, size_t len);

	static uint8_t CRC7(uint8_t * buf, size_t len);
	void transmitByte(uint8_t byte);
	uint8_t receiveByte();
	uint8_t waitForR1();
	uint32_t waitForR3R7();
	void sendCommand(int cmd, int arg);
	void selectCard();
	void deselectCard();
};

#endif // SPIDRIVER_H
