#include "BoardInit.h"

#include <stm32f1xx_hal.h>

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_usart.h>

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
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	LL_GPIO_SetPinMode(LED1_PORT, LED1_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(LED1_PORT, LED1_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinSpeed(LED1_PORT, LED1_PIN, LL_GPIO_SPEED_FREQ_LOW);

	LL_GPIO_SetPinMode(LED2_PORT, LED2_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(LED2_PORT, LED2_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinSpeed(LED2_PORT, LED2_PIN, LL_GPIO_SPEED_FREQ_LOW);

	LL_GPIO_SetPinMode(LED3_PORT, LED3_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(LED3_PORT, LED3_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinSpeed(LED3_PORT, LED3_PIN, LL_GPIO_SPEED_FREQ_LOW);

	LL_GPIO_SetPinMode(LED4_PORT, LED4_PIN, LL_GPIO_MODE_OUTPUT);
	LL_GPIO_SetPinOutputType(LED4_PORT, LED4_PIN, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinSpeed(LED4_PORT, LED4_PIN, LL_GPIO_SPEED_FREQ_LOW);
}


int main(void)
{
	InitBoard();
	MX_GPIO_Init();

	while(true)
	{
		LL_GPIO_ResetOutputPin(LED1_PORT, LED1_PIN);
		HAL_Delay(500);
		LL_GPIO_ResetOutputPin(LED2_PORT, LED2_PIN);
		HAL_Delay(500);
		LL_GPIO_ResetOutputPin(LED3_PORT, LED3_PIN);
		HAL_Delay(500);
		LL_GPIO_ResetOutputPin(LED4_PORT, LED4_PIN);
		HAL_Delay(500);

		LL_USART_TransmitData8(USART1, 'A');

		LL_GPIO_SetOutputPin(LED1_PORT, LED1_PIN);
		HAL_Delay(500);
		LL_GPIO_SetOutputPin(LED2_PORT, LED2_PIN);
		HAL_Delay(500);
		LL_GPIO_SetOutputPin(LED3_PORT, LED3_PIN);
		HAL_Delay(500);
		LL_GPIO_SetOutputPin(LED4_PORT, LED4_PIN);
		HAL_Delay(500);

		LL_USART_TransmitData8(USART1, 'B');
	}
}
