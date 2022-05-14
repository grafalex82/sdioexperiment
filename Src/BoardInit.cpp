#include <stm32f1xx_hal.h>
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_rcc.h>
#include <stm32f1xx_ll_utils.h>
#include <stm32f1xx_ll_cortex.h>
#include <stm32f1xx_ll_gpio.h>

#include <stm32f1xx_hal_rcc.h>
#include <stm32f1xx_hal_rcc_ex.h>
#include <stm32f1xx_hal_flash.h>
#include <stm32f1xx_hal_cortex.h>

#include "BoardInit.h"
#include "UartUtils.h"


namespace {
    volatile uint32_t tick;
    const uint32_t SYS_TICK_FREQ = 100000;
}

// Set up board clocks
void initClock(void)
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
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
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

    // SysTick_IRQn interrupt configuration - setting SysTick as lower priority
    HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

void initSysTick()
{
    LL_RCC_ClocksTypeDef clocks;
    LL_RCC_GetSystemClocksFreq(&clocks);

    // Run SysTick timer at 10kHz frequency
    LL_InitTick(clocks.SYSCLK_Frequency, SYS_TICK_FREQ);

    tick = 0;

    LL_SYSTICK_EnableIT();
}

void initBoard()
{
    // Initialize board and HAL
    HAL_Init();
    initClock();
    initSysTick();
    initUART();
}

void delayMs(uint32_t ms)
{
    uint32_t tstart = getTick();
    uint32_t tend = tstart + ms * SYS_TICK_FREQ / 1000;
    while(getTick() < tend)
        ;
}

void delayUs(uint32_t us)
{
    uint32_t tstart = getTick();
    uint32_t tend = tstart + us * SYS_TICK_FREQ / 1000000;

    while(getTick() < tend)
        ;
}

uint32_t getTick()
{
    return tick;
}

uint32_t getTickFreq()
{
    return SYS_TICK_FREQ;
}

extern "C"
void SysTick_Handler(void) {
    tick++;
}

extern "C"
void Error_Handler()
{
    printf("Critical error!!!");
    while(true)
        ;
}

