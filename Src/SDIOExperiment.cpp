#include "BoardInit.h"
#include "SDCard.h"

#include <stm32f1xx_ll_gpio.h>
#include <stm32f1xx_ll_usart.h>
#include <stm32f1xx_ll_bus.h>
#include <stm32f1xx_hal.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UartUtils.h"
#include "SPIDriver.h"
#include "SDIODriver.h"

SPIDriver spi;
SDIODriver sdio;
IDriver * driver = &spi;

// Pin constants
static GPIO_TypeDef * const		LED1_PORT		= GPIOB;
static const uint32_t			LED1_PIN		= LL_GPIO_PIN_11;
static GPIO_TypeDef * const		LED2_PORT		= GPIOB;
static const uint32_t			LED2_PIN		= LL_GPIO_PIN_10;
static GPIO_TypeDef * const		LED3_PORT		= GPIOC;
static const uint32_t			LED3_PIN		= LL_GPIO_PIN_4;
static GPIO_TypeDef * const		LED4_PORT		= GPIOA;
static const uint32_t			LED4_PIN		= LL_GPIO_PIN_4;


static void initLEDs(void)
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

void cmdLoopback(const char * argument)
{
    if(*argument == '\0')
        printf("OK\n");
    else
        printf("%s\n", argument);
}

void cmdSpiInit(const char * argument)
{
    // Expect argument that is fPCLK prescaler
    int prescaler = atoi(argument);

    driver = &spi;
    driver->init(prescaler);
    printf("OK\n");
}

void cmdSdioInit(const char * argument)
{
    // Expect argument that is fPCLK prescaler
    int prescaler = atoi(argument);

    driver = &sdio;
    driver->init(prescaler);
    printf("OK\n");
}

void cmdReset()
{
    driver->reset();
    printf("OK\n");
}

void cmd0_goIdleState()
{
    driver->cmd0_goIdleState();
    printf("OK\n");
}

void cmd8_sendInterfaceConditions()
{
    bool v2card = driver->cmd8_sendInterfaceConditions();
    printf("Card version - %d\n", v2card ? 2 : 1);
    printf("OK %d\n", v2card ? 2 : 1);
}

void cmd55_sendAppCommand()
{
    driver->cmd55_sendAppCommand();
    printf("OK\n");
}

void acmd41_sendAppOpConditions(const char * argument)
{
    // Expect argument that is SDHC support from the host side
    int sdhc = atoi(argument);

    bool valid = driver->acmd41_sendAppOpConditions(sdhc);
    printf("OK %s\n", valid ? "Valid" : "Busy");
}

void cmd58_readCCS()
{
    bool ccs = driver->cmd58_readCCS();
    printf("OK %s\n", ccs ? "SDHC" : "SDSC");
}

void parseCommand(const char * buf)
{
    printf("CMD: %s\n", buf);

    // Tokenize command
    const char * ptr = buf;
    while(*ptr != ' ' && *ptr != '\0')
        ptr++;

    size_t cmdLen = ptr - buf;

    // Search for argument start
    while(*ptr == ' ' && *ptr != '\0')
        ptr++;

    // Dispatch the command
    if(!strncmp("LOOPBACK", buf, cmdLen))
        cmdLoopback(ptr);
    else
    if(!strncmp("SPI_INIT", buf, cmdLen))
        cmdSpiInit(ptr);
    else
    if(!strncmp("SDIO_INIT", buf, cmdLen))
        cmdSdioInit(ptr);
    else
    if(!strncmp("RESET", buf, cmdLen))
        cmdReset();
    else
    if(!strncmp("CMD0", buf, cmdLen))
        cmd0_goIdleState();
    else
    if(!strncmp("CMD8", buf, cmdLen))
        cmd8_sendInterfaceConditions();
    else
    if(!strncmp("CMD55", buf, cmdLen))
        cmd55_sendAppCommand();
    else
    if(!strncmp("ACMD41", buf, cmdLen))
        acmd41_sendAppOpConditions(ptr);
    else
    if(!strncmp("CMD58", buf, cmdLen))
        cmd58_readCCS();
    else
        printf("ERROR Unknown command: %s\n", buf);
}

int main(void)
{
    initBoard();
    initLEDs();
    cardPowerUp();

    HAL_Delay(500);
    printf("============== Let the experiment begin ==============\n");

    while(true)
    {
        char buf[80];
        readLine(buf, 80);
        parseCommand(buf);
    }

//    SDCard card;

//    while(true)
//    {
//        LL_GPIO_ResetOutputPin(LED1_PORT, LED1_PIN);
//        HAL_Delay(500);
//        LL_GPIO_SetOutputPin(LED1_PORT, LED1_PIN);

//        printf("Initialize card\n");
//        card.init();
//        printf("Card initialized successfully\n");

//        HAL_Delay(2000);
//    }
}
