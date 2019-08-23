#ifndef SPIDRIVER_H
#define SPIDRIVER_H

#include <cstddef>
#include <stdint.h>

class SPIDriver
{
public:
	SPIDriver();

	void initConnection();
	void cmd0_goIdleState();
	bool cmd8_sendInterfaceConditions();

	void transmit(const uint8_t * buf, size_t len);
	void receive(uint8_t * buf, size_t len);

private:
	static uint8_t CRC7(uint8_t * buf, size_t len);
	void transmitByte(uint8_t byte);
	uint8_t receiveByte();
	void sendCommand(int cmd, int arg);
	void selectCard();
	void deselectCard();
};

#endif // SPIDRIVER_H
