#ifndef SDCARD_H
#define SDCARD_H

#define USE_SPI

#ifdef USE_SPI
#include "SPIDriver.h"
typedef SPIDriver SDDriver;
#else //USE_SPI
#include "SDIODriver.h"
typedef SDIODriver SDDriver;
#endif //USE_SPI


void cardPowerUp();

class SDCard
{
public:
	SDCard();

	bool init();

private:
	bool negotiateCapacity(bool hostSupportSdhc);

	SDDriver driver;
};

#endif // SDCARD_H
