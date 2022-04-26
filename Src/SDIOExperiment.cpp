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

void reinitDriver(IDriver * newDriver, int prescaler)
{
    driver->deinit();

    // Switch the driver, if necessary
    if(driver != newDriver)
    {
        driver->deinit();
        cardPowerCycle();
        driver = newDriver;
    }

    driver->init(prescaler);
    printf("OK\n");
}

void cmdSpiInit(const char * argument)
{
    // Expect argument that is fPCLK prescaler
    int prescaler = atoi(argument);
    reinitDriver(&spi, prescaler);
}

void cmdSdioInit(const char * argument)
{
    // Expect argument that is fPCLK prescaler
    int prescaler = atoi(argument);
    reinitDriver(&sdio, prescaler);
}

void cmdReset(const char *)
{
    driver->reset();
    printf("OK\n");
}

void cmd0_goIdleState(const char *)
{
    driver->cmd0_goIdleState();
    printf("OK\n");
}

void cmd8_sendInterfaceConditions(const char *)
{
    bool v2card = driver->cmd8_sendInterfaceConditions();
    printf("Card version - %d\n", v2card ? 2 : 1);
    printf("OK %d\n", v2card ? 2 : 1);
}

void cmd55_sendAppCommand(const char *)
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

void cmd58_readCCS(const char *)
{
    bool ccs = driver->cmd58_readCCS();
    printf("OK %s\n", ccs ? "SDHC" : "SDSC");
}

void cmd2_getCID(const char *)
{
    driver->cmd2_getCID();
    printf("OK\n");
}

void cmd3_getRCA(const char *)
{
    uint16_t rca = driver->cmd3_getRCA();
    printf("OK %d\n", rca);
}

struct CommandEntry
{
    const char * command;
    void (*handler)(const char * argument);
};

CommandEntry commandHandlers[] = {
    {"LOOPBACK",    cmdLoopback},
    {"SPI_INIT",    cmdSpiInit},
    {"SDIO_INIT",   cmdSdioInit},
    {"RESET",       cmdReset},
    {"CMD0",        cmd0_goIdleState},
    {"CMD8",        cmd8_sendInterfaceConditions},
    {"CMD55",       cmd55_sendAppCommand},
    {"ACMD41",      acmd41_sendAppOpConditions},
    {"CMD58",       cmd58_readCCS},
    {"CMD2",        cmd2_getCID},
    {"CMD3",        cmd3_getRCA}
};

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
    bool handled = false;
    for(size_t i=0; i<sizeof(commandHandlers)/sizeof(CommandEntry) && cmdLen != 0; i++)
    {
        if(!strncmp(commandHandlers[i].command, buf, cmdLen))
        {
            commandHandlers[i].handler(ptr);
            handled = true;
            break;
        }
    }

    if(!handled)
        printf("ERROR Unknown command: %s\n", buf);
}

int main(void)
{
    initBoard();
    initLEDs();

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
