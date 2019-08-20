#include "BoardInit.h"
#include "SPIDriver.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_usart.h>
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_hal.h>

#include <stdio.h>

// Pin constants
static GPIO_TypeDef * const		LED1_PORT		= GPIOB;
static const uint32_t			LED1_PIN		= LL_GPIO_PIN_11;
static GPIO_TypeDef * const		LED2_PORT		= GPIOB;
static const uint32_t			LED2_PIN		= LL_GPIO_PIN_10;
static GPIO_TypeDef * const		LED3_PORT		= GPIOC;
static const uint32_t			LED3_PIN		= LL_GPIO_PIN_4;
static GPIO_TypeDef * const		LED4_PORT		= GPIOA;
static const uint32_t			LED4_PIN		= LL_GPIO_PIN_4;


static void MX_GPIO_Init(void)
{
	// Enable clocking of corresponding periperhal
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);

	LL_GPIO_SetPinMode(LED1_PORT, LED1_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(LED1_PORT, LED1_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinSpeed(LED1_PORT, LED1_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetOutputPin(LED1_PORT, LED1_PIN);

	LL_GPIO_SetPinMode(LED2_PORT, LED2_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(LED2_PORT, LED2_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinSpeed(LED2_PORT, LED2_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetOutputPin(LED2_PORT, LED2_PIN);

	LL_GPIO_SetPinMode(LED3_PORT, LED3_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(LED3_PORT, LED3_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinSpeed(LED3_PORT, LED3_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetOutputPin(LED3_PORT, LED3_PIN);

	LL_GPIO_SetPinMode(LED4_PORT, LED4_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(LED4_PORT, LED4_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinSpeed(LED4_PORT, LED4_PIN, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetOutputPin(LED4_PORT, LED4_PIN);
}


int main(void)
{
	InitBoard();
	MX_GPIO_Init();

	HAL_Delay(1500);

	SPIDriver driver;

	while(true)
	{
		LL_GPIO_ResetOutputPin(LED1_PORT, LED1_PIN);
		HAL_Delay(500);
		LL_GPIO_SetOutputPin(LED1_PORT, LED1_PIN);

		printf("Initialize card\n");
		driver.initCard();

		HAL_Delay(1500);
	}
}
