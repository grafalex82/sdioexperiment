#include "SDCard.h"

#include <stdio.h>

#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_hal.h>


static GPIO_TypeDef * const		ENABLE_PIN_PORT		= GPIOA;
static const uint32_t			ENABLE_PIN_NUM		= LL_GPIO_PIN_15;


SDCard::SDCard()
{

}

void SDCard::powerUp()
{
	// Card power enable pin lives on PA15 which has to be unattached from JTAG first
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
	LL_GPIO_AF_Remap_SWJ_NOJTAG();

	// Enable SD card power by signalling low PA15
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

	LL_GPIO_SetPinMode(ENABLE_PIN_PORT, ENABLE_PIN_NUM, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(ENABLE_PIN_PORT, ENABLE_PIN_NUM, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinSpeed(ENABLE_PIN_PORT, ENABLE_PIN_NUM, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetOutputPin(ENABLE_PIN_PORT, ENABLE_PIN_NUM);
	LL_GPIO_ResetOutputPin(ENABLE_PIN_PORT, ENABLE_PIN_NUM);

	// And wait at least 1ms
	HAL_Delay(1);

	// initialize driver
	driver.init();
}

bool SDCard::init()
{
	printf("Sending soft reset command (GO IDLE)\n");
	driver.cmd0_goIdleState();

	printf("Card initialized. Checking card version\n");
	bool v2card = driver.cmd8_sendInterfaceConditions();
	printf("Card version - %d\n", v2card ? 2 : 1);

	bool sdhc = negotiateCapacity(v2card);
	printf("Card type - %s\n", sdhc ? "SDHC" : "SDSC");

	return true;
}

bool SDCard::negotiateCapacity(bool hostSupportSdhc)
{
	bool valid;
	do
	{
		driver.cmd55_sendAppCommand();
		valid = driver.acmd41_sendAppOpConditions(hostSupportSdhc);
	}
	while(!valid);

	bool sdhc = driver.cmd58_readCCS();
	return sdhc;
}
