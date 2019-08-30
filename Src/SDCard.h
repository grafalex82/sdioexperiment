#ifndef SDCARD_H
#define SDCARD_H

//#define USE_SPI

#ifdef USE_SPI
#include "SPIDriver.h"
#else //USE_SPI
#include "SDIODriver.h"
#endif //USE_SPI

class SDCard
{
public:
	SDCard();

	void powerUp();
	bool init();

#ifdef USE_SPI
	SPIDriver driver;
#else //USE_SPI
	SDIODriver driver;
#endif //USE_SPI
};

#endif // SDCARD_H
