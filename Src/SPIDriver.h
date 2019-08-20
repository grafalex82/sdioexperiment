#ifndef SPIDRIVER_H
#define SPIDRIVER_H

#include <cstddef>
#include <stdint.h>

class SPIDriver
{
public:
	SPIDriver();

	bool initCard();

	void transmit(const uint8_t * buf, size_t len);
	void receive(uint8_t * buf, size_t len);

private:
	void transmitByte(uint8_t byte);
	uint8_t receiveByte();
	void selectCard();
	void deselectCard();
};

#endif // SPIDRIVER_H
