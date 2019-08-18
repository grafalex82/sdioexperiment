#include <stm32f1xx_hal.h>
#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_usart.h>
#include <stm32f1xx_ll_bus.h>

#include <stm32f1xx_hal_rcc.h>
#include <stm32f1xx_hal_rcc_ex.h>
#include <stm32f1xx_hal_flash.h>
#include <stm32f1xx_hal_cortex.h>

#include "BoardInit.h"

// Pin constants
static GPIO_TypeDef * const		TX_PIN_PORT		= GPIOA;
static const uint32_t			TX_PIN_NUM		= LL_GPIO_PIN_9;
static GPIO_TypeDef * const		RX_PIN_PORT		= GPIOA;
static const uint32_t			RX_PIN_NUM		= LL_GPIO_PIN_10;


// Set up board clocks
void InitClock(void)
{
	// Set up external oscillator to 72 MHz
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	// Set up periperal clocking
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
								  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

	// Set up USB clock
	RCC_PeriphCLKInitTypeDef PeriphClkInit;
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
	PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

	// Set up SysTTick to 1 ms
	// TODO: Do we really need this? SysTick is initialized multiple times in HAL
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	// SysTick_IRQn interrupt configuration - setting SysTick as lower priority to satisfy FreeRTOS requirements
	HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

void InitUART()
{
	// Enable clocking of corresponding periperhal
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA | LL_APB2_GRP1_PERIPH_USART1);

	// Init pins in alternate function mode
	LL_GPIO_SetPinMode(TX_PIN_PORT, TX_PIN_NUM, LL_GPIO_MODE_ALTERNATE);	// TX pin
	LL_GPIO_SetPinSpeed(TX_PIN_PORT, TX_PIN_NUM, LL_GPIO_SPEED_FREQ_HIGH);
	LL_GPIO_SetPinOutputType(TX_PIN_PORT, TX_PIN_NUM, LL_GPIO_OUTPUT_PUSHPULL);

	LL_GPIO_SetPinMode(RX_PIN_PORT, RX_PIN_NUM, LL_GPIO_MODE_INPUT);		// RX pin

	// Prepare for initialization
	LL_USART_Disable(USART1);

	// Init
	LL_USART_SetBaudRate(USART1, HAL_RCC_GetPCLK2Freq(), 115200);
	LL_USART_SetDataWidth(USART1, LL_USART_DATAWIDTH_8B);
	LL_USART_SetStopBitsLength(USART1, LL_USART_STOPBITS_1);
	LL_USART_SetParity(USART1, LL_USART_PARITY_NONE);
	LL_USART_SetTransferDirection(USART1, LL_USART_DIRECTION_TX_RX);
	LL_USART_SetHWFlowCtrl(USART1, LL_USART_HWCONTROL_NONE);

	// Finally enable the peripheral
	LL_USART_Enable(USART1);
}

void InitBoard()
{
	// Initialize board and HAL
	HAL_Init();
	InitClock();
	InitUART();
}

extern "C"
{
	void SysTick_Handler(void) {
	  HAL_IncTick();
	  HAL_SYSTICK_IRQHandler();
	}

	extern "C"
	void Error_Handler()
	{

	}
}
