#include <stm32f1xx_hal.h>
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_ll_rcc.h>
#include <stm32f1xx_ll_system.h>
#include <stm32f1xx_ll_utils.h>
#include <stm32f1xx_ll_cortex.h>
#include <stm32f1xx_ll_gpio.h>

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
    // Init external oscillator
    LL_RCC_HSE_Enable();
    while(!LL_RCC_HSE_IsReady())
        ;

    // Init PLL at 48 MHz
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_6);
    LL_RCC_PLL_Enable();
    while(!LL_RCC_PLL_IsReady())
        ;


    // Set up periperal clocking
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
    LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

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

