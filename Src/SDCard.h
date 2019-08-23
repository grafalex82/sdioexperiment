#ifndef SDCARD_H
#define SDCARD_H

#include "SPIDriver.h"

class SDCard
{
public:
	SDCard();

	bool init();

private:
	SPIDriver driver;
};

#endif // SDCARD_H
